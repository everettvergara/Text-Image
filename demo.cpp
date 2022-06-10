#include <iostream>

// #include "text_image.hpp"
#include "text_video_anim.hpp"

using namespace g80;

// Todo: Test all functions

auto main(const int argc, const char *argv[]) -> int {

    // text_image<int16_t, uint16_t> ti(140, 30, 7, '`', ON);
    // ti.gfx_line_text(0,0,139,29, '*');
    // ti.gfx_circle_text(70, 15, 10, '*');
    // ti.gfx_fill_with_text_border(70, 15, 1, '*', ON);
    // // ti.gfx_fill_text(70, 15, '*');
    // ti.show();

    text_video_anim<int16_t, uint16_t> tva(130, 40, 15);

    tva.preprocess();
    tva.run();

}