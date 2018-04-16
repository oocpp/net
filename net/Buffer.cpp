#include <sys/uio.h>
#include"Buffer.h"

namespace net
{

    Buffer::Buffer()noexcept
            : _read_index(0)
            , _write_index(0)
            ,_reserve_index(0)
    {

    }

    void Buffer::swap(Buffer&b) noexcept
    {
        _buff.swap(b._buff);
        std::swap(_read_index,b._read_index);
        std::swap(_write_index,b._write_index);
        std::swap(_reserve_index,b._reserve_index);
    }

    Buffer::Buffer(Buffer&&b) noexcept
        : _buff(std::move(b._buff))
        ,_read_index(b._read_index)
        , _write_index(b._write_index)
        ,_reserve_index(b._reserve_index)
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
                _reserve_index-=_read_index;
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
        _reserve_index=0;
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

    void Buffer::reserve_head_space(size_t len)
    {
        ensure_writable_bytes(len);
        _reserve_index=_write_index;
        has_write(len);
    }

    void Buffer::fill_head_space(size_t index, const std::string &str)
    {
        std::copy(str.begin(), str.end(), _buff.begin() + _reserve_index);
    }

    void Buffer::fill_head_space(size_t index, const char *str,size_t len)
    {
        std::copy_n(str, len, _buff.begin() + _reserve_index);
    }

    void Buffer::append(void *data, size_t len) noexcept
    {
        append(static_cast<char*>(data),len);
    }

    void Buffer::append_int64(int64_t x)
    {
        int64_t be64 = htobe64(x);
        append(&be64, sizeof be64);
    }

    void Buffer::append_int32(int32_t x)
    {
        int32_t be32 = htobe32(x);
        append(&be32, sizeof be32);
    }

    void Buffer::append_int16(int16_t x)
    {
        int16_t be16 = htobe16(x);
        append(&be16, sizeof be16);
    }

    void Buffer::append_int8(int8_t x)
    {
        append(&x, sizeof x);
    }

    int8_t Buffer::peek_int8() const
    {
        assert(get_readable_size() >= sizeof(int8_t));
        int8_t x = *get_read_ptr();
        return x;
    }

    int16_t Buffer::peek_int16() const
    {
        assert(get_readable_size() >= sizeof(int16_t));
        int16_t be16 = 0;
        ::memcpy(&be16, get_read_ptr(), sizeof be16);
        return be16toh(be16);
    }

    int32_t Buffer::peek_int32() const
    {
        assert(get_readable_size() >= sizeof(int32_t));
        int32_t be32 = 0;
        ::memcpy(&be32, get_read_ptr(), sizeof be32);
        return be32toh(be32);
    }

    int64_t Buffer::peek_int64() const
    {
        assert(get_readable_size() >= sizeof(int64_t));
        int64_t be64 = 0;
        ::memcpy(&be64, get_read_ptr(), sizeof be64);
        return be64toh(be64);
    }

    int8_t Buffer::read_int8()
    {
        int8_t result = peek_int8();
        has_read(sizeof(int8_t));
        return result;
    }

    int16_t Buffer::read_int16()
    {
        int16_t result = peek_int16();
        has_read(sizeof(int16_t));
        return result;
    }

    int32_t Buffer::read_int32()
    {
        int32_t result = peek_int32();
        has_read(sizeof(int32_t));
        return result;
    }

    int64_t Buffer::read_int64()
    {
        int64_t result = peek_int64();
        has_read(sizeof(int64_t));
        return result;
    }
}