//
// Created by lg on 17-4-18.
//

#pragma once

#include<sstream>
#include<iostream>

namespace net
{

    class Log
    {
    public:
        Log(int n, const char *file, int line, const char *func);

        template<typename T>
        Log &operator<<(const T &t)
        {
            if (rank > RANK) {
                this->out << t;
            }
            return *this;
        }

        Log &operator<<(std::ostream &(*op)(std::ostream &));

        Log &operator()(const char *s, ...);

        ~Log();

        static void set_rank(int i)
        {
            RANK = i;
        }

    private:
        std::stringstream out;
        int rank;
        static int RANK;
    };
}

#define LOG_TRACE net::Log(1,__FILE__,__LINE__,__FUNCTION__)<<" [trace] "
#define LOG_DEBUG net::Log(2,__FILE__,__LINE__,__FUNCTION__)<<" [debug] "
#define LOG_INFO  net::Log(3,__FILE__,__LINE__,__FUNCTION__)<<" [info ] "
#define LOG_WARN  net::Log(4,__FILE__,__LINE__,__FUNCTION__)<<" [warn ] "
#define LOG_ERROR net::Log(5,__FILE__,__LINE__,__FUNCTION__)<<" [error] "
#define LOG_FATAL net::Log(6,__FILE__,__LINE__,__FUNCTION__)<<" [fatal] "