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

// Includes for kbhit
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
constexpr int FPS = 15;
constexpr int MSPF = 1000 / FPS;

// Game Functions
auto init(const Area &area, const Dim &N) -> CreaturesCountTuple;
auto point_to_index(const Point &point, const Area &area) -> Dim;
auto populate_adjacent_neighbors(const Creature &creature, const Area &area, AdjacentNeighbors &adjacent_neighbors) -> Count;
auto update_creature(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Creature &creature, const Count &count) -> void;
auto neighbor_exists(const CreaturesCount &creatures_count, const Area &area, const Creature &creature) -> bool;
auto neighbor_count(const CreaturesCount &creatures_count, const Area &area, const Creature &creature) -> Dim;
auto execute_rules(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Area &area) -> void;
auto get_blank_list_with_three_neighbors(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Area &area) -> CreaturesList;

// Helpers
auto is_key_pressed() -> int;

auto main(int argc, char **argv) -> int {

    const Dim N = 1000;
    Area area({140, 40});
    TextImage screen(area);
    auto [creatures_count, count_creatures] = init(area, N);

    // Render
    for (auto &cc : creatures_count) {
        screen.set_text(cc.first, '0' + cc.second);
        screen.set_color(cc.first, 1); // + rand() % 7);
    }

    screen.show();

    do {
        TimePointSysClock start {SysClock::now()};

        // Erase
        for (auto &cc : creatures_count)
            screen.set_text(cc.first, ' ');

        execute_rules(creatures_count, count_creatures, area);

        // Render
        for (auto &cc : creatures_count) {
            screen.set_text(cc.first, '0' + cc.second);
            screen.set_color(cc.first, 1 + rand() % 7);
        }

        screen.show();    
        
        TimePointSysClock end = SysClock::now();
        int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
        if (delay > 0) 
            this_thread::sleep_for(chr::milliseconds(delay));

    } while(creatures_count.size() > 0);
}

auto populate_adjacent_neighbors(
    const Creature &creature, 
    const Area &area, 
    AdjacentNeighbors &adjacent_neighbors) -> Count {

    Count neighbor_size = 0;

    Dim top = creature - area.w;
    Dim upper_left = top - 1;
    Dim upper_right = top + 1;
    Dim left = creature - 1;
    Dim right = creature + 1;
    Dim bottom = creature + area.w;
    Dim bottom_left = bottom - 1;
    Dim bottom_right = bottom + 1; 

    if (top >= 0 && top < area()) adjacent_neighbors[neighbor_size++] = top;
    if (upper_left >= 0 && top < area()) adjacent_neighbors[neighbor_size++] = upper_left;
    if (upper_right >= 0 && top < area()) adjacent_neighbors[neighbor_size++] = upper_right;
    if (left >= 0 && top < area()) adjacent_neighbors[neighbor_size++] = left;
    if (right >= 0 && top < area()) adjacent_neighbors[neighbor_size++] = right;
    if (bottom >= 0 && top < area()) adjacent_neighbors[neighbor_size++] = bottom;
    if (bottom_left >= 0 && top < area()) adjacent_neighbors[neighbor_size++] = bottom_left;
    if (bottom_right >= 0 && top < area()) adjacent_neighbors[neighbor_size++] = bottom_right;

    return neighbor_size;
}

auto get_blank_list_with_three_neighbors(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Area &area) -> CreaturesList {

    AdjacentNeighbors neighbors; 
    CreaturesList blank_list_with_three;
    blank_list_with_three.reserve(area());
    for (auto &cc : creatures_count) {
        Dim neighbor_size = populate_adjacent_neighbors(cc.first, area, neighbors);

        // For all neighbors of the current point
        for (Dim i = 0; i < neighbor_size; ++i)

            // Check only if the neigbor is blank
            if (creatures_count.find(neighbors[i]) == creatures_count.end())

                // If it has 3 neigbors then insert to blank_list_with_three
                if (blank_list_with_three.find(neighbors[i]) == blank_list_with_three.end())
                    if (neighbor_count(creatures_count, area, neighbors[i]) == 3)
                        blank_list_with_three.insert(neighbors[i]);
    }

    return blank_list_with_three;
}

