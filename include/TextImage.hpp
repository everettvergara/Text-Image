/*
 *  TextImage Class for console out buffering
 *  Copyright (C) 2022 Everett Gaius S. Vergara
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *	
 */

#ifndef TEXT_IMAGE_HPP
#define TEXT_IMAGE_HPP

#include <memory>
#include <cstdint>
#include <optional>
#include <functional>

#include "Area.hpp"
#include "Rect.hpp"

namespace g80 {

    // struct Area {

    //     int16_t w{0};
    //     int16_t h{0};

    //     Area () {};
    //     Area (const int16_t &w, const int16_t &h) : 
    //         w(w), h(h) {}

    //     auto operator()() const -> int16_t {
    //         return w * h;
    //     }
    // };

    using Color = uint8_t;
    using Text = unsigned char;
    using Mask8bit = uint8_t;
    using Sizeof_mask8bit = uint16_t;
    using Uptr_color = std::unique_ptr<Color[]>;
    using Uptr_text = std::unique_ptr<Text[]>;
    using Uptr_mask8bit = std::unique_ptr<Mask8bit[]>;
    using Mask8bitOp = std::optional<Mask8bit>;
    using String = std::string;

    enum MASK_BIT {OFF = 0x00, ON = 0x01};
    enum TextImageAttribute {TEXT = 1, COLOR = 2, MASK = 4, ALL = 7}; 

    class TextImage {
        public:
            // Constructors, assignments and destructors
            TextImage();
            TextImage(const String &filename);
            TextImage(const String &text, const Color &color, MASK_BIT mask_bit = ON);
            TextImage(const Area &area, MASK_BIT mask_bit = ON);
            TextImage(const Area &area, const Color &color, const Text &text, MASK_BIT = ON);
            TextImage(const TextImage &rhs);
            TextImage(TextImage &&rhs);
            auto operator=(const TextImage &rhs) -> TextImage &; 
            auto operator=(TextImage &&rhs) -> TextImage &;
            ~TextImage() = default;

            // Protected Getters
            auto area() const -> const Area &;
            auto raw_color() -> Uptr_color &;
            auto craw_color() const -> const Uptr_color &;
            auto raw_text() -> Uptr_text &;
            auto craw_text() const -> const Uptr_text &;
            auto raw_mask8bit() -> Uptr_mask8bit &;
            auto craw_mask8bit() const -> const Uptr_mask8bit &;
            auto size_of_mask8bit() -> Sizeof_mask8bit;

            // Text functions
            auto set_text(const Point &point, const Text &text) -> void;
            auto set_text(const int16_t &ix, const Text &text) -> void;
            auto get_text(const Point &point) const -> Text;
            auto get_text(const int16_t &ix) const -> Text;
            auto show_text() const -> void;
            auto fill_text(const Text &text) -> void;
            auto fill_text(const String &text) -> void;

            // Color functions
            auto set_color(const Point &point, const Color &color) -> void;
            auto set_color(const int16_t &ix, const Color &color) -> void;
            auto get_color(const Point &point) const -> Color;
            auto get_color(const int16_t &ix) const -> Color;
            auto show_color() const -> void;
            auto fill_color(const Color &color) const -> void;

            // Mask functions
            auto set_mask(const Point &point, MASK_BIT mask_bit) -> void;
            auto set_mask(const int16_t &ix, MASK_BIT mask_bit) -> void;
            auto get_mask(const Point &point) const -> MASK_BIT;
            auto get_mask(const int16_t &ix) const -> MASK_BIT;
            auto set_all_mask8bit() -> void;  
            auto clear_all_mask8bit() -> void;
            auto create_mask_if_color(const Color &color) -> void;
            auto create_mask_if_text(const Text &text) -> void;
            auto invert_mask() -> void;
            auto show_mask(int16_t marker = 0xffff) const -> void;
            auto show_mask_value() const -> void;

            // Text Image functions
            auto get_image(const Rect &rect) const -> TextImage;
            auto put_image(const TextImage &text_image, const Point &point) -> void;
            auto and_image(const TextImage &text_image, const Point &point) -> void;
            auto or_image(const TextImage &text_image, const Point &point) -> void;
            auto xor_image(const TextImage &text_image, const Point &point) -> void;
            auto put_text_color(const String &text, const Color &color, const Point &point) -> void;
            
            auto show() const -> void;

            // Translate Functions
            // TODO: TEST xlat
            auto xlat_shift_left(int16_t shift, TextImageAttribute tia, const Text &default_text = ' ', const Color &default_color = 0, const MASK_BIT &default_mask_bit = OFF) -> void;
            auto xlat_shift_right(int16_t shift, TextImageAttribute tia, const Text &default_text = ' ', const Color &default_color = 0, const MASK_BIT &default_mask_bit = OFF) -> void;
            auto xlat_reverse(int16_t start, int16_t end, TextImageAttribute tia) -> void;
            auto xlat_rotate_left(int16_t rotate, TextImageAttribute tia) -> void;
            auto xlat_rotate_right(int16_t rotate, TextImageAttribute tia) -> void;
            auto xlat_flip_horizontal(TextImageAttribute tia) -> void;
            auto xlat_flip_vertical(TextImageAttribute tia) -> void;

            // Graphics
            auto gfx_point(const Point &point, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            
            
            auto gfx_line(const Point &point1, const Point &point2, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            auto gfx_line_text(const Point &point1, const Point &point2, const Text &text) -> void;
            auto gfx_line_color(const Point &point1, const Point &point2, const Color &color) -> void;
            auto gfx_line_mask(const Point &point1, const Point &point2, const MASK_BIT &mask_bit) -> void;
            
            
            auto gfx_circle(const Point &point, const int16_t &radius, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            auto gfx_circle_text(const Point &point, const int16_t &radius, const Text &text) -> void;
            auto gfx_circle_color(const Point &point, const int16_t &radius, const Color &color) -> void;
            auto gfx_circle_mask(const Point &point, const int16_t &radius, const MASK_BIT &mask_bit) -> void;

            auto gfx_arc(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            auto gfx_arc_text(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, const Text &text) -> void;
            auto gfx_arc_color(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, const Color &color) -> void;
            auto gfx_arc_mask(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, const MASK_BIT &mask_bit) -> void;
            
            auto gfx_fill_with_text_border(const Point &point, const Text &text) -> void;
            auto gfx_fill_color_border(const Point &point, const Color &color) -> void;
            auto gfx_fill_with_mask_border(const Point &point, const MASK_BIT &mask_bit) -> void;
            
            // Save and Load
            auto save(const String &filename) const -> void;
            auto load(const String &filename) -> void;
            
            // Helper functions
            auto index(const Point &point) const -> int16_t;

        protected:
            Area area_{0,0};
            Uptr_color color_{nullptr};
            Uptr_text text_{nullptr};
            Sizeof_mask8bit size_of_mask8bit_{0};
            Uptr_mask8bit mask8bit_{nullptr};

        private:
            auto get_mask8bit_value(const int16_t &ix, const int16_t &size, const int16_t &init_offset = 0) const -> Mask8bitOp;
            auto gfx_circle_loop(const Point &point, const int16_t &radius, std::function<void(const int16_t &)> &tia_set) -> void;
            auto gfx_line_loop(const Point &point1, const Point &point2, std::function<void(const int16_t &)> &tia_set) -> void;
            auto gfx_arc_loop(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, std::function<void(const int16_t &)> &tia_set) -> void;
            auto gfx_fill_loop(const Point &point, std::function<void(const int16_t &)> &tia_set, std::function<bool(const int16_t &)> &border_check) -> void;
    };

 

}

#endif 