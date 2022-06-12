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

template<typename uint_type>
class creatures {
public:

    creatures(const uint_type w, const uint_type h) : w_(w), h_(h), size_(w * h) {}

    inline auto get_top(const uint_type ix) -> uint_type {return ix - w_;}
    inline auto get_upper_left(const uint_type ix) -> uint_type {return ix - w_ - 1;}
    inline auto get_upper_right(const uint_type ix) -> uint_type {return ix - w_ + 1;}    
    inline auto get_left(const uint_type ix) -> uint_type {return ix - 1;}    
    inline auto get_right(const uint_type ix) -> uint_type {return ix + 1;}    
    inline auto get_bottom(const uint_type ix) -> uint_type {return ix + w_;}    
    inline auto get_bottom_left(const uint_type ix) -> uint_type {return ix + w_ - 1;}    
    inline auto get_bottom_right(const uint_type ix) -> uint_type {return ix + w_ + 1;}
    
    auto exists(const uint_type ix) -> bool {
        auto f = creature_count_.find(ix);
        return f < creature_count_.end();
    }

    auto out_of_bounds(const uint_type ix) -> bool {
        return ix >= size_;
    }

    auto get_existing_neighbor_count(const uint_type ix) -> uint_type {
        uint_type count {0};

        if (exists(get_top(ix))) ++count;
        if (exists(get_upper_left(ix))) ++count;
        if (exists(get_upper_right(ix))) ++count;
        if (exists(get_left(ix))) ++count;
        if (exists(get_right(ix))) ++count;
        if (exists(get_bottom(ix))) ++count;
        if (exists(get_bottom_left(ix))) ++count;
        if (exists(get_bottom_right(ix))) ++count;

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

    auto init_update(const std::unordered_set<uint_type> &ixs, uint_type n) -> void {
        creature_count_.clear();
        for (auto &g : grouped_creature_) g.clear();

        // for (uint_type i{0}, m {std::min<uint_type>(n, ixs.size())}; i < m; ++i);
        //     creature_count_.insert({ixs[0], 0});
        
        // for (auto &c : creature_count_) {
        //     c.second = get_existing_neighbor_count(c.first);
        // }
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

    auto preprocess_random_creatures() -> std::unordered_set<uint_type> {
        std::unordered_set<uint_type> random_creatures;
        
        std::vector<uint_type> all_creature_ids;
        all_creature_ids.reserve(screen_.size());
        for (uint_type i = 0; i < screen_.size(); ++i) all_creature_ids.emplace_back(i);
        for (uint_type i = 0; i < screen_.size(); ++i) std::swap(all_creature_ids[i], all_creature_ids[rand() % screen_.size()]);
        
        for (uint_type i = 0; i < 1000; ++i) random_creatures.insert(all_creature_ids[i]);
        
        return random_creatures;
    }

public:

    auto preprocess() -> bool {

        std::unordered_set<uint_type> all_creature_ids = preprocess_random_creatures();
        live_creatures_.init_update(all_creature_ids, 1000);

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