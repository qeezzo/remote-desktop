#include "NetServer.h"
#include <memory>
#include <netdb.h>

NetServer::NetServer(int port)
    : mPort(port)
{
    using std::cerr, std::endl;

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo* pRes;
    if (getaddrinfo(nullptr, std::to_string(mPort).c_str(), &hints, &pRes))
        throw "Error: getaddrinfo";

    // handle future exceptions for "addrinfo* pRes" with freeaddrinfo
    std::unique_ptr<addrinfo, void(*)(addrinfo*)> uptrRes(pRes, freeaddrinfo);

    cerr << "Getting socket..." << endl;
    mSocket = socket(   uptrRes->ai_family, uptrRes->ai_socktype, 
                        uptrRes->ai_protocol);
    if (mSocket < 0)
        throw "Error: failed to get socket";

    int yes = 1;
    if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        throw "Error: setsockopt";
    }

    cerr << "Binding socket..." << endl;
    if (bind(   mSocket, reinterpret_cast<sockaddr*>(uptrRes->ai_addr), 
                uptrRes->ai_addrlen) < 0 )
        throw "Error: failed to bind socket";

    cerr << "Waiting for connection..." << endl;
    if (listen(mSocket, BACKLOG) < 0)
        throw "Error: failed to listen socket";

    cerr << "Acepting connection..." << endl;
    mClientSocket = accept( mSocket, reinterpret_cast<sockaddr*>(&mClientAddr),
                            &mClientAddrLen);
    if (mClientSocket < 0)
        throw "Error failed to accept connection";
    cerr << "Accepted!" << endl;
}

NetServer::~NetServer()
{
    close(mSocket);
}

int NetServer::send(void* data, size_t size)
{
    uint8_t* pdata = static_cast<uint8_t*>(data);
    while (size > SEND_SIZE) {
        if (int rc = sendData(mClientSocket, pdata, SEND_SIZE))
            return rc;
        size -= SEND_SIZE;
        pdata += SEND_SIZE;
    }
    return sendData(mClientSocket, pdata, size);
}
