#include <iostream>
#include <vector>
#include <unordered_set>
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
using Creatures = std::unordered_set<Dim>;

constexpr int FPS = 15;
constexpr int MSPF = 1000 / FPS;
namespace chr = std::chrono;
namespace this_thread = std::this_thread;
using TimePointSysClock = chr::time_point<chr::system_clock>;
using SysClock = chr::system_clock;

auto is_key_pressed() -> int;
auto spawner(const Area &area, const Dim &N) -> Creatures;
auto kill_creatures_with_single_neighbors(Creatures &creatures, const Area &area) -> void;

auto main(int argc, char **argv) -> int {

    const Dim N = 1000;
    Area area({140, 50});
    TextImage screen(area);
    Creatures creatures = spawner(area, N);
    
    do {
        TimePointSysClock start {SysClock::now()};

        for (auto &c : creatures) {
            
            // Erase Previous
            screen.set_text(c, ' ');

            // Execute Policies
            kill_creatures_with_single_neighbors(creatures, area);
        }

        // Render
        for (auto &c : creatures) {
            screen.set_text(c, 'x');
            screen.set_color(c, rand() % 8);
        }

        screen.show();
        TimePointSysClock end = SysClock::now();
        int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
        if (delay > 0) 
            this_thread::sleep_for(chr::milliseconds(delay));

    } while(!is_key_pressed() && creatures.size() > 0);
}


auto kill_creatures_with_single_neighbors(Creatures &creatures, const Area &area) -> void {


    std::cout << creatures.size() << " before....\n";
    for (auto &c : creatures) {
        Dim neighbor = 0;

        // Check upper left exists
        Dim top = c - area.w;
        Dim upper_left = top - 1;
        Dim upper_right = top + 1;
        Dim left = c - 1;
        Dim right = c + 1;
        Dim bottom = c + area.w;
        Dim bottom_left = bottom - 1;
        Dim bottom_right = bottom + 1;        
        
        if (creatures.find(upper_left) != creatures.end()) ++neighbor;
        if (creatures.find(top) != creatures.end()) ++neighbor;

        if (neighbor >= 2) continue;
        if (creatures.find(upper_right) != creatures.end()) ++neighbor;
        
        if (neighbor >= 2) continue;
        if (creatures.find(left) != creatures.end()) ++neighbor;
        
        if (neighbor >= 2) continue;
        if (creatures.find(right) != creatures.end()) ++neighbor;
        
        if (neighbor >= 2) continue;
        if (creatures.find(bottom_left) != creatures.end()) ++neighbor;
        
        if (neighbor >= 2) continue;
        if (creatures.find(bottom) != creatures.end()) ++neighbor;
        
        if (neighbor >= 2) continue;
        if (creatures.find(bottom_right) != creatures.end()) ++neighbor;

        if (neighbor >= 2) continue;
        std::cout << "erasing c: " << c << ", erased count: " << creatures.erase(c) << " size: " << creatures.size() << "\n";
    }

    std::cout << creatures.size() << " xxxxx....\n"; exit(0);
}

auto spawner(const Area &area, const Dim &N) -> Creatures {
    
    std::vector<Dim> creature_ixs;
    
    for (Dim i = 0; i < area(); ++i)
        creature_ixs.push_back(i);

    for (Dim i = 0; i < area(); ++i)
        std::swap(creature_ixs[i], creature_ixs[rand() % area()]);

    Creatures creatures;
    for (Dim i = 0; i < N; ++i)
        creatures.insert(creature_ixs[i]);

    return creatures;
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
