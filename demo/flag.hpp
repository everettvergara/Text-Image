#ifndef FLAG_HPP
#define FLAG_HPP

#include <array>
#include "../include/text_video_anim.hpp"

using namespace g80;
using int_type = int16_t;
using uint_type = uint16_t;
using float_type = float;

constexpr uint_type flag_width = 120;
constexpr uint_type flag_height = 30;
constexpr uint_type wave_height = 6;
constexpr uint_type screen_height_allowance = 5;
constexpr uint_type fps = 15;

constexpr float_type waves = 8.0f;
constexpr float_type inc = waves * M_PI / flag_width;


class flag : public text_video_anim<int_type, uint_type> {
public:
    
    flag() : 
        text_video_anim<int_type, uint_type>(flag_width, flag_height + wave_height + screen_height_allowance, fps),
        pinoy_flag_(flag_width, flag_height) {}
    
    ~flag() = default;

    auto preprocess_wave_values() -> void {
        uint_type x = 0;
        for (float_type a = 0; a < waves * M_PI; a += inc) {
            wave_y_[x] = (wave_height / 2.0) - sin(a) * (wave_height / 2.0 - 1) - 1;
            wave_yn_[x] = +1;
            ++x;
        }
    }

    auto preprocess_flag_drawing() -> void {
        pinoy_flag_.fill_text(
            "BayangmagiliwPerlasngSilangananAlabngpusoSadibdibmoybuhayLupangHinirangDuyankangmagiting"
            "SamanlulupigDikapasisiilSadagatatbundokSsimoyatsalangitmongbughawMaydilagangtulaAtawitsa"
            "paglayangminamahalAngkislapngwatawatmoyTagumpaynanagniningningAngbituinatarawniyaKailanpa"
            "maydimagdidilimLupangarawngluwalhatitpagsintaBuhayaylangitsapilingmoAmingligayana'"
            "pagmaymangaapiAngmamatayngdahilsaiyo");

        for (uint_type i = 0; i < 15; ++i) {
            for (uint_type j = 1 + i * 3; j < flag_width; ++j) {
                pinoy_flag_.set_color(pinoy_flag_.ix(j, i), 4);
                pinoy_flag_.set_color(pinoy_flag_.ix(j, flag_height - i - 1), 1);
            }
        }

        pinoy_flag_.gfx_circle_color(14, 15, 6, 3);
        pinoy_flag_.gfx_fill_color(14, 15, 3);
        for (double i = 0.0; i < 2.0 * M_PI; i += 2.0 * M_PI / 8.0) {
            pinoy_flag_.set_color(static_cast<int_type>(14.0 + 7.0 * cos(i)), static_cast<int_type>(15.0 + 7.0 * sin(i)), 3);
            pinoy_flag_.set_color(static_cast<int_type>(14.0 + 8.0 * cos(i)), static_cast<int_type>(15.0 + 8.0 * sin(i)), 3);
        }

        pinoy_flag_.gfx_line_color(3, 3, 5, 5, 3);
        pinoy_flag_.gfx_line_color(3, 5, 5, 3, 3);
        pinoy_flag_.gfx_line_color(3, flag_height - 3 - 1, 5, flag_height - 5 - 1, 3);
        pinoy_flag_.gfx_line_color(3, flag_height - 5 - 1, 5, flag_height - 3 - 1, 3);
        pinoy_flag_.gfx_line_color(33, 14, 35, 16, 3);
        pinoy_flag_.gfx_line_color(33, 16, 35, 14, 3);
    }

    auto preprocess() -> bool {
        preprocess_wave_values();
        preprocess_flag_drawing();
        update();
        return true;
    }

    auto update() -> bool {

        screen_.fill_text(' ');
        pinoy_flag_.xlat_rotate_right(flag_width - 1, COLOR);

        for (int_type x = 0; x < static_cast<int_type>(flag_width); ++x) {
            text_image<int_type, uint_type> vertical_line = pinoy_flag_.get_image(x, 0, 1, flag_height);
            screen_.put_image(x, wave_height / 2  + wave_y_[x], vertical_line);
            wave_y_[x] = wave_y_[x] + wave_yn_[x];
            if (wave_y_[x] <= 0) {wave_y_[x] = 1; wave_yn_[x] = 1;} 
            else if (wave_y_[x] >= wave_height - 1) {wave_y_[x] = wave_height - 2; wave_yn_[x] = -1;} 
        }

        return true;
    }

private:
    text_image<int_type, uint_type> pinoy_flag_;
    std::array<int_type, flag_width> wave_y_;
    std::array<uint_type, flag_width> wave_yn_;
};

#endif