#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdio>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <cstdlib>

#include "TextImage.h"


// Game of Life Automata
using namespace g80;
using Creature = Dim;
using Count = Dim;

using CreaturesList = std::unordered_set<Creature>;

// Creatures, Count
using CreaturesCount = std::unordered_map<Creature, Count>;

// Count, Creatures
using CountCreatures = std::array<CreaturesList, 9>;

// Creatures and Count
using CreaturesCountTuple = std::tuple<CreaturesCount, CountCreatures>;


constexpr int FPS = 5;
constexpr int MSPF = 1000 / FPS;
namespace chr = std::chrono;
namespace this_thread = std::this_thread;
using TimePointSysClock = chr::time_point<chr::system_clock>;
using SysClock = chr::system_clock;

auto is_key_pressed() -> int;
auto init(const Area &area, const Dim &N) -> CreaturesCountTuple;


auto point_to_index(const Point &point, const Area &area) -> Dim;
auto update_creature(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Creature &creature, const Count &count) -> void;
auto neighbor_exists(const CreaturesCount &creatures_count, const Area &area, const Creature &creature) -> bool;
auto neighbor_count(const CreaturesCount &creatures_count, const Area &area, const Creature &creature) -> Dim;
auto execute_rules(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Area &area) -> void;
// auto find_creatures_with_single_or_ge_four(Creatures &creatures, const Area &area) -> Creatures;
// auto find_creatures_with_three(Creatures &creatures, const Area &area) -> Creatures;
// auto kill_creatures(const Creatures &creatures_to_kill, Creatures &creatures) -> void;
// auto spawn_creatures(const Creatures &creatures_to_spawn, Creatures &creatures) -> void;

auto main(int argc, char **argv) -> int {

    const Dim N = 5;
    Area area({140, 30});
    TextImage screen(area);
    auto [creatures_count, count_creatures] = init(area, N);

    // Render
    for (auto &cc : creatures_count) {
        screen.set_text(cc.first, '0' + cc.second);
        screen.set_color(cc.first, rand() % 8);
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
            screen.set_color(cc.first, rand() % 8);
        }


        screen.show();

                
        std::cout << "\n\n\n----" << creatures_count.size() << "\n";
        exit(0);

        TimePointSysClock end = SysClock::now();
        int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
        if (delay > 0) 
            this_thread::sleep_for(chr::milliseconds(delay));

        exit(0);
    } while(creatures_count.size() > 0);
}

auto execute_rules(CreaturesCount &creatures_count, CountCreatures &count_creatures, const Area &area) -> void {

    CreaturesList to_update;

    // can be made efficient by making static?
    to_update.reserve(area());  

    Dim neighbor_size;
    std::array<Creature, 8> neighbors; 
    auto populate_neighbors = [&](const Creature &creature) {
        neighbor_size = 0;
        Dim top = creature - area.w;
        Dim upper_left = top - 1;
        Dim upper_right = top + 1;
        Dim left = creature - 1;
        Dim right = creature + 1;
        Dim bottom = creature + area.w;
        Dim bottom_left = bottom - 1;
        Dim bottom_right = bottom + 1; 

        if (top >= 0 && top < area()) neighbors[neighbor_size++] = top;
        if (upper_left >= 0 && top < area()) neighbors[neighbor_size++] = upper_left;
        if (upper_right >= 0 && top < area()) neighbors[neighbor_size++] = upper_right;
        if (left >= 0 && top < area()) neighbors[neighbor_size++] = left;
        if (right >= 0 && top < area()) neighbors[neighbor_size++] = right;
        if (bottom >= 0 && top < area()) neighbors[neighbor_size++] = bottom;
        if (bottom_left >= 0 && top < area()) neighbors[neighbor_size++] = bottom_left;
        if (bottom_right >= 0 && top < area()) neighbors[neighbor_size++] = bottom_right;
    };

    // Kill all creatures with <= 1 neighbor
    for (Dim i = 0; i <= 1; ++i) {
        for (auto &cc : count_creatures[i]) {
            creatures_count.erase(cc);
            populate_neighbors(cc);
            for (Dim j = 0; j < neighbor_size; ++j) 
                to_update.insert(neighbors[j]);
        }
        count_creatures[i].clear();
    }
    
    // Kill all creatures with >= 4 neighbor
    for (Dim i = 4; i <= 8; ++i) {
        for (auto &cc : count_creatures[i]) {
            creatures_count.erase(cc);
            populate_neighbors(cc);
            for (Dim j = 0; j < neighbor_size; ++j) 
                to_update.insert(neighbors[j]);
        }
        count_creatures[i].clear();
    }
    
    // Spawn
}


auto neighbor_exists(const CreaturesCount &creatures_count, const Area &area, const Creature &creature) -> bool {
    Dim neighbor = 0;

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


// auto find_creatures_with_single_or_ge_four(Creatures &creatures, const Area &area) -> Creatures {

//     Creatures to_kill;
//     to_kill.reserve(area());    // N can be optimized
//     for (auto &creature : creatures) {
//         Dim neighbors = neighbor_count(creatures, area, creature);
//         if (neighbors <= 1 || neighbors >= 4) 
//             to_kill.insert(creature);
//     }
//     return to_kill;
// }

// // Blank cell with two three neigbors survives
// auto find_creatures_with_three(Creatures &creatures, const Area &area) -> Creatures {

//     Creatures to_test;
    
//     to_test.reserve(area());    // N can be optimized;
//     for (auto &creature : creatures) {
//         Dim top = creature - area.w;
//         Dim upper_left = top - 1;
//         Dim upper_right = top + 1;
//         Dim left = creature - 1;
//         Dim right = creature + 1;
//         Dim bottom = creature + area.w;
//         Dim bottom_left = bottom - 1;
//         Dim bottom_right = bottom + 1;    

//         if (top >= 0 && creatures.find(top) == creatures.end()) to_test.insert(top);
//         if (upper_left >= 0 && creatures.find(upper_left) == creatures.end()) to_test.insert(upper_left);
//         if (upper_right >= 0 && creatures.find(upper_right) == creatures.end()) to_test.insert(upper_right);
//         if (left >= 0 && creatures.find(left) == creatures.end()) to_test.insert(left);
//         if (right < area() && creatures.find(right) == creatures.end()) to_test.insert(right);
//         if (bottom < area() && creatures.find(bottom) == creatures.end()) to_test.insert(bottom);
//         if (bottom_left < area() && creatures.find(bottom_left) == creatures.end()) to_test.insert(bottom_left);
//         if (bottom_right < area() && creatures.find(bottom_right) == creatures.end()) to_test.insert(bottom_right);
//     }

//     Creatures to_spawn;
//     to_spawn.reserve(area());   // N can be optimized;
//     for (auto &creature : to_test) {
//         Dim neighbors = neighbor_count(creatures, area, creature);
//         if (neighbors == 3) 
//             to_spawn.insert(creature);
//     }

//     return to_spawn;
// }

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
