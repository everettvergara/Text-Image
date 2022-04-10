#include <iostream>
#include <cmath>
#include <array>

// Includes for delayer
#include <chrono>
#include <thread>

// Includes for is_key_pressed
#include <cstdio>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <cstdlib>

#include "TextImage.h"

// Shortcuts
using namespace g80;
namespace chr = std::chrono;
namespace this_thread = std::this_thread;
using TimePointSysClock = chr::time_point<chr::system_clock>;
using SysClock = chr::system_clock;

// Game configurations 
constexpr Dim FPS = 15;
constexpr Dim MSPF = 1000 / FPS;

auto delayer(TimePointSysClock start) -> void;
auto is_key_pressed() -> int;

auto main(int argc, char **argv) -> int {
    
    const Dim wave_height = 6;
    const Dim flag_height = 30;
    const Dim flag_width = 120;
    const double waves = 8.0;
    
    TextImage screen({flag_width, flag_height + wave_height}, 7, ' ', ON);
    TextImage flag({flag_width, flag_height + wave_height}, 7, '.', ON);

    // Populate the flag,
    for (Dim y = 0; y < flag_height; ++y)
        for (Dim x = 0; x < flag_width; ++x)
            flag.set_text(flag.index({x, y}), 'a' + rand() % 26);

    Dim x = 0;
    std::array<Dim, flag_width> y;
    std::array<Dim, flag_width> yn;

    double inc = waves * M_PI / flag_width;
    for (double a = 0; a < waves * M_PI; a += inc) {
        y[x] = (wave_height / 2.0) - sin(a) * (wave_height / 2.0 - 1) - 1;
        yn[x] = +1;
        //flag.set_text(flag.index({x, static_cast<Dim>(y[x])}), '@');
        ++x;
    }
    // std::cout << "x: " << x << "\n";
    // flag.show();
    // exit(0);

    do {

        TimePointSysClock start_delay_timer {SysClock::now()};
        
        screen.fill_text(" ");

        for (Dim x = 0; x < flag_width; ++x) {
            TextImage vertical_line = flag.get_image({{x, 0}, {1, flag_height}});
            screen.put_image(vertical_line, {x, static_cast<Dim>(wave_height / 2  + y[x])});
            y[x] = y[x] + yn[x];
            if (y[x] <= 0) {
                y[x] = 1;
                yn[x] = 1;
            } else if (y[x] >= wave_height - 1) {
                y[x] = wave_height - 2;
                yn[x] = -1;
            } 
        }

        screen.show();
        //exit(0);

        delayer(start_delay_timer);

    } while(!is_key_pressed());

}


auto delayer(TimePointSysClock start) -> void {
    TimePointSysClock end = SysClock::now();
    Dim delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
    if (delay > 0) 
        this_thread::sleep_for(chr::milliseconds(delay));
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
