#include <iostream>
#include <chrono>

#include "TextImage.h"

using namespace g80;

auto main(int argc, char **argv) -> int {

    TextImage text_image({140, 50}, 7, ' ', ON);
    text_image.gfx_arc({70, 25}, 20, 25, 359, '*', 3, OFF);
    text_image.show();

}



