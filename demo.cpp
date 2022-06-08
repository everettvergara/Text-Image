#include <iostream>

#include "text_image.hpp"

using namespace g80;
auto main(const int argc, const char *argv[]) -> int {

    text_image ti(140, 30, 7, '.', ON);
    ti.show();
    
}