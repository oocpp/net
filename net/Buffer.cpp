
#include <sys/uio.h>
#include"Buffer.h"

namespace net{

    std::pair<ssize_t, int> Buffer::read_from_fd(int fd) {
       char extrabuf[65536];
        struct iovec vec[2];

        const size_t w_size = get_writable_size();

        vec[0].iov_base = get_write_ptr();
        vec[0].iov_len = w_size;
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof extrabuf;

        const int count = (w_size < sizeof extrabuf) ? 2 : 1;

        std::pair<ssize_t ,int> result;
        result.first = ::readv(fd, vec, count);

        if (result.first < 0) {
            result.second = errno;
        }
        else if (static_cast<size_t>(result.first) <= w_size) {
            has_write(result.first);
        } else {
            has_write(w_size);
            append(extrabuf, result.first - w_size);
        }

        return result;
    }

}