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

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <array>
#include <tuple>
#include <bitset>
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

// todo: generalize ixs
using creature_ids = std::array<uint_type, SCREEN_SIZE>;
using creature_id_exists = std::bitset<SCREEN_SIZE>;
using creature_ids_ix = std::array<int_type, 9>;
using creature_ids_neigh_count = std::array<uint_type, SCREEN_SIZE>;
using neigh_counts_creature_ids = std::array<std::array<uint_type, SCREEN_SIZE>, 9>;
using neigh_counts_ix = std::array<int_type, 9>;


// // Adj neighbors? could be merged with creatures count?
// using adjacent_neighbors = std::array<uint_type, 8>;

// // creatures_count, count_creatures
// using creatures_count_tuple = std::tuple<creatures_count, count_creatures>;



class gol : public text_video_anim<int_type, uint_type> {
public:
    
    gol() : text_video_anim<int_type, uint_type>(SCREEN_WIDTH, SCREEN_HEIGHT, FPS) {}
    
    ~gol() = default;


private: 

    auto preprocess_random_creatures() -> std::vector<uint_type> {
        std::vector<uint_type> all_creature_ids;
        all_creature_ids.reserve(screen_.size());
        for (uint_type i = 0; i < screen_.size(); ++i) all_creature_ids[i] = i;
        for (uint_type i = 0; i < screen_.size(); ++i) std::swap(all_creature_ids[i], all_creature_ids[rand() % screen_.size()]);
        return all_creature_ids;
    }

    auto neighbor_count(const uint_type creature_id) -> uint_type {
        
        uint_type neighbor = 0;

        uint_type top = creature_id - screen_.width();
        uint_type upper_left = top - 1;
        uint_type upper_right = top + 1;
        uint_type left = creature_id - 1;
        uint_type right = creature_id + 1;
        uint_type bottom = creature_id + screen_.width();
        uint_type bottom_left = bottom - 1;
        uint_type bottom_right = bottom + 1;    

        if (creature_id_exists_[top]) ++neighbor;
        if (creature_id_exists_[upper_left]) ++neighbor;
        if (creature_id_exists_[upper_right]) ++neighbor;
        if (creature_id_exists_[left]) ++neighbor;
        if (creature_id_exists_[right]) ++neighbor;
        if (creature_id_exists_[bottom]) ++neighbor;
        if (creature_id_exists_[bottom_left]) ++neighbor;
        if (creature_id_exists_[bottom_right]) ++neighbor;

        return neighbor;
    }


    auto update_creature(uint_type creature_id) -> void {

        // creature_ids_neigh_count_ = neighbor_c
        // if (!creature_id_exists_[creature_id]) {
        //     creature_id_exists_[creature_id] = true;
        // } else {

        // }
        // auto f = creature_id_neigh_count_.find(creature_id);
        // if (f == creature_id_neigh_count_.end()) {
        //     creature_id_neigh_count_.insert(creature_id, neigh_count);
        // } else {
        //     auto prev_neigh_count = f->second;
        //     f->second = neigh_count;


        // }
    }

public:

    auto preprocess() -> bool {

        creature_id_exists_.reset();
        memset(neigh_counts_ix.data(), ~0, sizeof(int_type) * 9);
        memset(neigh_counts_ix.data(), ~0, sizeof(int_type) * 9);
        
        std::vector<uint_type> all_shuffled_creatureids = preprocess_random_creatures();
        for (uint_type i = 0; i < N_; ++i) {
            creature_id_exists_[all_shuffled_creatureids[i]] = true;
        }
    }

    auto update() -> bool {


        return true;
    }

private:
    uint_type N_;
    creature_ids creature_ids_;
    creature_ids_ix creature_ids_ix_;
    creature_id_exists creature_id_exists_;
    creature_ids_neigh_count creature_ids_neigh_count_;
    neigh_counts_creature_ids neigh_counts_creature_ids_;
    neigh_counts_ix neigh_counts_ix;


};

#endif