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

        size_t writable_size() const noexcept;

        size_t length() const noexcept;

        size_t readable_size() const noexcept;

        const char *read_ptr() const noexcept;

        char *read_ptr() noexcept;

        char *write_ptr()noexcept;

        void has_read(size_t n)noexcept;

        void has_write(size_t n)noexcept;

        void clear() noexcept;

        void append(const char *data, size_t len);

        void append(const std::string &str);

        void append(const Buffer &buff);

        /// 添加'\0'，但不改变数据长度
        void to_text();

        void append(std::initializer_list<std::pair<const char *, std::size_t>> args);

    public:
        /// 在当前的写位置，预留一块空间
        /// 后面可以对这块空间进行填充
        void reserve_head_space(size_t len);

        /// 填充预留的空间
        void fill_head_space(size_t index,const std::string &str);

        /// 填充预留的空间
        void fill_head_space(size_t index,const char *str,size_t len);

        /// 末尾写入一个数值
        /// 会自动传换成网络字节序

        void append_int64(int64_t x);

        void append_int32(int32_t x);

        void append_int16(int16_t x);

        void append_int8(int8_t x);

        /// 读取一个数值
        /// 会自动传换成本机字节序
        int64_t read_int64();

        int32_t read_int32();

        int16_t read_int16();

        int8_t read_int8();

        /// 读取一个数值，不消耗内容，可重复读取
        /// 会自动传换成本机字节序
        int64_t peek_int64() const;

        int32_t peek_int32() const;

        int16_t peek_int16() const;

        int8_t peek_int8() const;

        /// 预留空间
        void reserve(size_t len);

    private:
        static constexpr size_t INIT_SIZE=1024;
        std::vector<char> _buff;
        size_t _read_index;
        size_t _write_index;
        size_t _reserve_index;
    };
}