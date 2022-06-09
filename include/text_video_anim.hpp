#ifndef TEXT_VIDEO_ANIM_HPP
#define TEXT_VIDEO_ANIM_HPP

#include <chrono>
#include <thread>
#include "text_image.hpp"

// Includes for is_key_pressed
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
    
    /**
     * Constructors, Assignments and 
     * Destructors 
     * 
     */

    public:

        text_video_anim(const uint_type w, const uint_type h, const color c = 7, const text t = ' ', const mask_bit m = ON) :
            timg_(w, h, c, t, m) {}

        virtual ~text_video_anim() = default;

    /**
     * Getters
     * 
     */
    
    public:

        inline auto data() -> text_image<int_type, uint_type> & {
            return timg_;
        }

        inline auto cdata() const -> const text_image<int_type, uint_type> & {
            return timg_;
        }

    /**
     * User overridable functions 
     * 
     */

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
            
            do {
                time_point<system_clock> start {system_clock::now()};
                
                timg_.show();

                if (event()) {
                    update(); 
                    delayer(start);
                }
            } while(is_running_);

            return true;
        }



    protected:

    // TODO: consoder using literal suffix for width and height
    // i.e. 10d d -> double the width for conversion factor
        uint_type FPS_, MSPF_;
        bool is_running_{false};
        text_image<int_type, uint_type> timg_;
        
    };
}

#endif 