#include "Screen.h"

using namespace scr;

scr::Screen::Screen() : connection(xcb_connect(nullptr, nullptr)),
        screen(xcb_setup_roots_iterator(xcb_get_setup(connection)).data)
{
    xcb_flush(connection);
}

scr::Screen::~Screen()
{
    xcb_disconnect(connection);
}

void scr::Screen::create_window()
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

void scr::Screen::map_window()
{
    xcb_map_window(connection, window);
    xcb_flush(connection);
}

uint8_t* scr::Screen::screen_data(int width, int height)
{
    xcb_get_image_cookie_t image_cookie = xcb_get_image(
            connection, XCB_IMAGE_FORMAT_Z_PIXMAP, 
            screen->root, 0, 0, width, height, ~0);
    reply = xcb_get_image_reply(connection, image_cookie, nullptr);
    idata = xcb_get_image_data(reply);
    xcb_flush(connection);


    return idata;
}

void scr::Screen::screen_free_data()
{
    free(reply);
}
