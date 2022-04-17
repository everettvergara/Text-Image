#include <iostream>
#include <cmath>

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

using namespace g80;
const int FPS = 15;
const int MSPF = 1000 / FPS;
namespace chr = std::chrono;
namespace this_thread = std::this_thread;
using TimePointSysClock = chr::time_point<chr::system_clock>;
using SysClock = chr::system_clock;


auto delayer(TimePointSysClock start) -> void;
auto is_key_pressed() -> int;

auto main(int argc, char **argv) -> int {

    Dim ball_radius = 15;
    TextImage ball({ball_radius * 2 + 1, ball_radius * 2 + 1}, ON);
    TextImage screen({130, 40}, ON);

    ball.gfx_circle_text({15, 15}, 15, 'x');
    ball.gfx_fill_with_text_border({15, 15}, 'x');
    
    for (Dim y = 0; y < 10; ++y) 
        for (Dim x = 0; x < 31; ++x) 
            ball.set_color(ball.index({x, y}), 4);
        
    for (Dim y = 10; y < 20; ++y) 
        for (Dim x = 0; x < 31; ++x) 
            ball.set_color(ball.index({x, y}), 5);
    
    for (Dim y = 20; y < 31; ++y) 
        for (Dim x = 0; x < 31; ++x) 
            ball.set_color(ball.index({x, y}), 6);

    for (Dim y = 14; y < 17; ++y) 
        for (Dim x = 0; x < 31; ++x) 
            ball.set_color(ball.index({x, y}), 7);



    Dim ball_start_x = static_cast<Dim>(screen.area().w / 2 - ball.area().w / 2);
    Dim ball_start_y = static_cast<Dim>(screen.area().h / 2 - ball.area().h / 2);

    screen.or_image(ball, {ball_start_x, ball_start_y});
    screen.or_image(ball, {static_cast<Dim>(ball_start_x - ball_radius * 2), ball_start_y});
    screen.or_image(ball, {static_cast<Dim>(ball_start_x + ball_radius * 2), ball_start_y});
    screen.show();

    do {

        TimePointSysClock start_delay_timer {SysClock::now()};

        for (Dim c = 0; c < 30; ++c) {
            Dim x = static_cast<Dim>(ball_start_x - ball_radius + rand() % (ball_radius * 4 + 1));
            Dim y = static_cast<Dim>(rand() % (ball_radius * 2 + 1));
            TextImage droplets = screen.get_image({{x, y}, {2, 3}});
            screen.or_image(droplets, {x, static_cast<Dim>(y + 2)});
        }
        screen.show();

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
