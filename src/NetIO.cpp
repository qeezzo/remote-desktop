#include "NetIO.h"
#include <sys/socket.h>
#include <sys/types.h>

int NetIO::sendData(int socket, void *data, size_t size)
{
    if (size > MaxDataSize)
        return -2;

    size_t bytes_sent{};
    int send_return{};
    while (bytes_sent < size) {
        send_return = send( socket, static_cast<char*>(data) + bytes_sent, 
                            size - bytes_sent, 0);
        if (send_return < 0)
            return -1;

        bytes_sent += send_return;
    }

    return 0;
}

int NetIO::recvData(int socket, void *buf, size_t size)
{
    return recv(socket, buf, size, 0);
}
