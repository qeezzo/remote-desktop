#include <iostream>
#include <timer.h>
#include <Imlib2.h>

#include "NetServer.h"
#include "H264Encoder.h"
#include "ScreenData.h"


int main()
{
    using std::cout, std::endl, std::cerr;

    const int width { 1920 };
    const int height { 1080 };
    
    try {

        struct Cursor {
            Coord pos{};
            bool click{};
        };

        NetServer server(4000);
        
        H264Encoder encoder(width, height);
        ScreenData scrData;

        bool pressed{};
        for (int i{}; i < 60*10; ++i) {
            Timer t;
            int counter{};
            while (t.elapsed() < 1.0) {

                uint8_t* screen_data = scrData.screen_data(width, height);

                Packet* pack=encoder.encodeFrame(screen_data);
                if (pack) {
                    server.send(pack->getPtrToHeader(), pack->getSizeWithHeader());

                    Cursor cursor;
                    server.recv(&cursor, sizeof(Cursor));

                    if (cursor.pos.x > 0 || cursor.pos.y > 0)
                        scrData.move_pointer_to_coords(cursor.pos);

                    if (cursor.click && !pressed) {
                        cout << "pressed" << endl;
                        scrData.mouse_left_press(cursor.pos);
                        pressed = true;
                    } else if (pressed && !cursor.click) {
                        cout << "released" << endl;
                        scrData.mouse_left_release(cursor.pos);
                        pressed = false;
                    }

                    ++counter;
                }
            }
            cout << "Frames : " << counter << endl;
        }

        scrData.close_window();

    } catch (const char* msg) {
        cerr << msg << endl;
    }

}
