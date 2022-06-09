#ifndef TEXT_VIDEO_HPP
#define TEXT_VIDEO_HPP

#include "text_image.hpp"

// Includes for is_key_pressed
#include <cstdio>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>

namespace g80 {

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

    template<typename int_type, typename uint_type>
    class text_video {
    public:
        text_video() = default;
        virtual ~text_video() = default;

        inline auto data() -> text_image<int_type, uint_type> & {
            return timg_;
        }

        inline auto cdata() const -> const text_image<int_type, uint_type> & {
            return timg_;
        }

        virtual auto preprocess() -> bool {return true;}
        virtual auto update() -> bool {return true;}

        // delayer()

    private:
        uint_type w_, h_, FPS_, MSPF_;
        bool is_running_{false};
        text_image<int_type, uint_type> timg_;
        
    };
}

#endif 