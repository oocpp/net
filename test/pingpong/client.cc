#include <TcpClient.h>
#include <Log.h>
#include <EventLoop.h>
#include <EventLoopThreadPool.h>
#include <InetAddress.h>
#include<Buffer.h>
#include<TcpConnection.h>

#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace net;
using namespace std::placeholders;

class Client;

class Session
{
 public:
  Session(EventLoop* loop,
          const InetAddress& serverAddr,
          const string& name,
          Client* owner)
    : client_(loop, serverAddr, name),
      owner_(owner),
      bytesRead_(0),
      bytesWritten_(0),
      messagesRead_(0)
  {
    client_.set_connection_cb(
        std::bind(&Session::onConnection, this, _1));
    client_.set_message_cb(
        std::bind(&Session::onMessage, this, _1, _2));
  }

  void start()
  {
    client_.connect();
  }

  void stop()
  {
    client_.disconnect();
  }

  int64_t bytesRead() const
  {
     return bytesRead_;
  }

  int64_t messagesRead() const
  {
     return messagesRead_;
  }

 private:

  void onConnection(const TCPConnPtr & conn);

  void onMessage(const TCPConnPtr& conn, Buffer* buf)
  {
    ++messagesRead_;
    bytesRead_ += buf->get_readable_size();
    bytesWritten_ += buf->get_readable_size();
    conn->send(buf);
  }

  TcpClient client_;
  Client* owner_;
  int64_t bytesRead_;
  int64_t bytesWritten_;
  int64_t messagesRead_;
};

class Client
{
 public:
  Client(EventLoop* loop,
         const InetAddress& serverAddr,
         int blockSize,
         int sessionCount,
         chrono::seconds timeout,
         int threadCount)
    : loop_(loop),
      threadPool_(loop,threadCount),
      sessionCount_(sessionCount),
      timeout_(timeout)
  {

    threadPool_.run();

    for (int i = 0; i < blockSize; ++i)
    {
      message_.push_back(static_cast<char>(i % 128));
    }
      loop->run_after(timeout, std::bind(&Client::handleTimeout, this));

    for (int i = 0; i < sessionCount; ++i)
    {
      char buf[32];
      snprintf(buf, sizeof buf, "C%05d", i);
      Session* session = new Session(threadPool_.get_next_loop(), serverAddr, buf, this);
      session->start();
      sessions_.push_back(session);
    }
  }

  const string& message() const
  {
    return message_;
  }

  void onConnect()
  {
    if (++numConnected_ == sessionCount_)
    {
      LOG_WARN << "all connected";
    }
  }

  void onDisconnect(const TCPConnPtr& conn)
  {
    if (--numConnected_ == 0)
    {
      LOG_WARN << "all disconnected";

      int64_t totalBytesRead = 0;
      int64_t totalMessagesRead = 0;
      for (std::vector<Session*>::iterator it = sessions_.begin();
          it != sessions_.end(); ++it)
      {
        totalBytesRead += (*it)->bytesRead();
        totalMessagesRead += (*it)->messagesRead();
      }
      LOG_WARN << totalBytesRead << " total bytes read";
      LOG_WARN << totalMessagesRead << " total messages read";
      LOG_WARN << static_cast<double>(totalBytesRead) / static_cast<double>(totalMessagesRead)
               << " average message size";
      LOG_WARN << static_cast<double>(totalBytesRead) / (timeout_.count() * 1024 * 1024)
               << " MiB/s throughput";
      conn->get_loop()->queue_in_loop(std::bind(&Client::quit, this));
    }
  }

 private:

  void quit()
  {
    loop_->queue_in_loop(std::bind(&EventLoop::stop, loop_));
  }

  void handleTimeout()
  {
    LOG_WARN << "stop";
    std::for_each(sessions_.begin(), sessions_.end(),
                  std::mem_fn(&Session::stop));
  }

  EventLoop* loop_;
  EventLoopThreadPool threadPool_;
  int sessionCount_;
  chrono::seconds timeout_;
  std::vector<Session*> sessions_;
  string message_;
  atomic<int64_t >numConnected_{0};
};

void Session::onConnection(const TCPConnPtr& conn)
{
  if (conn->is_connected())
  {
      conn->set_tcp_no_delay(true);
    conn->send(owner_->message());
    owner_->onConnect();
  }
  else
  {
    owner_->onDisconnect(conn);
  }
}

int main(int argc, char* argv[])
{
  if (argc != 7)
  {
    fprintf(stderr, "Usage: client <host_ip> <port> <threads> <blocksize> ");
    fprintf(stderr, "<sessions> <time>\n");
  }
  else
  {
    Log::set_rank(2);

    const char* ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    int threadCount = atoi(argv[3]);
    int blockSize = atoi(argv[4]);
    int sessionCount = atoi(argv[5]);
    chrono::seconds timeout {atoi(argv[6])};

    EventLoop loop;
    InetAddress serverAddr(ip, port);

    Client client(&loop, serverAddr, blockSize, sessionCount, timeout, threadCount);
    loop.run();
  }
}

