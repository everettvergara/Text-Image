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


#include <tuple>

#include "gol_common.hpp"
#include "gol_bounds.hpp"
#include "gol_creatures.hpp"
#include "../include/text_video_anim.hpp"

using namespace g80;
constexpr uint_type SCREEN_WIDTH = 130;
constexpr uint_type SCREEN_HEIGHT = 30;
constexpr uint_type SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;
constexpr uint_type FPS = 5;

using SCR_BND = gol_bounds<int_type, uint_type, SCREEN_WIDTH, SCREEN_HEIGHT>;


class gol : public text_video_anim<int_type, uint_type> {
public:
    
    gol() : 
        text_video_anim<int_type, uint_type>(SCREEN_WIDTH, SCREEN_HEIGHT, FPS),
        live_creatures_(SCREEN_WIDTH, SCREEN_HEIGHT),
        potential_creatures_(SCREEN_WIDTH, SCREEN_HEIGHT) {

        }
    
    ~gol() = default;


private: 

    auto preprocess_random_creatures(const uint_type n) -> uoset_ix {
        uoset_ix random_creatures;
        std::vector<uint_type> live_creatures;
        live_creatures.reserve(screen_.size());
        for (uint_type i = 0; i < screen_.size(); ++i) live_creatures.emplace_back(i);
        for (uint_type i = 0; i < screen_.size(); ++i) std::swap(live_creatures[i], live_creatures[rand() % screen_.size()]);
        for (uint_type i = 0; i < n; ++i) random_creatures.insert(live_creatures[i]);
        return random_creatures;
    }

public:

    auto preprocess() -> bool {
        uoset_ix live = preprocess_random_creatures(2500);
        uoset_ix potential;

        uint_type count;
        using fparameter_bool = std::function<auto (const uint_type ix) -> bool>;
        using fparameter_void = std::function<auto (const uint_type ix) -> void>;
        fparameter_bool if_exists = [&](const uint_type ix) -> bool {auto f = live.find(ix); return f != live.end();};
        fparameter_void then_inc_count = [&](const uint_type ix) -> void {++count;};
        fparameter_void else_add_to_potential = [&](const uint_type ix) -> void {potential.insert(ix);};

        for (auto &ix : live) {
            count = 0;
            SCR_BND::get_instance().iterate(ix, if_exists, then_inc_count, else_add_to_potential);
            live_creatures_.update(ix, count);
        }

        for (auto &ix : potential) {
            count = 0;
            if (ix < screen_.size()) SCR_BND::get_instance().iterate(ix, if_exists, then_inc_count, nullptr);
            if (count == 3) potential_creatures_.update(ix, count);
        }

        return true;
    }

    auto update_erase_creatures() -> void {
        for (auto &c : live_creatures_.get_creature_count()) 
            screen_.set_text(c.first, ' ');
    }

    auto update_execute_rules_get_vars() -> std::tuple<uoset_ix, uoset_ix, uoset_ix> {
        
        uoset_ix to_get_neighbors;
        uoset_ix to_update_count;
        uoset_ix to_spawn;

        for (uint_type group{0}; group <= 8; ++group) {
            
            // Spaces with 3 neighbors 
            // will be spawned
            if (group == 3) {
                for (auto ix : potential_creatures_.get_grouped_creatures(group)) {
                    to_spawn.insert(ix);
                    to_get_neighbors.insert(ix);
                }
                potential_creatures_.kill_group(group);
            
            // Creatures with 2 neighbors will remain
            // but the count needs to get updated
            } else if (group == 2) {
                for (auto &ix : live_creatures_.get_grouped_creatures(group)) 
                    to_update_count.insert(ix);


            // Under population (0, 1) and 
            // over population (4, 5, 6, 7, 8) will be killed
            } else {
                for (auto &ix : live_creatures_.get_grouped_creatures(group)) 
                    to_get_neighbors.insert(ix);

                live_creatures_.kill_group(group);
            }
        }

        return {to_get_neighbors, to_update_count, to_spawn};
    }

    auto update_execute_rules() -> void {

        std::tuple<uoset_ix, uoset_ix, uoset_ix> vars = update_execute_rules_get_vars();
        uoset_ix &to_get_neighbors = std::get<0>(vars);
        uoset_ix &to_update_count = std::get<1>(vars);
        uoset_ix &to_spawn = std::get<2>(vars);

        // Recalc neighbors
        uint_type count;
        uoset_ix additional_to_update_count;
        std::function<auto (const uint_type) -> bool> if_always_true = [](const uint_type) {return true;};
        std::function<auto (const uint_type) -> bool> if_bound_exists = [&](const uint_type ix) {return live_creatures_.exists(ix);};
        std::function<auto (const uint_type) -> void> then_add_to_update_count = [&](const uint_type ix) {to_update_count.insert(ix);};
        std::function<auto (const uint_type) -> void> then_inc_count = [&](const uint_type ix) {++count;};
        
        for (auto &s : to_spawn) {
            count = 0;
            SCR_BND::get_instance().iterate(s, if_bound_exists, then_inc_count, nullptr);
            live_creatures_.update(s, count);
        }
        
        for (auto &n : to_get_neighbors) 
            SCR_BND::get_instance().iterate(n, if_always_true, then_add_to_update_count, nullptr);
        
        for (auto &n : additional_to_update_count) 
            to_update_count.insert(n);
        
        for (auto &ix : to_update_count) {
            count = 0;
            SCR_BND::get_instance().iterate(ix, if_bound_exists, then_inc_count, nullptr);

            if (live_creatures_.exists(ix)) {
                live_creatures_.update(ix, count);
            } else if (count == 3) {
                potential_creatures_.update(ix, count);
            }
        } 
    }
    auto update_render_creatures() -> void {
        for (auto &c : live_creatures_.get_creature_count()) {
            screen_.set_color(c.first, 1 + c.second % 7);
            screen_.set_text(c.first, '0' + c.second);
        }
    }

    auto update() -> bool {
        update_erase_creatures();
        update_execute_rules();
        update_render_creatures();
        return true;
    }


private:

    // Contains all the live creatures for display
    gol_creatures<uint_type> live_creatures_;

    // Contains all spaces with 3 neighbors
    gol_creatures<uint_type> potential_creatures_;
};

#endif