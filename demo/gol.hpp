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

#include <unordered_map>
#include <unordered_set>
#include <array>
#include <algorithm>

#include "../include/text_video_anim.hpp"

using namespace g80;

using int_type = int16_t;
using uint_type = uint16_t;

constexpr uint_type SCREEN_WIDTH = 140;
constexpr uint_type SCREEN_HEIGHT = 40;
constexpr uint_type SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;
constexpr uint_type FPS = 15;

template<typename uint_type, typename int_type, uint_type w, uint_type h>
struct bounds {

    uint_type size;
    std::array<int_type, 8> ref;
    bounds() : size(w * h) {
        ref[0] = - w;
        ref[1] = - w - 1;
        ref[2] = - w + 1;
        ref[3] = - 1;
        ref[4] = + 1;
        ref[5] = + w;
        ref[6] = + w - 1;
        ref[7] = + w + 1;
    }

    auto out_of_bounds(const uint_type ix) -> bool {return ix >= size;}

    auto iterate_bounds(
        const uint_type ix, 
        const std::function<auto (const uint_type) -> bool> &condition,
        const std::function<auto (const uint_type) -> void> &if_action,
        const std::function<auto (const uint_type) -> void> &else_action) {
        
        for (auto &r : ref) {
            uint_type p = ix + r; 
            if (condition(p)) if_action(p); 
            else else_action(p);
        }
    }
};

bounds<uint_type, int_type, SCREEN_WIDTH, SCREEN_HEIGHT> SCR_BND;

template<typename uint_type>
class creatures {
public:

    creatures(const uint_type w, const uint_type h) : w_(w), h_(h), size_(w * h) {}

    auto exists(const uint_type ix) -> bool {
        auto f = creature_count_.find(ix);
        return f < creature_count_.end();
    }

    auto get_existing_neighbor_count(const uint_type ix) -> uint_type {
        uint_type count {0};


        // SCR_BND.iterate_bounds(ix, nullptr, nullptr, nullptr);
        // if (exists(screen_bounds.get_top(ix))) ++count;
        // if (exists(screen_bounds.get_upper_left(ix))) ++count;
        // if (exists(screen_bounds.get_upper_right(ix))) ++count;
        // if (exists(screen_bounds.get_left(ix))) ++count;
        // if (exists(screen_bounds.get_right(ix))) ++count;
        // if (exists(screen_bounds.get_bottom(ix))) ++count;
        // if (exists(screen_bounds.get_bottom_left(ix))) ++count;
        // if (exists(screen_bounds.get_bottom_right(ix))) ++count;

        return count;
    }

    auto size() -> uint_type {
        return static_cast<uint_type>(creature_count_.size());
    }

    auto update(const uint_type ix, const uint_type count) -> void {
        auto f = creature_count_.find(ix);
        if (f == creature_count_.end()) {
            creature_count_.insert({ix, count});
        } else {
            auto pcount = f->second;
            grouped_creature_[pcount].erase(ix);
            f->second = count;
        }
        grouped_creature_[count].insert(ix);
    }

private:

    uint_type w_, h_, size_;
    std::unordered_map<uint_type, uint_type> creature_count_;
    std::array<std::unordered_set<uint_type>, 9> grouped_creature_;
};


class gol : public text_video_anim<int_type, uint_type> {

public:
    
    gol() : 
        text_video_anim<int_type, uint_type>(SCREEN_WIDTH, SCREEN_HEIGHT, FPS),
        live_creatures_(SCREEN_WIDTH, SCREEN_HEIGHT),
        potential_creatures_(SCREEN_WIDTH, SCREEN_HEIGHT) {

        }
    
    ~gol() = default;


private: 

    auto preprocess_random_creatures(const uint_type n) -> std::unordered_set<uint_type> {
        std::unordered_set<uint_type> random_creatures;
        
        std::vector<uint_type> live_creatures;
        live_creatures.reserve(screen_.size());
        for (uint_type i = 0; i < screen_.size(); ++i) live_creatures.emplace_back(i);
        for (uint_type i = 0; i < screen_.size(); ++i) std::swap(live_creatures[i], live_creatures[rand() % screen_.size()]);
        for (uint_type i = 0; i < n; ++i) random_creatures.insert(live_creatures[i]);
        
        return random_creatures;
    }

public:

    auto preprocess() -> bool {

        std::unordered_set<uint_type> live = preprocess_random_creatures(1000);
        std::unordered_set<uint_type> potential;

        uint_type count;
        std::function<auto (const uint_type ix) -> bool> if_exists = [&](const uint_type ix) -> bool {auto f = live.find(ix); return f != live.end();};
        std::function<auto (const uint_type ix) -> void> then_inc_count = [&](const uint_type ix) -> void {++count;};
        std::function<auto (const uint_type ix) -> void> else_add_to_potential = [&](const uint_type ix) -> void {potential.insert(ix);};

        for (auto &ix : live) {
            count = 0;
            SCR_BND.iterate_bounds(ix, if_exists, then_inc_count, else_add_to_potential);
            live_creatures_.update(ix, count);
        }


        // for (auto &c : potential_creatures) {
        //     count = 0;
        //     if (exists(screen_bounds.get_top(c))) ++count;
        //     potential_creatures(c, count);
        // }

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

    creatures<uint_type> live_creatures_;
    creatures<uint_type> potential_creatures_;
};

#endif