#include "ScreenData.h"
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xproto.h>

ScreenData::ScreenData() : 
    mConnection(xcb_connect(nullptr, nullptr)),
    mScreen(xcb_setup_roots_iterator(xcb_get_setup(mConnection)).data)
{
    xcb_flush(mConnection);
}

ScreenData::~ScreenData()
{
    xcb_disconnect(mConnection);
    if (mReply) free(mReply);
}

void ScreenData::create_window(int width, int height)
{
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t value_mask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS;
    uint32_t values[] = { mScreen->black_pixel, value_mask };
    int depth = mScreen->root_depth;
    int win_class = XCB_WINDOW_CLASS_INPUT_OUTPUT;

    mColormap = mScreen->default_colormap;
    mGC = xcb_generate_id(mConnection);
    mWindow = xcb_generate_id(mConnection); 
    mPixmap = xcb_generate_id(mConnection); 

    xcb_create_window(  mConnection,
                        depth,
                        mWindow,
                        mScreen->root,
                        0, 0,
                        width, height, 1,
                        win_class,
                        mScreen->root_visual,
                        mask, values);

    xcb_create_pixmap(mConnection, depth, mPixmap, mWindow, width, height);
    xcb_create_gc(mConnection, mGC, mPixmap, 0, NULL);
    mImage = xcb_image_create_native(
            mConnection, width, height, XCB_IMAGE_FORMAT_Z_PIXMAP,
            mScreen->root_depth, NULL, 0, NULL);
}

void ScreenData::map_window()
{
    xcb_map_window(mConnection, mWindow);
    xcb_flush(mConnection);
}

void ScreenData::put_image(void* data, int width, int height)
{
    mImage->data = (uint8_t*)data;
    xcb_image_put(mConnection, mPixmap, mGC, mImage, 0, 0, 0);
    xcb_copy_area(mConnection, mPixmap, mWindow, mGC, 0, 0, 0, 0, width, height);
    xcb_flush(mConnection);
    //xcb_image_destroy(image);
}

void ScreenData::close_window()
{
    xcb_free_pixmap(mConnection, mPixmap);
    xcb_free_gc(mConnection, mGC);
}

inline xcb_generic_event_t* ScreenData::eventCheck()
{
    return xcb_poll_for_event(mConnection);
}

uint8_t* ScreenData::screen_data(int width, int height)
{
    xcb_get_image_cookie_t image_cookie = xcb_get_image_unchecked(
            mConnection, XCB_IMAGE_FORMAT_Z_PIXMAP, 
            mScreen->root, 0, 0, width, height, ~0);
    xcb_flush(mConnection);

    xcb_connection_has_error(mConnection);
    if (mReply) {
        free(mReply);
        mReply = nullptr;
    }
    mReply = xcb_get_image_reply(mConnection, image_cookie, nullptr);
    uint8_t* pData = xcb_get_image_data(mReply);
    //std::cerr << "after" << std::endl;

    xcb_flush(mConnection);
    return pData;
}

Coord ScreenData::get_pointer_pos()
{
    Coord c;

    xcb_query_pointer_cookie_t pos = xcb_query_pointer(mConnection, mScreen->root);
    xcb_query_pointer_reply_t* reply = xcb_query_pointer_reply(mConnection, pos, NULL);

    c.x = reply->root_x;
    c.y = reply->root_y;

    return c;
}

void ScreenData::move_pointer_to_coords(Coord cursor)
{
    xcb_warp_pointer(mConnection, XCB_NONE,
            mScreen->root, 0, 0, 0, 0, cursor.x, cursor.y);
    xcb_flush(mConnection);
}

void ScreenData::mouse_left_click(Coord pos)
{
    xcb_test_fake_input(mConnection, XCB_BUTTON_PRESS, XCB_BUTTON_INDEX_1,
            XCB_CURRENT_TIME, XCB_NONE, pos.x, pos.y, 0);
    xcb_test_fake_input(mConnection, XCB_BUTTON_RELEASE, XCB_BUTTON_INDEX_1,
            XCB_CURRENT_TIME, XCB_NONE, pos.x, pos.y, 0);
    xcb_flush(mConnection);
}

void ScreenData::mouse_left_press(Coord pos)
{
    xcb_test_fake_input(mConnection, XCB_BUTTON_PRESS, XCB_BUTTON_INDEX_1,
            XCB_CURRENT_TIME, XCB_NONE, pos.x, pos.y, 0);
    xcb_flush(mConnection);
}

void ScreenData::mouse_left_release(Coord pos)
{
    xcb_test_fake_input(mConnection, XCB_BUTTON_RELEASE, XCB_BUTTON_INDEX_1,
            XCB_CURRENT_TIME, XCB_NONE, pos.x, pos.y, 0);
    xcb_flush(mConnection);
}
