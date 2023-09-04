#include <X11/X.h>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <timer.h>

#include "NetClient.h"
#include "H264Decoder.h"
//#include "H264Encoder.h"
#include "ScreenData.h"
#include "Frame.h"

using std::cout, std::endl, std::cerr;

int ximage(FrameRGBA* frame);

Packet* recievePack(NetClient* client)
{
    
    int size;
    if (!client->recv(&size, sizeof(int)))
        return nullptr;
    //cout << "DATA SIZE : " << size << endl;

    static Packet pack(size - sizeof(int));
    pack.resize(size - sizeof(int));

    int total{};
    while (total < pack.getSizeWithoutHeader()) {
        int bytes = client->recv(pack.getPtrToData() + total,
                                pack.getSizeWithoutHeader() - total);
        if (bytes <= 0)
            return nullptr;
        total += bytes;
    }
    return &pack;
}

int main()
{

    const int width { 1920 };
    const int height { 1080 };

    try {

        Display *display;
        Window window;
        GC gc;
        int width = 1920;
        int height = 1080;

        // Connect to the X server
        display = XOpenDisplay(NULL);
        if (!display) {
            fprintf(stderr, "Unable to open the X display.\n");
            return 1;
        }

        // Create the window
        int screen = DefaultScreen(display);
        window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, width, height, 0, BlackPixel(display, screen), WhitePixel(display, screen));

        // Select the events to listen for
        XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);

        // Map the window
        XMapWindow(display, window);

        // Create a graphics context
        gc = XCreateGC(display, window, 0, NULL);

        NetClient client("renatux", 4000);
        H264Decoder decoder(width, height);
        ScreenData scrData;

        // Main event loop
        int counter{};
        Timer t;

        struct Cursor {
            Coord pos{};
            bool pressed{};
        };

        Cursor prev_cursor = {scrData.get_pointer_pos(), };

        bool pressed{};
        while (1) {
            XEvent event{};
            XCheckWindowEvent(display, window, -1, &event);
            if (event.type == KeyPress) {
                break; // Exit the loop when any key is pressed
            }

            Packet* pack {recievePack(&client)};
            //cout << "pointer: (" << cursor.x << ", " << cursor.y << endl;

            // sending cursor position...
            if (event.type == ButtonPress) {
                cout << "Pressed" << endl;
                pressed = true;
            }
            else if (event.type == ButtonRelease) {
                cout << "Released" << endl;
                pressed = false;
            }
            Cursor cursor = {scrData.get_pointer_pos(), pressed};

            if (!cursor.pressed && cursor.pos.x == prev_cursor.pos.x && cursor.pos.y == prev_cursor.pos.y) {
                Cursor same{{-1, -1}, false};
                client.send(&same, sizeof(Cursor));
            } else
                client.send(&cursor, sizeof(Cursor));
            prev_cursor = cursor;
            

            FrameRGBA* f{};

            if (pack) {
                //cout << "Decoding packet..." << endl;
                f = decoder.decodePacket(pack);
            } else break;

            if (f) {
                XImage* image = XCreateImage(
                        display,
                        DefaultVisual(display, screen),
                        DefaultDepth(display, screen),
                        ZPixmap, 0, (char*)f->data(), width, height, 32, 0
                );

                XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);
                ++counter;
            }
            if (t.elapsed() > 1.0) {
                cout << "Frames : " << counter << endl;
                counter = 0;
                t.reset();
            }
        }

        // Clean up and close the X server connection
        XFreeGC(display, gc);
        XDestroyWindow(display, window);
        XCloseDisplay(display);

    } catch (const char* msg) {
        cerr << msg << endl;
    }
}
