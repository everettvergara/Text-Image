/**
 * @file gol_demo.hpp
 * @author Everett Gaius S. Vergara (me@everettgaius.com)
 * @brief Starter program for Connor's Game of Life 
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
#include <iostream>
#include "gol.hpp"

auto main(const int argc, const char *argv[]) -> int {


    gol game_of_life; 
    game_of_life.preprocess();
    game_of_life.run();
}

