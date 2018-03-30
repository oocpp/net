#pragma once
#include<vector>
#include <algorithm>
#include <cassert>
#include <utility>
#include<string>

namespace net{
    class Buffer final
    {
    private:
        void ensure_writable_bytes(size_t len);

    public:
        Buffer();

        std::pair<ssize_t,int> read_from_fd(int fd);

        size_t get_writable_size() const;

        size_t length() const;

        void clear();

        void append(const char*data,size_t len);

        void append(const std::string& str);

        void append(const Buffer& buff);

        void append(std::initializer_list<std::pair<const char*,std::size_t>>args);

        size_t get_readable_size() const;

        const char* get_read_ptr()const;

        char* get_read_ptr();

        char* get_write_ptr();

        void has_read(size_t n);

        void has_write(size_t n);

    private:
        std::vector<char>_buff;
        size_t _read_index;
        size_t _write_index;
    };

}
