/**
 * @file text_video_anim.hpp
 * @author Everett Gaius S. Vergara (me@everettgaius.com)
 * @brief GOL Bounds Template
 * @version 0.1
 * @date 2022-06-10
 * 
 * @copyright Copyright (c) 2022
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 */

#ifndef GOL_BOUNDS_HPP
#define GOL_BOUNDS_HPP

#include "common.hpp"

template<typename uint_type, typename int_type, uint_type w, uint_type h>
class gol_bounds {

protected:

    uint_type size_;
    std::array<int_type, 8> ref_;

public:

    gol_bounds() : size_(w * h) {
        ref_[0] = - w;
        ref_[1] = - w - 1;
        ref_[2] = - w + 1;
        ref_[3] = - 1;
        ref_[4] = + 1;
        ref_[5] = + w;
        ref_[6] = + w - 1;
        ref_[7] = + w + 1;
    }

    auto is_out_of_bounds(const uint_type ix) -> bool {return ix >= size_;}

    auto iterate(
        const uint_type ix, 
        const std::function<auto (const uint_type) -> bool> &condition,
        const std::function<auto (const uint_type) -> void> &if_action,
        const std::function<auto (const uint_type) -> void> &else_action) -> void {
        
        if (!if_action) return;
        for (auto &r : ref_) {
            uint_type i = ix + r; 
            if (condition(i)) if_action(i); 
            else if (else_action) else_action(i);
        }
    }
};

#endif