#ifndef NET_IO_H
#define NET_IO_H

#include <cstddef>

class NetIO {
public:

    enum { MaxDataSize = 1024 * 64 - 1 };

    NetIO(){};
    virtual ~NetIO(){};

    // sends entire packet of data.
    // returns 0 if successed, -1 otherwise.
    int sendData(int socket, void* data, size_t size);

    // returns number of recieved bytes.
    int recvData(int socket, void* buf, size_t buf_size);

};

#endif
