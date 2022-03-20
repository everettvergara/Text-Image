#ifndef _AREA_HPP_
#define _AREA_HPP_ 

#include "Dim.hpp"

namespace g80 {

    struct Area {

        Dim w{0};
        Dim h{0};

        Area () {}
        Area (Dim w, Dim h) : 
            w(w), h(h) {}

        auto operator()() const -> Dim {
            return w * h;
        }
    };

}

#endif 