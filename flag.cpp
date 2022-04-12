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
auto draw_flag(TextImage &flag) -> void;

auto main(int argc, char **argv) -> int {
    
    const Dim wave_height = 6;
    const Dim flag_height = 30;
    const Dim flag_width = 120;
    const double waves = 8.0;
    
    TextImage screen({flag_width, flag_height + wave_height + 5}, 7, ' ', ON);
    TextImage flag({flag_width, flag_height}, 7, '.', ON);
    
    
    // Draw the flag
    flag.fill_text("BayangmagiliwPerlasngSilangananAlabngpusoSadibdibmoybuhayLupangHinirangDuyankangmagitingSamanlulupigDikapasisiilSadagatatbundokSsimoyatsalangitmongbughawMaydilagangtulaAtawitsapaglayangminamahalAngkislapngwatawatmoyTagumpaynanagniningningAngbituinatarawniyaKailanpamaydimagdidilimLupangarawngluwalhatitpagsintaBuhayaylangitsapilingmoAmingligayana'pagmaymangaapiAngmamatayngdahilsaiyo");
    //flag.fill_text("Bayang magiliw Perlas ng Silanganan Alab ng puso Sa dibdib moy buhay Lupang Hinirang Duyan ka ng magiting  Sa manlulupig Di ka pasisiil Sa dagat at bundok Sa simoy at sa langit mong bughaw  May dilag ang tula  At awit sa paglayang minamahal Ang kislap ng watawat moy Tagumpay na nagniningning Ang bituin at araw niya  Kailan pa may di magdidilim Lupa ng araw ng luwalhatit pagsinta Buhay ay langit sa piling mo  Aming ligaya na pag may mang-aapi Ang mamatay ng dahil sa iyo.");

    for (Dim i = 0; i < 15; ++i) {
        for (Dim j = 1 + i * 3; j < flag_width; ++j) {
            flag.set_color(flag.index({j, i}), 4);
            flag.set_color(flag.index({j, static_cast<Dim>(flag_height - i - 1)}), 1);
        }
    }

    flag.gfx_circle_color({14, 15}, 6, 3);
    flag.gfx_fill_with_color_border({14, 15}, 'X', 3, ON);
    for (double i = 0.0; i < 2.0 * M_PI; i += 2.0 * M_PI / 8.0) {
        flag.set_color({static_cast<Dim>(14.0 + 7.0 * cos(i)), static_cast<Dim>(15.0 + 7.0 * sin(i))}, 3);
        flag.set_color({static_cast<Dim>(14.0 + 8.0 * cos(i)), static_cast<Dim>(15.0 + 8.0 * sin(i))}, 3);
    }

    // Stars
    flag.set_color({3,3}, 3);
    flag.set_color({4,4}, 3);
    flag.set_color({5,5}, 3);
    flag.set_color({5,3}, 3);
    flag.set_color({3,5}, 3);

    flag.set_color({3, flag_height - 3 - 1}, 3);
    flag.set_color({4,flag_height - 4 - 1}, 3);
    flag.set_color({5,flag_height - 5 - 1}, 3);
    flag.set_color({5,flag_height - 3 - 1}, 3);
    flag.set_color({3,flag_height - 5 - 1}, 3);

    flag.set_color({33, 14}, 3);
    flag.set_color({34, 15}, 3);
    flag.set_color({35, 16}, 3);
    flag.set_color({35, 14}, 3);
    flag.set_color({33, 16}, 3);

    Dim x = 0;
    std::array<Dim, flag_width> y;
    std::array<Dim, flag_width> yn;

    double inc = waves * M_PI / flag_width;
    for (double a = 0; a < waves * M_PI; a += inc) {
        y[x] = (wave_height / 2.0) - sin(a) * (wave_height / 2.0 - 1) - 1;
        yn[x] = +1;
        ++x;
    }

    do {

        TimePointSysClock start_delay_timer {SysClock::now()};
        
        screen.fill_text(" ");

        flag.xlat_rotate_right(flag_width - 1, TEXT);
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
        // exit(0);

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

auto draw_flag(TextImage &flag) -> void {

}