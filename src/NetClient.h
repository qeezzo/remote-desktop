#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <memory>

#include "NetIO.h"

// class similar to NetClient, except waits connection
// rather than send request.
class NetClient : private NetIO {
private:

    sockaddr_storage    mServerAddr{}; 
    socklen_t           mServerAddrLen{}; 
    std::string         mServerName;
    int                 mPort;
    int                 mSocket;
    int                 mServerSocket;

public:

    NetClient(const std::string_view serv_name, int port);
    ~NetClient();

    // sends entire packet of data.
    // returns number of bytes sent.
    int send(void* data, size_t size) {
        return sendData(mSocket, data, size);
    }

    // returns number of recieved bytes.
    int recv(void* buf, size_t buf_size) {
        return recvData(mSocket, buf, buf_size);
    }

    // returns static fucntion's member
    const std::string_view getClientInfo();

};

#endif
