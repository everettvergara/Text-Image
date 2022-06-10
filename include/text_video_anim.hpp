/**
 * @file text_video_anim.hpp
 * @author Everett Gaius S. Vergara (me@everettgaius.com)
 * @brief A simple C++17 class to handle console animation using the text_image class.
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

#ifndef TEXT_VIDEO_ANIM_HPP
#define TEXT_VIDEO_ANIM_HPP

#include <chrono>
#include <thread>
#include "text_image.hpp"

#include <cstdio>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>

namespace g80 {

    using namespace std::chrono;
            
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

        int bytes_waiting;
        ioctl(STDIN, FIONREAD, &bytes_waiting); 
        return bytes_waiting;
    }

    template<typename int_type, typename uint_type>
    class text_video_anim {
    
    // Constructors and destructors

    public:

        text_video_anim(const uint_type w, const uint_type h, const uint_type fps, const color c = 7, const text t = ' ', const mask_bit m = ON) :
            screen_(w, h, c, t, m),
            MSPF_(1000 / validator_if_less_than<uint_type, 1>(fps)) {}

        virtual ~text_video_anim() = default;

    // Getters

    public:

        inline auto data() -> text_image<int_type, uint_type> & {
            return screen_;
        }

        inline auto cdata() const -> const text_image<int_type, uint_type> & {
            return screen_;
        }

    // Overridable functions

    protected:

        virtual auto update() -> bool {return true;}
        
        virtual auto event() -> bool {
            if (is_key_pressed()) {
                is_running_ = false;
            }
            return is_running_;
        }

    private:

        auto delayer(time_point<system_clock> &start) -> void {
            time_point<system_clock> end {system_clock::now()};
            uint_type delay = MSPF_ - static_cast<uint_type>(duration_cast<milliseconds>(end - start).count());
            if (delay > 0) std::this_thread::sleep_for(milliseconds(delay));
        }

    public:

        virtual auto preprocess() -> bool {return true;}
        
        virtual auto run() -> bool {
            is_running_ = true;
            do {
                time_point<system_clock> start {system_clock::now()};
                screen_.show();
                if (event()) {update(); delayer(start);}
            } while(is_running_);

            return true;
        }



    protected:
        // TODO: consoder using literal suffix for width and height
        // i.e. 10d d -> double the width for conversion factor
        text_image<int_type, uint_type> screen_;
        uint_type MSPF_;
        bool is_running_{false};
        
    };
}

#endif 