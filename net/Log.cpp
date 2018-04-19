#include<cstdio>
#include<cstdarg>
#include<cstring>
#include<iomanip>
#include <cassert>
#include"Log.h"

namespace net
{
    int Log::RANK = 0;

    Log &Log::operator()(const char *s, ...)
    {
        char buf[1024];/*注意 最大输出长度1024..自行设置*/
        va_list ap;
        va_start(ap, s);
        int n = vsnprintf(buf, 1024, s, ap);
        va_end(ap);
        buf[n] = '\0';
        out << buf;
        return *this;
    }

    Log &Log::operator<<(std::ostream &(*op)(std::ostream &))
    {
        op(this->out);
        return *this;
    }

    Log& Log::print_file_info()
    {
        out <<"   -> "<< file << ':' << line<<' '<< func;
        return *this;
    }

    Log &Log::print_time()
    {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        time_t sec=std::chrono::duration_cast<std::chrono::seconds>(now).count();
        auto micr = std::chrono::duration_cast<std::chrono::microseconds>(now%std::chrono::seconds(1)).count();

        static thread_local time_t last_time{};

        thread_local char t_time[18];
        if(sec!=last_time){
            last_time=sec;

            tm tm_time{};
            gmtime_r(&sec,&tm_time);
            int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                               tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                               tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
            assert(len == 17);

        }

        static thread_local std::thread::id id = std::this_thread::get_id();
        out<<t_time<<'.'<< std::setfill('0') << std::setw(6)<<micr<<' '<<id<<' ';
        return *this;
    }

    Log::~Log(){
        print_file_info();
        std::cout<<out.rdbuf()<<std::endl;
    }

}
