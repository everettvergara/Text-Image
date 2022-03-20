#include <iostream>
#include <chrono>

#include "TextImage.h"

using namespace g80;

auto main(int argc, char **argv) -> int {

    TextImage text_image({30, 30}, 7, '.', ON);
    //TextImage put_image({11, 11}, 1, 'x', OFF);

    // for (int i = 0; i < 30; ++i) {
    //     text_image.gfx_point(i, i, 'A' + i, 1 + (i % 7), OFF);
    //     // text_image.set_text(i, i, 'A' + i);
    //     // text_image.set_color(i, i, 1 + (i % 7));
    //     // text_image.set_mask(i, i, OFF);
    //     // text_image.set_mask(7, i, OFF);
    //     // text_image.set_mask(9, i, OFF);
    //     // text_image.set_mask(0, i, OFF);
    //     // text_image.set_mask(29, i, OFF);
    // }


    // text_image.xlat_rotate_left(10, ALL);
    // text_image.gfx_line(0,0,29,29, 'x', 1, OFF);
    // text_image.gfx_line(29,0,0,29, '-', 2, OFF);
    text_image.gfx_circle(15, 15, 14, 'o', 3, OFF);
    text_image.gfx_fill_with_text_border(15, 15, 'o', 4, ON);
    text_image.show();

}
