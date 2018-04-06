#include <iostream>
#include"EventLoop.h"
#include"TcpServer.h"
#include"Buffer.h"
#include"TcpConnection.h"
#include"Log.h"

using namespace std;
using namespace net;

void http_echo(const TCPConnPtr &conn, Buffer *buff){
        string str="HTTP/1.1 200 OK\r\n"
                "Server: net server\r\n"
                "Content-Length: 113\r\n"
                "Content-Type: text/html\r\n\r\n<html>net server echo</html>";

        conn->set_write_complete_cb([](const TCPConnPtr &conn) {
            LOG_INFO<<"close http";
            conn->close();
        });

        if(buff->get_read_ptr()[0]=='G'
           &&buff->get_read_ptr()[1]=='E'
           &&buff->get_read_ptr()[2]=='T') {
                conn->send(str);
        }
        else{
                string str1 = "HTTP/1.1 404 not found\r\n"
                        "Server: net server\r\n"
                        "Content-Length: 91\r\n\r\n";
                conn->send(str1);
        }
        LOG_INFO<<"send";
}

void new_conn(const TCPConnPtr &conn){
        if(conn->is_connected()){
                LOG_INFO<<"new connection";
        }
}



int main() {
        Log::set_rank(5);
        EventLoop loop;

        InetAddress addr("127.0.0.1",8888);
        TcpServer ser(&loop,addr,"http server",3);

        ser.set_message_cb(http_echo);
        ser.set_connection_cb(new_conn);

        ser.run();
        loop.run();
}