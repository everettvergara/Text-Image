#include <iostream>
#include <chrono>

#include "TextImage.h"

using namespace g80;

auto main(int argc, char **argv) -> int {

    TextImage text_image({140, 60}, 7, '.', ON);

    // start and end
    text_image.gfx_arc({70, 25}, 25, 60, 365, '*', 3, ON);
    text_image.show(); 

}


