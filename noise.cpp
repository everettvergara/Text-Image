#include <iostream>
#include <chrono>
#include <thread>
#include <cstdio>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <cstdlib>

#include "TextImage.h"

using namespace g80;
const int FPS = 15;
const int MSPF = 1000 / FPS;
namespace chr = std::chrono;
namespace this_thread = std::this_thread;
using TimePointSysClock = chr::time_point<chr::system_clock>;
using SysClock = chr::system_clock;

auto is_key_pressed() -> int;

auto main(int argc, char **argv) -> int {
    
    TextImage noise({130, 40}, ON);
    noise.fill_color(7);
    
    do {
        TimePointSysClock start {SysClock::now()};

        auto &raw_text = noise.raw_text();
        for (int i = 0; i < noise.area()(); ++i) {
            raw_text[i] = 'A' + rand() % ('z' - 'A');
        }
        noise.show();

        TimePointSysClock end = SysClock::now();
        int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
        if (delay > 0) 
            this_thread::sleep_for(chr::milliseconds(delay));

    } while(!is_key_pressed());
    
}


auto is_key_pressed() -> int {
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized) {
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting); 
    return bytesWaiting;
}
