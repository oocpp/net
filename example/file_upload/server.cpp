#include <iostream>
#include <NetFwd.h>
#include <EventLoopThread.h>
#include <fstream>
#include <Any.h>
#include <functional>
#include <map>

using namespace std;
using namespace net;

class Server{
public:
    Server(EventLoop*loop,const InetAddress&addr)
            :loop(loop)
            ,ser(loop,addr,"ChatServer",4){
        ser.set_connection_cb([this](const TCPConnPtr &conn){
            onConnection(conn);
        });

        ser.set_message_cb([this](const TCPConnPtr &conn,Buffer*message){
            onMessage(conn,message);
        });
    }

    void start(){
        ser.run();
    }
private:
    void onConnection(const TCPConnPtr &conn){
        LOG_INFO<< conn->get_peer_addr().to_ip()<<":"<< conn->get_peer_addr().to_port()<<" is "
                <<((conn->is_connected())?"up":"down");

        if(conn->is_connected()) {
            conn->set_tcp_no_delay(true);
            conns[conn]=0;
        }
        else {
            conns.erase(conn);
        }
    }

    void onMessage(const TCPConnPtr &conn,Buffer*message){

       if(conn->get_context().has_value()){
           auto & file= any_cast<shared_ptr<ofstream>& >(conn->get_context());
           file->write(message->read_ptr(),message->readable_size());
           message->clear();
       }
       else{
           char *end =nullptr;

           if(message->readable_size()>sizeof(int64_t)) {
               string st = "\r\n";
                end= search(message->read_ptr()+sizeof(int64_t), message->read_ptr() + message->readable_size(), st.begin(),
                                    st.end());
           }

           if(end!= nullptr) {
               auto len = message->read_int64();
               conns[conn] = len;

               string str("recv_");
               str.append(message->read_ptr(),end);
               shared_ptr<ofstream> file = make_shared<ofstream>(str, ifstream::binary);
               conn->set_context(Any(file));

               message->has_read(end-message->read_ptr()+2);
           }
       }
    }

    using ConnectionList= std::map<TCPConnPtr,int64_t>;
    EventLoop*loop;
    TcpServer ser;
    ConnectionList conns;
};

int main()
{
    EventLoop loop;

    Server c(&loop, InetAddress("127.0.0.1", 55555));
    c.start();

    loop.run();
}