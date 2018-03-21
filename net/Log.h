//
// Created by lg on 17-4-18.
//

#pragma once
#include<sstream>
#include<cstdio>
#include<cstdarg>
#include<iostream>
#include<thread>
#include"cstring"
#include<iomanip>

namespace net{
    class Log {
    public:
        Log(const char* file, int line, const char*func) {

            const char*p=strrchr(file,'/');
            if(p==nullptr)
                p=file;
            else
                ++p;

            this->out <<std::this_thread::get_id()<<" "
                      <<std::setw(15)<< p << '('<< line << ") "
                      <<std::setw(15)<< std::setiosflags(std::ios::left) <<func
                      <<"     ";
        }

        template<typename T>
        Log&operator<<(const T & t) {
            this->out << t;
            return *this;
        }

        Log&operator<<(std::ostream&(*op)(std::ostream&)) {
            op(this->out);
            return *this;
        }

        Log& operator()(const char*s, ...) {
            char buf[1024];/*注意 最大输出长度1024..自行设置*/
            va_list ap;
            va_start(ap, s);
            int n = vsnprintf(buf, 1024, s, ap);
            va_end(ap);
            buf[n] = '\0';
            out << buf;
            return *this;
        }

        ~Log() { std::cout << out.rdbuf() << std::endl; }

    private:
        std::stringstream out;
    };
}

#define LOG_TRACE net::Log(__FILE__,__LINE__,__FUNCTION__)
#define LOG_DEBUG net::Log(__FILE__,__LINE__,__FUNCTION__)
#define LOG_INFO  net::Log(__FILE__,__LINE__,__FUNCTION__)
#define LOG_WARN  net::Log(__FILE__,__LINE__,__FUNCTION__)
#define LOG_ERROR net::Log(__FILE__,__LINE__,__FUNCTION__)
#define LOG_FATAL net::Log(__FILE__,__LINE__,__FUNCTION__)