#include <iostream>
#include <EventLoopThread.h>
#include"EventLoop.h"
#include"TcpClient.h"
#include"Buffer.h"
#include"TcpConnection.h"
#include"Log.h"

using namespace std;
using namespace net;
using namespace std::placeholders;


class ChatClient{
public:
    ChatClient(EventLoop*loop,const InetAddress&addr)
            :loop(loop)
            ,client(loop,addr,"ChatClient"){
        client.set_connection_cb(std::bind(&ChatClient::onConnection,this,_1));
        client.set_message_cb(std::bind(&ChatClient::onMessage,this,_1,_2));
    }

    void connect(){
        client.connect();
    }

    void disconnect() {
            client.disconnect();
    }

    void write(const string&str){
        if(std::atomic_load(&conns)!= nullptr)
            conns->send(str);
    }

private:
private:
    void onConnection(const TCPConnPtr &conn) {
        LOG_INFO << conn->get_local_addr().toIp() << ":" << conn->get_local_addr().toPort() << " is "
                 << ((conn->is_connected()) ? "up" : "down");

        if (conn->is_connected()) {
            conn->set_tcp_no_delay(true);
            std::atomic_store(&conns, conn);
        }
        else
            conns.reset();
    }

    void onMessage(const TCPConnPtr &conn,Buffer*message){
        message->append("\0",1);
        cout<<message->get_read_ptr()<<endl;

        message->clear();
    }

    EventLoop*loop;
    TcpClient client;
    TCPConnPtr conns;
};

int main() {

    Log::set_rank(2);

    EventLoopThread loop;
    loop.run();

    //InetAddress addr("112.74.86.0", 55555);
    InetAddress addr("127.0.0.1", 55555);
    ChatClient c(loop.get_loop(), addr);
    c.connect();

    std::string str;
    while (std::getline(std::cin, str))
       c.write(str);

    c.disconnect();
    LOG_INFO<<"client quit";
    loop.stop_and_join();
}