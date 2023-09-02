#include "NetClient.h"

NetClient::NetClient(const std::string_view serv_name, int port)
    : mServerName(serv_name), mPort(port)
{
    using std::cerr, std::endl;

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* pRes;
    if (getaddrinfo(mServerName.c_str(), std::to_string(mPort).c_str(), &hints, &pRes))
        throw "Error: getaddrinfo";

    // handle future exceptions for "addrinfo* pRes" with freeaddrinfo
    std::unique_ptr<addrinfo, void(*)(addrinfo*)> uptrRes(pRes, freeaddrinfo);

    cerr << "Getting socket..." << endl;
    mSocket = socket(uptrRes->ai_family, uptrRes->ai_socktype, uptrRes->ai_protocol);
    if (mSocket < 0)
        throw "Error: failed to get socket";

    int yes = 1;
    if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        throw "Error: setsockopt";
    }

    cerr << "Connecting to the server..." << endl;
    if(connect( mSocket, reinterpret_cast<sockaddr*>(uptrRes->ai_addr), 
                uptrRes->ai_addrlen) < 0)
        throw "Error: failed to connect to the server";
    cerr << "Connected!" << endl;
}

NetClient::~NetClient()
{
    close(mSocket);
}


