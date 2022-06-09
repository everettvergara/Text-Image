#ifndef TEXT_VIDEO_HPP
#define TEXT_VIDEO_HPP

#include "text_image.hpp"

namespace g80 {

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

    private:
        
        bool is_running_{false};
        text_image<int_type, uint_type> timg_;

    };
}

#endif 