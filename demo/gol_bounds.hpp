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

#include <functional>
#include "gol_common.hpp"

using fparameter_bool = std::function<auto (const uint_type) -> bool>;
using fparameter_void = std::function<auto (const uint_type) -> void>;

/**
 * A singleton template for 
 * processing indexes and its boundaries
 * 
 */

template<typename int_type, typename uint_type, uint_type w, uint_type h>
class gol_bounds {

/**
 * Constructors, Assignments and 
 * Instance
 * 
 */

private:

    static uint_type size_;
    static std::array<int_type, 8> ref_bounds_;

private:

    gol_bounds() {}

public:

    gol_bounds(const gol_bounds &) = delete;
    auto operator=(const gol_bounds &) -> gol_bounds & = delete;

    static auto get_instance() -> gol_bounds & {
        static gol_bounds<int_type, uint_type, w, h> instance;
        return instance;
    }


/**
 * Public Interface
 * 
 */

public: 

    auto is_out_of_bounds(const uint_type ix) -> bool {return ix >= size_;}

    auto iterate(
        const uint_type ix, 
        const fparameter_bool &condition,
        const fparameter_void &if_action,
        const fparameter_void &else_action) -> void {
        
        if (!if_action) return;
        for (auto &r : ref_bounds_) {
            uint_type i = ix + r; 
            if (condition(i)) if_action(i); 
            else if (else_action) else_action(i);
        }
    }
};

/**
 * Static vars instantiation
 * per template
 * 
 */

template<typename int_type, typename uint_type, uint_type w, uint_type h>
uint_type gol_bounds<int_type, uint_type, w, h>::size_ = w * h;

template<typename int_type, typename uint_type, uint_type w, uint_type h>
std::array<int_type, 8> gol_bounds<int_type, uint_type, w, h>::ref_bounds_ = {- w, - w - 1, - w + 1, - 1, + 1, + w, + w - 1, + w + 1};

#endif