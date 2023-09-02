#include <iostream>
#include <memory>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <tuple>
#include <error.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
using namespace std;

static constexpr size_t BufSize = 4100;

int main(int argc, char* argv[])
{
    //Display* display = XOpenDisplay(NULL);
    //Window root = DefaultRootWindow(display);

    //// Create a new window
    //Window win = XCreateSimpleWindow(display, root, 0, 0, 640, 480, 0, 0, 0);

    //// Set window properties
    //XStoreName(display, win, "My Window");
    //XSelectInput(display, win, ExposureMask | KeyPressMask);
    //XMapWindow(display, win);

    //GC gc = XCreateGC(display, win, 0, NULL);

    ////XWindowAttributes attr;
    ////while (true) {
    ////    XImage* image = XGetImage(display, root, 1920, 0, 1920, 1080, AllPlanes, ZPixmap);
    ////    XGetWindowAttributes(display, win, &attr);
    ////    cout << attr.width << " : " << attr.height << endl;
    ////    XPutImage(display, win, gc, image, 0, 0, 0, 0, attr.width, attr.height);
    ////    XDestroyImage(image);
    ////}

    //// Create a graphics context and draw the image onto the window

    //// Wait for user input
    //XEvent event;
    //XSelectInput(display, win, ExposureMask | KeyPressMask);
    //while (1) {
    //    XNextEvent(display, &event);
    //    if (event.type == KeyPress)
    //        break;
    //}


    // -----------------------------------------

    addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    cout << "Setting addrinfo..." << endl;
    addrinfo* res;
    if ( int status = getaddrinfo("renatux", "4000", &hints, &res)){
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

    cout << "Connecting socket..." << endl;
    if ( connect(sockfd, res->ai_addr, res->ai_addrlen) < 0 ) {
        cout << strerror(errno) << endl;
        return 4;
    }

    freeaddrinfo(res);

    cout << "Sending myself..." << endl;
    unique_ptr<char[]> buf { new char[BufSize] };
    send(sockfd, buf.get(), 1, 0);
    cout << "connected: " << buf.get() << endl;


    while ( true ) {
        recv(sockfd, buf.get(), 4100, 0);
        send(sockfd, buf.get(), 1, 0);
        cout << "Packet: " << ((int*)buf.get())[0] << endl;
    }



    // Clean up and exit
    //XFreeGC(display, gc);
    //XDestroyWindow(display, win);
    //XCloseDisplay(display);
}
