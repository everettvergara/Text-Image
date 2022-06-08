/*
 *  Game of Life using TextImage Class
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

// Game of Life Dastruct
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <tuple>
#include <algorithm>

// Includes for delayer
#include <chrono>
#include <thread>

// Includes for is_key_pressed
#include <cstdio>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <cstdlib>

// Graphics Console
#include "TextImage.h"

// Shortcuts
using namespace g80;
using Creature = Dim;
using Count = Dim;
using CreaturesList = std::unordered_set<Creature>;
using CreaturesCount = std::unordered_map<Creature, Count>;
using CountCreatures = std::array<CreaturesList, 9>;
using AdjacentNeighbors = std::array<Creature, 8>;
using CreaturesCountTuple = std::tuple<CreaturesCount, CountCreatures>;
namespace chr = std::chrono;
namespace this_thread = std::this_thread;
using TimePointSysClock = chr::time_point<chr::system_clock>;
using SysClock = chr::system_clock;

// Game configurations 
constexpr Dim FPS = 15;
constexpr Dim MSPF = 1000 / FPS;
constexpr Dim N = 3500;
constexpr Dim SCREEN_WIDTH = 140;
constexpr Dim SCREEN_HEIGHT = 40;
const Area area(SCREEN_WIDTH, SCREEN_HEIGHT);

// Game Routine Functions
auto init() -> CreaturesCountTuple;
auto render_creatures(TextImage &screen, const CreaturesCount &creatures_count) -> void;
auto erase_creatures(TextImage &screen, const CreaturesCount &creatures_count) -> void;
auto delayer(TimePointSysClock start) -> void;
auto is_key_pressed() -> int;

// Game of Life Functions
auto point_to_index(const Point &point) -> Dim;
auto populate_adjacent_neighbors(const Creature &creature, AdjacentNeighbors &adjacent_neighbors) -> Count;
auto update_creature(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Creature &creature, const Count &count) -> void;
auto neighbor_count(const CreaturesCount &creatures_count, const Creature &creature) -> Dim;
auto execute_rules(CreaturesCount &creatures_count, CountCreatures &count_creatures) -> void;
auto get_blank_list_with_three_neighbors(CreaturesCount &creatures_count, CountCreatures &count_creatures) -> CreaturesList;

auto main(int argc, char **argv) -> int {
    
    TextImage screen(area);
    TextImage title("---===| Game of Life |===---", 3, ON);

    screen.fill_text(' ');
    auto [creatures_count, count_creatures] = init(); 

    do {

        TimePointSysClock start_delay_timer {SysClock::now()};
        
        erase_creatures(screen, creatures_count);
        execute_rules(creatures_count, count_creatures);
        render_creatures(screen, creatures_count);

        screen.or_image(title, {static_cast<Dim>(SCREEN_WIDTH / 2 - title.area().w / 2), 0});
        screen.show();

        delayer(start_delay_timer);

    } while(!is_key_pressed() &&
            creatures_count.size() > 0);
}

auto render_creatures(TextImage &screen, const CreaturesCount &creatures_count) -> void {
    for (auto &cc : creatures_count) {
        screen.set_text(cc.first, '0' + cc.second);
        screen.set_color(cc.first, 1 + rand() % 7);
    }
}

auto erase_creatures(TextImage &screen, const CreaturesCount &creatures_count) -> void {
    for (auto &cc : creatures_count)
        screen.set_text(cc.first, ' '); 
}

auto delayer(TimePointSysClock start) -> void {
    TimePointSysClock end = SysClock::now();
    Dim delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
    if (delay > 0) 
        this_thread::sleep_for(chr::milliseconds(delay));
}

auto populate_adjacent_neighbors(const Creature &creature, AdjacentNeighbors &adjacent_neighbors) -> Count {

    Count neighbor_size = 0;
    auto add_neighbor_if_within_bounds = [&](const Dim &neighbor_to_check) -> bool {
        if (neighbor_to_check >= 0 && neighbor_to_check <= area()) {
            adjacent_neighbors[neighbor_size++] = neighbor_to_check;
            return true;
        }
        return false;
    };

    Dim top = creature - area.w;
    Dim upper_left = top - 1;
    Dim upper_right = top + 1;
    Dim left = creature - 1;
    Dim right = creature + 1;
    Dim bottom = creature + area.w;
    Dim bottom_left = bottom - 1;
    Dim bottom_right = bottom + 1; 

    add_neighbor_if_within_bounds(top);
    add_neighbor_if_within_bounds(upper_left);
    add_neighbor_if_within_bounds(upper_right);
    add_neighbor_if_within_bounds(left);
    add_neighbor_if_within_bounds(right);
    add_neighbor_if_within_bounds(bottom);
    add_neighbor_if_within_bounds(bottom_left);
    add_neighbor_if_within_bounds(bottom_right);
    
    return neighbor_size;
}

auto get_blank_list_with_three_neighbors(CreaturesCount &creatures_count, CountCreatures &count_creatures) -> CreaturesList {

    AdjacentNeighbors neighbors; 
    CreaturesList blank_list_with_three;
    blank_list_with_three.reserve(area());
    for (auto &cc : creatures_count) {
        Dim neighbor_size = populate_adjacent_neighbors(cc.first, neighbors);

        // For all neighbors of the current point
        for (Dim i = 0; i < neighbor_size; ++i)

            // Check only if the neigbor is blank
            if (creatures_count.find(neighbors[i]) == creatures_count.end())

                // If it has 3 neigbors then insert to blank_list_with_three
                if (blank_list_with_three.find(neighbors[i]) == blank_list_with_three.end())
                    if (neighbor_count(creatures_count, neighbors[i]) == 3)
                        blank_list_with_three.insert(neighbors[i]);
    }

    return blank_list_with_three;
}

auto execute_rules(CreaturesCount &creatures_count, CountCreatures &count_creatures) -> void {

    AdjacentNeighbors neighbors; 

    CreaturesList to_update;
    to_update.reserve(area());

    CreaturesList blank_list_with_three = get_blank_list_with_three_neighbors(creatures_count, count_creatures);

    // Kill all creatures with <= 1 neighbor
    for (Dim i = 0; i <= 1; ++i) {
        for (auto &cc : count_creatures[i]) {
            creatures_count.erase(cc);
            Dim neighbor_size = populate_adjacent_neighbors(cc, neighbors);
            for (Dim j = 0; j < neighbor_size; ++j) 
                to_update.insert(neighbors[j]);
        }
        count_creatures[i].clear();
    }
    
    // Kill all creatures with >= 4 neighbor
    for (Dim i = 4; i <= 8; ++i) {
        for (auto &cc : count_creatures[i]) {
            creatures_count.erase(cc);
            Dim neighbor_size = populate_adjacent_neighbors(cc, neighbors);
            for (Dim j = 0; j < neighbor_size; ++j) 
                to_update.insert(neighbors[j]);
        }
        count_creatures[i].clear();
    }
    
    // Spawn Blank Spaces with three neighbors
    for (auto &cc : blank_list_with_three) {
        update_creature(
            creatures_count, 
            count_creatures, 
            cc,
            0);
        to_update.insert(cc);

        Dim neighbor_size = populate_adjacent_neighbors(cc, neighbors);
        for (Dim j = 0; j < neighbor_size; ++j) 
            to_update.insert(neighbors[j]);
    }
    
    // Update neighbors of killed creatures
    for (auto &cc : to_update)
        if (creatures_count.find(cc) != creatures_count.end())
            update_creature(
                creatures_count, 
                count_creatures, 
                cc,
                neighbor_count(creatures_count, cc));

}

auto neighbor_count(const CreaturesCount &creatures_count, const Creature &creature) -> Dim {
    
    Dim neighbor = 0;

    Dim top = creature - area.w;
    Dim upper_left = top - 1;
    Dim upper_right = top + 1;
    Dim left = creature - 1;
    Dim right = creature + 1;
    Dim bottom = creature + area.w;
    Dim bottom_left = bottom - 1;
    Dim bottom_right = bottom + 1;    

    if (creatures_count.find(top) != creatures_count.end()) ++neighbor;
    if (creatures_count.find(upper_left) != creatures_count.end()) ++neighbor;
    if (creatures_count.find(upper_right) != creatures_count.end()) ++neighbor;
    if (creatures_count.find(left) != creatures_count.end()) ++neighbor;
    if (creatures_count.find(right) != creatures_count.end()) ++neighbor;
    if (creatures_count.find(bottom) != creatures_count.end()) ++neighbor;
    if (creatures_count.find(bottom_left) != creatures_count.end()) ++neighbor;
    if (creatures_count.find(bottom_right) != creatures_count.end()) ++neighbor;

    return neighbor;
}

auto point_to_index(const Point &point) -> Dim {
    return point.y * area.w + point.x;
} 

auto update_creature(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Creature &creature, const Count &count) -> void {
    
    // Update Creatures Count
    auto f = creatures_count.find(creature);
    if (f == creatures_count.end()) {
        creatures_count.insert({creature, count});
    } else {
        Count prev_count;
        prev_count = f->second;
        f->second = count;
        count_creatures[prev_count].erase(creature);
    }

    count_creatures[count].insert(creature);
}

auto init() -> CreaturesCountTuple {
    
    CreaturesCount creatures_count;
    creatures_count.reserve(area()); 
    
    CountCreatures count_creatures; 
    for (Dim i = 0; i < 9; ++i) 
        count_creatures[i].reserve(area());

    // Generate and shuffle all creatures
    // then get only first N Creatures
    std::vector<Creature> all_creatures;
    all_creatures.reserve(area());
    for (Dim i = 0; i < area(); ++i)
        all_creatures[i] = i;

    for (Dim i = 0; i < area(); ++i) 
        std::swap(all_creatures[i], all_creatures[rand() % area()]);

    // Initial creatures
    for (Dim i = 0; i < N; ++i)
        update_creature(creatures_count, count_creatures, all_creatures[i], 0);

    for (auto &cc : creatures_count)
        update_creature(
            creatures_count, 
            count_creatures, 
            cc.first, 
            neighbor_count(creatures_count, cc.first));

    return {creatures_count, count_creatures};
}

auto is_key_pressed() -> int {
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized) {
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting); 
    return bytesWaiting;
}
