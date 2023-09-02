#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <timer.h>

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/composite.h>
#include <xcb/xproto.h>
#include <xcb/xtest.h>

struct Coord {
    int x{};
    int y{};
};

class ScreenData {
private:

    xcb_connection_t*       mConnection;
    xcb_screen_t*           mScreen;
    xcb_get_image_reply_t*  mReply{};
    xcb_window_t            mWindow;
    xcb_pixmap_t            mPixmap;
    xcb_gcontext_t          mGC;
    xcb_colormap_t          mColormap;
    xcb_generic_event_t*    mEvent;
    xcb_image_t*            mImage;

public:
    ScreenData();

    ~ScreenData();

    void map_window();
    void create_window(int width, int height);
    void put_image(void* data, int width, int height);
    void close_window();
    inline xcb_generic_event_t* eventCheck();
    uint8_t* screen_data(int width, int height);

    Coord get_pointer_pos();
    void mouse_right_click(Coord pos);
    void mouse_left_click(Coord pos);
    void mouse_left_press(Coord pos);
    void mouse_left_release(Coord pos);
    void move_pointer_to_coords(Coord cursor);
};
