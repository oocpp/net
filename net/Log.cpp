#include<cstdio>
#include<cstdarg>
#include<thread>
#include<cstring>
#include<iomanip>
#include"Log.h"

namespace net
{
    int Log::RANK = 0;

    Log::Log(int n, const char *file, int line, const char *func)
            : rank(n)
    {
        if (rank > RANK) {
            const char *p = strrchr(file, '/');
            if (p == nullptr)
                p = file;
            else
                ++p;

            this->out << std::this_thread::get_id() << " "
                      << std::setw(15) << p << '(' << line << ") "
                      << std::setw(15) << std::setiosflags(std::ios::left) << func
                      << "     ";
        }
    }

    Log &Log::operator()(const char *s, ...)
    {
        if (rank > RANK) {
            char buf[1024];/*注意 最大输出长度1024..自行设置*/
            va_list ap;
            va_start(ap, s);
            int n = vsnprintf(buf, 1024, s, ap);
            va_end(ap);
            buf[n] = '\0';
            out << buf;
        }
        return *this;
    }

    Log &Log::operator<<(std::ostream &(*op)(std::ostream &))
    {
        if (rank > RANK) {
            op(this->out);
        }
            return *this;
    }

    Log::~Log()
    {
            std::cout << out.rdbuf() << std::endl;
    }

}
