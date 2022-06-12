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

#include <cstdint>
#include <algorithm>
#include <array>

#include "index_bin.hpp"
#include "../include/text_video_anim.hpp"

using namespace g80;

using int_type = int16_t;
using uint_type = uint16_t;
using creature = uint_type;
using count = uint_type;

constexpr uint_type SCREEN_WIDTH = 140;
constexpr uint_type SCREEN_HEIGHT = 40;
constexpr uint_type SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

constexpr uint_type FPS = 15;

// Creature List: 
// Use array instead of unordered_list for optimization purposes

using creatures = index_bin<uint_type>;
using creatures_count = std::array<uint_type, SCREEN_SIZE>;
using grouped_creatures = std::array<index_bin<uint_type>, 9>;



// // Adj neighbors? could be merged with creatures count?
// using adjacent_neighbors = std::array<uint_type, 8>;

// // creatures_count, count_creatures
// using creatures_count_tuple = std::tuple<creatures_count, count_creatures>;



class gol : public text_video_anim<int_type, uint_type> {

public:
    
    gol() : 
        text_video_anim<int_type, uint_type>(SCREEN_WIDTH, SCREEN_HEIGHT, FPS),
        creatures_{SCREEN_SIZE},
        creatures_count_{0},
        grouped_creatures_{SCREEN_SIZE, SCREEN_SIZE, SCREEN_SIZE, SCREEN_SIZE, SCREEN_SIZE, SCREEN_SIZE, SCREEN_SIZE, SCREEN_SIZE, SCREEN_SIZE} {

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
        
        uint_type neighbor {0};

        uint_type top = creature_id - screen_.width();
        uint_type upper_left = top - 1;
        uint_type upper_right = top + 1;
        uint_type left = creature_id - 1;
        uint_type right = creature_id + 1;
        uint_type bottom = creature_id + screen_.width();
        uint_type bottom_left = bottom - 1;
        uint_type bottom_right = bottom + 1;    

        if (creatures_.is_used(top)) ++neighbor;
        if (creatures_.is_used(upper_left)) ++neighbor;
        if (creatures_.is_used(upper_right)) ++neighbor;
        if (creatures_.is_used(left)) ++neighbor;
        if (creatures_.is_used(right)) ++neighbor;
        if (creatures_.is_used(bottom)) ++neighbor;
        if (creatures_.is_used(bottom_left)) ++neighbor;
        if (creatures_.is_used(bottom_right)) ++neighbor;

        return neighbor;
    }


    auto update_creature(uint_type creature_id) -> void {
        auto current_count = creatures_count_[creature_id];
        grouped_creatures_[current_count].unuse(creature_id);

        auto new_count = neighbor_count(creature_id);
        creatures_count_[creature_id] = new_count;
        grouped_creatures_[new_count].use(creature_id);
    }

public:

    auto preprocess() -> bool {
        
        // All counts must be 0 and add all creatures to group 0 since they are unused
        std::fill_n(creatures_count_.data(), creatures_count_.size(), 0);
        for (uint_type i{0}; i < SCREEN_SIZE; ++i) grouped_creatures_[0].use(i);

        // Shuffle creatures to be considered in the demo 
        std::array<uint_type, SCREEN_SIZE> random_creatures;
        for (uint_type i{0}; i < SCREEN_SIZE; ++i) 
            std::swap(random_creatures[0], random_creatures[rand() % SCREEN_SIZE]);

        // Get only the first N
        for (uint_type i{0}; i < size_; ++i) {
            auto ix {random_creatures[i]};
            creatures_.use(ix);
            auto count = {neighbor_count(ix)};
            if (count > 0) {
                creatures_count_[ix] = count;
                grouped_creatures_[0].unuse(ix);
                grouped_creatures_[count].use(ix);
            }
        }

        return true;
    }

    auto update() -> bool {


        return true;
    }

private:

    uint_type size_;
    creatures creatures_;
    creatures_count creatures_count_;
    grouped_creatures grouped_creatures_;
};

#endif