auto execute_rules(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Area &area) -> void {

    AdjacentNeighbors neighbors; 

    CreaturesList to_update;
    to_update.reserve(area());

    CreaturesList blank_list_with_three = get_blank_list_with_three_neighbors(creatures_count, count_creatures, area);

    // Kill all creatures with <= 1 neighbor
    for (Dim i = 0; i <= 1; ++i) {
        for (auto &cc : count_creatures[i]) {
            creatures_count.erase(cc);
            Dim neighbor_size = populate_adjacent_neighbors(cc, area, neighbors);
            for (Dim j = 0; j < neighbor_size; ++j) 
                to_update.insert(neighbors[j]);
        }
        count_creatures[i].clear();
    }
    
    // Kill all creatures with >= 4 neighbor
    for (Dim i = 4; i <= 8; ++i) {
        for (auto &cc : count_creatures[i]) {
            creatures_count.erase(cc);
            Dim neighbor_size = populate_adjacent_neighbors(cc, area, neighbors);
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

        Dim neighbor_size = populate_adjacent_neighbors(cc, area, neighbors);
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
                neighbor_count(creatures_count, area, cc));

}


auto neighbor_exists(const CreaturesCount &creatures_count, const Area &area, const Creature &creature) -> bool {
    
    Dim top = creature - area.w;
    
    if (creatures_count.find(top) != creatures_count.end()) return true;
    
    Dim upper_left = top - 1;
    if (creatures_count.find(upper_left) != creatures_count.end()) return true;
    
    Dim upper_right = top + 1;
    if (creatures_count.find(upper_right) != creatures_count.end()) return true;
    
    Dim left = creature - 1;
    if (creatures_count.find(left) != creatures_count.end()) return true;
    
    Dim right = creature + 1;
    if (creatures_count.find(right) != creatures_count.end()) return true;
    
    Dim bottom = creature + area.w;
    if (creatures_count.find(bottom) != creatures_count.end()) return true;
    
    Dim bottom_left = bottom - 1;
    if (creatures_count.find(bottom_left) != creatures_count.end()) return true;
    
    Dim bottom_right = bottom + 1;    
    if (creatures_count.find(bottom_right) != creatures_count.end()) return true;

    return false;
}

auto neighbor_count(const CreaturesCount &creatures_count, const Area &area, const Creature &creature) -> Dim {
    
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

auto point_to_index(const Point &point, const Area &area) -> Dim {
    return point.y * area.w + point.x;
} 

auto update_creature(
    CreaturesCount &creatures_count, 
    CountCreatures &count_creatures, 
    const Creature &creature, 
    const Count &count) -> void {
    
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

auto init(const Area &area, const Dim &N) -> CreaturesCountTuple {
    
    CreaturesCount creatures_count;
    CountCreatures count_creatures; 

    creatures_count.reserve(area());
    for (Dim i = 0; i < 9; ++i) 
        count_creatures[i].reserve(area());

    // Generate and shuffle all creatures
    // then get the first N Creatures
    std::vector<Creature> all_creatures;
    all_creatures.reserve(area());
    for (Dim i = 0; i < area(); ++i)
        all_creatures[i] = i;

    for (Dim i = 0; i < area(); ++i) 
        std::swap(all_creatures[i], all_creatures[rand() % area()]);

    // Create the initial creatures
    for (Dim i = 0; i < N; ++i)
        update_creature(creatures_count, count_creatures, all_creatures[i], 0);

    for (auto &cc : creatures_count)
        update_creature(
            creatures_count, 
            count_creatures, 
            cc.first, 
            neighbor_count(creatures_count, area, cc.first));

/*
    creatures_count.reserve(area());
    for (Dim i = 0; i < 9; ++i) 
        count_creatures[i].reserve(area());

    update_creature(creatures_count, count_creatures, point_to_index({1,1}, area), 0);
    update_creature(creatures_count, count_creatures, point_to_index({2,2}, area), 0);
    update_creature(creatures_count, count_creatures, point_to_index({2,3}, area), 0);
    update_creature(creatures_count, count_creatures, point_to_index({3,1}, area), 0);
    update_creature(creatures_count, count_creatures, point_to_index({3,2}, area), 0);

    for (auto &cc : creatures_count)
        update_creature(
            creatures_count, 
            count_creatures, 
            cc.first, 
            neighbor_count(creatures_count, area, cc.first));
*/

    // update_creature(creatures_count, count_creatures, point_to_index({1,1}, area), 0);
    // update_creature(creatures_count, count_creatures, point_to_index({2,1}, area), 0);
    // update_creature(creatures_count, count_creatures, point_to_index({1,2}, area), 0);

    // for (auto &cc : creatures_count)
    //     update_creature(
    //         creatures_count, 
    //         count_creatures, 
    //         cc.first, 
    //         neighbor_count(creatures_count, area, cc.first));

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
