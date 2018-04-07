#include <sys/uio.h>
#include"Buffer.h"

namespace net
{

    Buffer::Buffer()noexcept
            : _read_index(0)
              , _write_index(0)
    {
    }

    std::pair<ssize_t, int> Buffer::read_from_fd(int fd)
    {
        char extrabuf[65536];
        struct iovec vec[2];

        const size_t w_size = get_writable_size();

        vec[0].iov_base = get_write_ptr();
        vec[0].iov_len = w_size;
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof extrabuf;

        const int count = (w_size < sizeof extrabuf) ? 2 : 1;

        std::pair<ssize_t, int> result;
        result.first = ::readv(fd, vec, count);

        if (result.first < 0) {
            result.second = errno;
        }
        else if (static_cast<size_t>(result.first) <= w_size) {
            has_write(static_cast<size_t>(result.first));
        }
        else {
            has_write(w_size);
            append(extrabuf, result.first - w_size);
        }

        return result;
    }

    void Buffer::ensure_writable_bytes(size_t len)
    {
        if (get_writable_size() < len) {
            if (get_writable_size() + _read_index < len) {
                _buff.resize(_write_index + len);
            }
            else {
                std::copy(_buff.begin() + _read_index, _buff.begin() + _write_index, _buff.begin());
                _write_index -= _read_index;
                _read_index = 0;
            }
        }
    }

    size_t Buffer::get_writable_size() const noexcept
    {
        assert(_buff.size()>=_write_index);
        return _buff.size() - _write_index;
    }

    size_t Buffer::length() const noexcept
    {
        assert(_write_index >= _read_index);
        return _write_index - _read_index;
    }

    void Buffer::clear()noexcept
    {
        _read_index = 0;
        _write_index = 0;
    }

    void Buffer::append(const char *data, size_t len)
    {
        assert(data != nullptr);

        ensure_writable_bytes(len);

        std::copy_n(data, len, _buff.begin() + _write_index);
        has_write(len);
    }

    void Buffer::append(const std::string &str)
    {
        append(str.data(), str.size());
    }

    void Buffer::append(const Buffer &buff)
    {
        append(buff.get_read_ptr(), buff.get_readable_size());
    }

    void Buffer::append(std::initializer_list<std::pair<const char *, std::size_t>> args)
    {
        std::size_t len = 0;
        for (auto &t:args) {
            assert(t.first != nullptr);
            len += t.second;
        }

        ensure_writable_bytes(len);

        for (auto &t:args) {
            std::copy_n(t.first, t.second, _buff.begin() + _write_index);
            has_write(t.second);
        }
    }

    size_t Buffer::get_readable_size() const noexcept
    {
        assert(_write_index>=_read_index);
        return _write_index - _read_index;
    }

    const char *Buffer::get_read_ptr() const noexcept
    {
        assert(_write_index>=_read_index);
        return _buff.data() + _read_index;
    }

    char *Buffer::get_read_ptr()noexcept
    {
        assert(_write_index>=_read_index);
        return _buff.data() + _read_index;
    }

    char *Buffer::get_write_ptr()noexcept
    {
        assert(_buff.size()>=_write_index);
        return _buff.data() + _write_index;
    }

    void Buffer::has_read(size_t n)noexcept
    {
        _read_index += n;
        assert(_read_index <= _write_index);
    }

    void Buffer::has_write(size_t n)noexcept
    {
        _write_index += n;
        assert(_write_index <= _buff.size());
    }
}