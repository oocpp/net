#pragma once

#include<sstream>
#include<iostream>
#include<chrono>
#include<thread>

namespace net
{
    namespace impl
    {
        constexpr bool is_separator(const char c)
        {

            return (c == '/' || c == '\\');
        }

        constexpr int get_base_index(const char *filename, const int n)
        {
            return (n == -1 || is_separator(filename[n])) ? n + 1 : get_base_index(filename, n - 1);
        }
    }

    class Log
    {
    public:

    public:
        Log() = default;

        template<int N>
        Log &init(const char *file, int line, const char *func)
        {
            this->func = func;
            this->file = file + N;
            this->line = line;

            print_time();
            return *this;
        }

        ~Log() ;

        Log &print_time();

        Log &print_file_info();

        template<typename T>
        Log &operator<<(const T &t)
        {
            out << t;
            return *this;
        }

        Log &operator<<(std::ostream &(*op)(std::ostream &));

        Log &operator()(const char *s, ...);

        static void set_rank(int i)
        {
            RANK = i;
        }

        static int get_rank()
        {
            return RANK;
        }

    private:
        const char *func;
        const char *file;
        int line;
        std::stringstream out;
        static int RANK;
    };
}

#define LOG_TRACE if(net::Log::get_rank()<1)\
    net::Log{}.init<net::impl::get_base_index(__FILE__,sizeof(__FILE__)-1)>(__FILE__,__LINE__,__func__)<<"[trace] "
#define LOG_DEBUG if(net::Log::get_rank()<2)\
    net::Log{}.init<net::impl::get_base_index(__FILE__,sizeof(__FILE__)-1)>(__FILE__,__LINE__,__func__)<<"[debug] "
#define LOG_INFO  if(net::Log::get_rank()<3)\
    net::Log{}.init<net::impl::get_base_index(__FILE__,sizeof(__FILE__)-1)>(__FILE__,__LINE__,__func__)<<"[info ] "
#define LOG_WARN  if(net::Log::get_rank()<4)\
    net::Log{}.init<net::impl::get_base_index(__FILE__,sizeof(__FILE__)-1)>(__FILE__,__LINE__,__func__)<<"[warn ] "
#define LOG_ERROR if(net::Log::get_rank()<5)\
    net::Log{}.init<net::impl::get_base_index(__FILE__,sizeof(__FILE__)-1)>(__FILE__,__LINE__,__func__)<<"[error] "
#define LOG_FATAL if(net::Log::get_rank()<6)\
    net::Log{}.init<net::impl::get_base_index(__FILE__,sizeof(__FILE__)-1)>(__FILE__,__LINE__,__func__)<<"[fatal] "

