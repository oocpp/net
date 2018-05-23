#include <iostream>
#include <NetFwd.h>
#include <EventLoopThread.h>
#include <fstream>
#include <Any.h>

using namespace std;
using namespace net;

void upload(EventLoop*loop,const string &str) {
    ifstream in(str, ifstream::binary);
    shared_ptr<TcpClient> client = make_shared<TcpClient>(loop, InetAddress("127.0.0.1", 55555));
    client->set_retry(false);

    client->set_connection_cb([str, client](const TCPConnPtr &conn) {
        shared_ptr<ifstream> in=make_shared<ifstream>(str, ifstream::binary);
        if (in->is_open()) {

            LOG_INFO << "start upload";
            in->seekg(0, ifstream::end);
            auto len = in->tellg();
            in->seekg(0, ifstream::beg);

            conn->set_context(Any{in});

            conn->set_write_complete_cb([client](const TCPConnPtr &conn) {
                char c[8 * 1024] = {0};

                shared_ptr<ifstream>file = any_cast<shared_ptr<ifstream> >(conn->get_context());

                auto count = file->readsome(c, sizeof(c));
                if(count>0)
                    conn->send(c, count);
                else if (count == 0) {
                    LOG_INFO << "upload finshed";
                    conn->set_connection_cb();
                    client->set_connection_cb();
                    client->disconnect();
                }
            });
            Buffer buf;
            buf.append_int64(len);
            buf.append(str);
            buf.append("\r\n",2);
            conn->send(&buf);
        }
        else {
            LOG_ERROR << str << " open failed";
        }

    });

    client->connect();
}

int main() {
    EventLoopThread loop;
    loop.run();

    std::string str;
    while (std::getline(std::cin, str)){
        thread(upload,loop.loop(),str).detach();
    }

    LOG_INFO<<"client quit";
    loop.stop_and_join();
}