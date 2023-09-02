#include <cstdint>
#include <iostream>
#include <unistd.h>
using namespace std;

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/composite.h>

#include <timer.h>

namespace scr {

    class Screen {
    private:

        xcb_connection_t *connection;
        xcb_screen_t *screen;

        xcb_get_image_reply_t *reply;
        uint8_t *idata;

        xcb_window_t window;

    public:
        Screen();

        ~Screen();

        void create_window();
        void map_window();
        uint8_t* screen_data(int width, int height);
        void screen_free_data();
    };

}
