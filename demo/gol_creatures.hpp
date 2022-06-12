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

#ifndef GOL_CREATURES_HPP
#define GOL_CREATURES_HPP

#include <unordered_map>
#include <unordered_set>
#include <array>
#include "gol_common.hpp"

template<typename uint_type>
class gol_creatures {
public:

    gol_creatures(const uint_type w, const uint_type h) : w_(w), h_(h), size_(w * h) {}

    auto exists(const uint_type ix) -> bool {
        auto f = creature_count_.find(ix);
        return f != creature_count_.end();
    }

    auto get_existing_neighbor_count(const uint_type ix) -> uint_type {
        uint_type count {0};
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
            grouped_creatures_[pcount].erase(ix);
            f->second = count;
        }
        grouped_creatures_[count].insert(ix);
    }

    auto kill_group(uint_type group) {
        for (auto ix : grouped_creatures_[group]) creature_count_.erase(ix);
        grouped_creatures_[group].clear();
    }

    inline auto get_creature_count() -> std::unordered_map<uint_type, uint_type> & {
        return creature_count_;
    }

    inline auto get_grouped_creatures(uint_type group) -> std::unordered_set<uint_type> & {
        return grouped_creatures_[group];
    }

private:

    uint_type w_, h_, size_;
    std::unordered_map<uint_type, uint_type> creature_count_;
    std::array<std::unordered_set<uint_type>, 9> grouped_creatures_;
};


#endif