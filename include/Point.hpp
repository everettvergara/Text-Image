/*
 *  Point Struct for g80::TextImage Class
 *  Copyright (C) 2022 Everett Gaius S. Vergara
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *	
 */

#ifndef _POINT_HPP_
#define _POINT_HPP_ 

#include "Dim.hpp"

namespace g80 {

    struct Point {
        Dim x;
        Dim y;
        auto operator==(const Point &rhs) const -> bool {
            return x == rhs.x && y == rhs.y;
        }
    };

}

#endif 