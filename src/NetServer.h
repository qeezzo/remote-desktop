#ifndef NET_SERVER_H
#define NET_SERVER_H

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <memory>

#include "NetIO.h"

// class similar to NetClient, except waits connection
// rather than send request.
class NetServer : private NetIO {
private:

    enum {BACKLOG = 1, SEND_SIZE = 32'768};

    sockaddr_storage    mClientAddr{}; 
    socklen_t           mClientAddrLen{}; 
    std::string         mClientName;
    int                 mPort;
    int                 mSocket;
    int                 mClientSocket;

public:

    NetServer(int port);
    ~NetServer();

    // sends entire packet of data.
    // returns 0 if successed
    int send(void* data, size_t size);

    // returns number of recieved bytes.
    int recv(void* buf, size_t buf_size) {
        return recvData(mClientSocket, buf, buf_size);
    }

    // returns static fucntion member
    const std::string_view getClientInfo();

};

#endif
