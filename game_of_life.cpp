#include <iostream>
#include <vector>
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
using Creatures = std::vector<Point>;

constexpr int FPS = 15;
constexpr int MSPF = 1000 / FPS;
namespace chr = std::chrono;
namespace this_thread = std::this_thread;
using TimePointSysClock = chr::time_point<chr::system_clock>;
using SysClock = chr::system_clock;

auto is_key_pressed() -> int;
auto spawner(const Area &area, const Dim &N) -> Creatures;

auto main(int argc, char **argv) -> int {

    const Dim N = 1000;
    Area area({140, 50});
    TextImage screen(area);
    Creatures creatures = spawner(area, N);

    // Plot Creatures to Screen


    screen.show();
    
     do {
        TimePointSysClock start {SysClock::now()};

        screen.fill_text(' ');
        for (auto &c : creatures) {
            screen.set_text(c, 'x');
            screen.set_color(c, 1);
        }

        TimePointSysClock end = SysClock::now();
        int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
        if (delay > 0) 
            this_thread::sleep_for(chr::milliseconds(delay));

    } while(!is_key_pressed());

}


auto spawner(const Area &area, const Dim &N) -> Creatures {
    
    std::vector<Dim> creature_ixs;
    
    for (Dim i = 0; i < area(); ++i)
        creature_ixs.push_back(i);

    for (Dim i = 0; i < area(); ++i)
        std::swap(creature_ixs[i], creature_ixs[rand() % area()]);

    Creatures creatures;
    for (Dim i = 0; i < N; ++i) {
        std::cout << "x: " << static_cast<Dim>(creature_ixs[i] % area.w) << " y: " << static_cast<Dim>(creature_ixs[i] / area.w) << "\n";
        creatures.emplace_back(
            Point{
                static_cast<Dim>(creature_ixs[i] % area.w), 
                static_cast<Dim>(creature_ixs[i] / area.w)});
    }
    
    //exit(0);
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
