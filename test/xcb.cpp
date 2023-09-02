#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <jpeglib.h>
using namespace std;

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/composite.h>


#include <sys/mman.h>
#include <fcntl.h>

#include <Imlib2.h>

#include <timer.h>

class Xapi {
private:

    xcb_connection_t *connection;
    xcb_screen_t *screen; xcb_window_t window; xcb_get_image_reply_t *reply;

public:
    Xapi() : connection(xcb_connect(nullptr, nullptr)),
            screen(xcb_setup_roots_iterator(xcb_get_setup(connection)).data)
    {}

    ~Xapi()
    {
        xcb_disconnect(connection);
        free(reply);
    }

    void create_window()
    {
        uint32_t mask = XCB_GC_FOREGROUND;
        uint32_t value[] = { screen->black_pixel };
        window = xcb_generate_id(connection); 
        xcb_create_window(  connection,
                            XCB_COPY_FROM_PARENT,
                            window, 
                            screen->root,
                            0, 0,
                            150, 150, 10,
                            XCB_WINDOW_CLASS_INPUT_OUTPUT,
                            screen->root_visual,
                            mask, value);


    }

    void map_window()
    {
        xcb_map_window(connection, window);
        xcb_flush(connection);
    }

    void screenshot()
    {
        xcb_get_image_cookie_t cookie;
        xcb_generic_error_t *error;

        cookie = xcb_get_image(connection, XCB_IMAGE_FORMAT_Z_PIXMAP, screen->root,
                            0, 0, 1920, 1080, ~0);

        reply = xcb_get_image_reply(connection, cookie, &error);

        uint32_t bytes = xcb_get_image_data_length(reply);
        uint8_t *idata = xcb_get_image_data(reply);
        xcb_flush(connection);

        cout << bytes << endl;
        
        Imlib_Image imlib_image = imlib_create_image_using_data(1920, 1080, (DATA32*) idata);
        imlib_context_set_image(imlib_image);
        imlib_image_set_format("jpeg");
        imlib_save_image("screenshot.jpg");

    }
};

// g++ -o %< % Screen.cpp -lopenh264 -lxcb -lxcb-image -lyuv -ltimer -lImlib2

int main ()
{
    Xapi api;

    //api.create_window();
    //api.map_window();

    for (int i{}; i < 10; ++i) {

        Timer t;
        int count{};
        while (t.elapsed() < 1.0) {
            api.screenshot();
            ++count;
        }
        cout << "Packs : " << count << endl;

    }

    //pause();

    return 0;
}

