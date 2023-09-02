#include <iostream>
#include <memory>
#include <cstring>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <tuple>
#include <error.h>
#include <fcntl.h>
#include <unistd.h>
#include <timer.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
using namespace std;

static constexpr size_t BufSize = 4100;

int main(int argc, char* argv[])
{
    Display* display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);

    addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    cout << "Setting addrinfo..." << endl;
    addrinfo* res;
    if ( int status = getaddrinfo(nullptr, "4000", &hints, &res)){
        cout << gai_strerror(status) << endl;
        return 1;
    }

    cout << "Getting socket..." << endl;
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if ( sockfd < 0 ) {
        cout << strerror(errno) << endl;
        return 2;
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return 3;
    }

    cout << "Binding socket..." << endl;
    if ( bind(sockfd, res->ai_addr, res->ai_addrlen) < 0 ) {
        cout << strerror(errno) << endl;
        return 4;
    }

    freeaddrinfo(res);

    cout << "Waiting for connection..." << endl;
    unique_ptr<char[]> buf { new char[BufSize] };
    sockaddr_storage reciever;
    socklen_t reciever_len = sizeof(sockaddr_storage);
    recvfrom(sockfd, buf.get(), BufSize, 0, (sockaddr*) &reciever, &reciever_len);

    cout << "Unblocking..." << endl;
    if ( fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0 ) {
        cout << strerror(errno) << endl;
        return 5;
    }



    fd_set master;
    fd_set readfds;
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(sockfd, &master);

    //double sumtime = 0;
    //int ntime{};

    int size = 1920 * 1080 * 4;
    cout << "size of image: " << size << endl;

    for (int k{}; k < 10; ++k) {
        XImage* image = XGetImage(display, root, 1920, 0, 1920, 1080, AllPlanes, ZPixmap);

        for (int i{}; i < size / 4096; ++i) {
            ((int*)buf.get())[0] = i;
            for (int j{4}; j < size / 4096; ++j)
                buf[j] = image->data[i * 4096 + j - 4];

            sendto(sockfd, buf.get(), BufSize, 0, (sockaddr*)&reciever, reciever_len);
            int ntry{20};
            //Timer t;

            while ( ntry-- > 0 ) {
                readfds = master;
                timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 100;
                if (select(sockfd+1, &readfds, nullptr, nullptr, &tv) < 0) {
                    cout << strerror(errno) << endl;
                    return 6;
                }

                if (FD_ISSET(sockfd, &readfds)) {
                    ssize_t bytes = recvfrom(sockfd, buf.get(), 1, 0, (sockaddr*) &reciever, &reciever_len);
                    switch (bytes) {
                        case 0: k = 10, i = 2024, ntry = 0;
                                break;
                        case 1: ntry = -1;
                                break;
                        default: break;
                    }
                }
            }
            
            if (ntry == 0) cout << "Missed: " << i << endl;

            //sumtime += t.elapsed();
            //++ntime;
            //tv.tv_usec = sumtime / ntime * 1e6;
            //cout << "tv_usec: " << tv.tv_usec << endl;
        }

        cout << "image: " << k << endl;
        XDestroyImage(image);
    }

    // Clean up and exit
    XCloseDisplay(display);
    cout << "end" << endl;
}
