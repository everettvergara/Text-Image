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

// Creatures, Count
using CreaturesCount = std::unordered_map<Creature, Count>;

// Count, Creatures
using CountCreatures = std::array<std::unordered_set<Creature>, 9>;

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
// auto neighbor_count(const Creatures &creatures, const Area &area, const Creature &creature) -> Dim;
// auto execute_rules(Creatures &creatures, const Area &area) -> void;
// auto find_creatures_with_single_or_ge_four(Creatures &creatures, const Area &area) -> Creatures;
// auto find_creatures_with_three(Creatures &creatures, const Area &area) -> Creatures;
// auto kill_creatures(const Creatures &creatures_to_kill, Creatures &creatures) -> void;
// auto spawn_creatures(const Creatures &creatures_to_spawn, Creatures &creatures) -> void;

auto main(int argc, char **argv) -> int {

    const Dim N = 5;
    Area area({140, 30});
    TextImage screen(area);
    auto [creatures_count, count_creatures] = init(area, N);


    // // // Render
    // // for (auto &c : creatures) {
    // //     screen.set_text(c.first, '0' + c.second);
    // //     screen.set_color(c.first, rand() % 8);
    // // }

    // screen.show();
    // do {
    //     TimePointSysClock start {SysClock::now()};

    //     // // Erase previous
    //     // for (auto &c : creatures)
    //     //     screen.set_text(c.first, ' ');

        
    //     //execute_rules(creatures, area);
    //     // // Execute Policies
    //     // Creatures to_kill = find_creatures_with_single_or_ge_four(creatures, area);
    //     // kill_creatures(to_kill, creatures);

    //     // Creatures to_spawn = find_creatures_with_three(creatures, area);
    //     // spawn_creatures(to_spawn, creatures);

    //     // // Render
    //     // for (auto &c : creatures) {
    //     //     screen.set_text(c, 'x');
    //     //     screen.set_color(c, rand() % 8);
    //     // }

    //     screen.show();
    //     TimePointSysClock end = SysClock::now();
    //     int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
    //     if (delay > 0) 
    //         this_thread::sleep_for(chr::milliseconds(delay));

    //     exit(0);
    // } while(creatures.size() > 0);
}

// auto execute_rules(Creatures &creatures, const Area &area) -> void {
    
//     // CreatureList to_kill;
//     // to_kill.reserve(area());    // Can be optimized, because we know how many are creatures with x neigbors
    
//     // for ()
// }

// auto kill_creatures(const Creatures &creatures_to_kill, Creatures &creatures) -> void {
//     for (auto &c : creatures_to_kill)
//         creatures.erase(c);
// }

// auto spawn_creatures(const Creatures &creatures_to_spawn, Creatures &creatures) -> void {
//     for (auto &c : creatures_to_spawn)
//         creatures.insert(c);
// }

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

auto init(const Area &area, const Dim &N) -> CreaturesCountTuple {
    
    CreaturesCount creatures_count;
    CountCreatures count_creatures; 

    creatures_count.reserve(area());
    creatures_count.insert({point_to_index({1, 1}, area), 0});
    creatures_count.insert({point_to_index({2, 2}, area), 0});
    creatures_count.insert({point_to_index({2, 3}, area), 0});
    creatures_count.insert({point_to_index({3, 1}, area), 0});
    creatures_count.insert({point_to_index({3, 2}, area), 0});

    return {creatures_count, count_creatures};


    /*
    std::vector<Dim> creature_ixs;
    
    creature_ixs.reserve(area());
    for (Dim i = 0; i < area(); ++i)
        creature_ixs.push_back(i);

    for (Dim i = 0; i < area(); ++i)
        std::swap(creature_ixs[i], creature_ixs[rand() % area()]);

    Creatures creatures;
    creatures.reserve(N);
    for (Dim i = 0; i < N; ++i)
        creatures.insert(creature_ixs[i]);
    */

    // Creatures creatures;
    // for (Dim i = 0; i < 9; ++i)
    //     creatures[i].reserve(area());

    // creatures[0].insert(point_to_index({1,1}, area));
    // creatures[0].insert(point_to_index({2,2}, area));
    // creatures[0].insert(point_to_index({2,3}, area));
    // creatures[0].insert(point_to_index({3,1}, area));
    // creatures[0].insert(point_to_index({3,2}, area));

    // creatures.insert({point_to_index({1,1}, area), 0});
    // creatures.insert({point_to_index({2,2}, area), 0});
    // creatures.insert({point_to_index({2,3}, area), 0});
    // creatures.insert({point_to_index({3,1}, area), 0});
    // creatures.insert({point_to_index({3,2}, area), 0});

    // for (auto &creature : creatures)
    //     creatures.find(creature.first)->second = 
    //         neighbor_count(creatures, area, creature.first);

    //return creatures;
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
