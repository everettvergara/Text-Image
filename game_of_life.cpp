#include <iostream>
#include <vector>
#include <algorithm>
#include "TextImage.h"


// Game of Life Automata
using namespace g80;
using Creatures = std::vector<Point>;


auto spawner(const Area &area, const Dim &N) -> Creatures;

auto main(int argc, char **argv) -> int {

    const Dim N = 50;
    Area area({140, 50});
    TextImage screen(area);
    Creatures creatures = spawner(area, N);

    // Plot Creatures to Screen
    screen.fill_text('.');
    screen.fill_color(7);
    for (auto &c : creatures) {
        screen.set_text(c, 'x');
        screen.set_color(c, 1);
    }

    screen.show();
    

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