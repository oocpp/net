#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <utility>
#include <string>
#include <cstring>

namespace net
{
    class Buffer final
    {
    private:
        void ensure_writable_bytes(size_t len);

        void append(void *data, size_t len)noexcept;
    public:
        Buffer()noexcept;

        Buffer(Buffer&&b)noexcept;

        void swap(Buffer&b)noexcept ;

        std::pair<ssize_t, int> read_from_fd(int fd);

        size_t get_writable_size() const noexcept;

        size_t length() const noexcept;

        void clear() noexcept;

        void append(const char *data, size_t len);

        void append(const std::string &str);

        void append(const Buffer &buff);

        void append(std::initializer_list<std::pair<const char *, std::size_t>> args);

        size_t get_readable_size() const noexcept;

        const char *get_read_ptr() const noexcept;

        char *get_read_ptr() noexcept;

        char *get_write_ptr()noexcept;

        void has_read(size_t n)noexcept;

        void has_write(size_t n)noexcept;

    public:

        void reserve_head_space(size_t len);

        void fill_head_space(size_t index,const std::string &str);

        void fill_head_space(size_t index,const char *str,size_t len);

        void append_int64(int64_t x);

        void append_int32(int32_t x);

        void append_int16(int16_t x);

        void append_int8(int8_t x);

        int64_t read_int64();

        int32_t read_int32();

        int16_t read_int16();

        int8_t read_int8();

        int64_t peek_int64() const;

        int32_t peek_int32() const;

        int16_t peek_int16() const;

        int8_t peek_int8() const;

        void reserve(size_t len);

    private:
        static constexpr size_t INIT_SIZE=1024;
        std::vector<char> _buff;
        size_t _read_index;
        size_t _write_index;
        size_t _reserve_index;
    };
}