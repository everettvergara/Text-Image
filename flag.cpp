#include <iostream>
#include <cmath>

#include "TextImage.h"

using namespace g80;

auto main(int argc, char **argv) -> int {
    
    const Dim wave_radius = 10;
    const Dim flag_height = 30;
    const Dim flag_width = 120;
    TextImage flag({flag_width, flag_height + wave_radius}, 7, '.', ON);

    // Populate the flag,
    for (Dim y = 0; y < flag_height; ++y)
        for (Dim x = 0; x < flag_width; ++x)
            flag.set_text(flag.index({x, y}), 'a' + rand() % 26);

    double inc = 2.0 * M_PI / flag_width;
    Dim x = 0;
    for (double a = 0; a < 2 * M_PI; a += inc) {
        Dim y = sin(a) * 5;
        flag.set_text(flag.index({x++, static_cast<Dim>(flag_height + 5 - y)}), '@');
    }

    flag.show();
}