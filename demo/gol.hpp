/**
 * @file text_video_anim.hpp
 * @author Everett Gaius S. Vergara (me@everettgaius.com)
 * @brief Derived from text_video_anim to animate the PH Flag.
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

#ifndef GOL_HPP
#define GOL_HPP

#include "../include/text_video_anim.hpp"

using namespace g80;

using int_type = int16_t;
using uint_type = uint16_t;

constexpr uint_type SCREEN_WIDTH = 140;
constexpr uint_type SCREEN_HEIGHT = 40;
constexpr uint_type SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

constexpr uint_type FPS = 15;

// Creature List: 
// Use array instead of unordered_list for optimization purposes




// // Adj neighbors? could be merged with creatures count?
// using adjacent_neighbors = std::array<uint_type, 8>;

// // creatures_count, count_creatures
// using creatures_count_tuple = std::tuple<creatures_count, count_creatures>;



class gol : public text_video_anim<int_type, uint_type> {

public:
    
    gol() : 
        text_video_anim<int_type, uint_type>(SCREEN_WIDTH, SCREEN_HEIGHT, FPS),
        size_(SCREEN_SIZE) {

        }
    
    ~gol() = default;


private: 

    // auto preprocess_random_creatures() -> std::vector<uint_type> {
    //     // std::vector<uint_type> all_creature_ids;
    //     // all_creature_ids.reserve(screen_.size());
    //     // for (uint_type i = 0; i < screen_.size(); ++i) all_creature_ids[i] = i;
    //     // for (uint_type i = 0; i < screen_.size(); ++i) std::swap(all_creature_ids[i], all_creature_ids[rand() % screen_.size()]);
    //     // return all_creature_ids;
    // }

    auto neighbor_count(const uint_type creature_id) -> uint_type {
        
        // uint_type neighbor {0};

        // uint_type top = creature_id - screen_.width();
        // uint_type upper_left = top - 1;
        // uint_type upper_right = top + 1;
        // uint_type left = creature_id - 1;
        // uint_type right = creature_id + 1;
        // uint_type bottom = creature_id + screen_.width();
        // uint_type bottom_left = bottom - 1;
        // uint_type bottom_right = bottom + 1;    

        // if (top >= size_) top %= size_;
        // if (upper_left >= size_) upper_left %= size_;
        // if (upper_right >= size_) upper_right %= size_;
        // if (left >= size_) left %= size_;
        // if (right >= size_) right %= size_;
        // if (bottom >= size_) bottom %= size_;
        // if (bottom_left >= size_) bottom_left %= size_;
        // if (bottom_right >= size_) bottom_right %= size_;

        // if (creatures_.is_used(top)) ++neighbor;
        // if (creatures_.is_used(upper_left)) ++neighbor;
        // if (creatures_.is_used(upper_right)) ++neighbor;
        // if (creatures_.is_used(left)) ++neighbor;
        // if (creatures_.is_used(right)) ++neighbor;
        // if (creatures_.is_used(bottom)) ++neighbor;
        // if (creatures_.is_used(bottom_left)) ++neighbor;
        // if (creatures_.is_used(bottom_right)) ++neighbor;

        return neighbor;
    }


    auto update_creature(uint_type creature_id) -> void {

    }

public:

    auto preprocess() -> bool {


        return true;
    }

    auto update_erase_creatures() -> void {

    }
    


    auto update_execute_rules() -> void {

    }   

    auto update() -> bool {


        return true;
    }


private:

    uint_type size_;

};

#endif