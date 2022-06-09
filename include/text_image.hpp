/*
 *  text_image.hpp A class for console text graphics
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

#include <iostream>
#include <fstream>
#include <memory>
#include <cstdint>
#include <optional>
#include <functional>
#include <sstream>
#include <unordered_map>
#include <tuple>
#include <vector>

namespace g80 {

    using color = uint8_t;
    using text = uint8_t;
    using mask8bit = uint8_t;

    using uptr_color = std::unique_ptr<color[]>;
    using uptr_text = std::unique_ptr<text[]>;
    using uptr_mask8bit = std::unique_ptr<mask8bit[]>;
    using mask8bitop = std::optional<mask8bit>;

    enum mask_bit {OFF = 0x00, ON = 0x01};
    enum text_image_attrib {TEXT = 1, COLOR = 2, MASK = 4, ALL = 7}; 

    
    class text_image {
            
    // todo: use template instead of int16_t
    // * todo: all ix must be uint
    // todo: fix warnings uint16_t vs. uint16_t in for loops
    // TODO, remove reference if parameter type is primitive 
    // TODO, remove on return type unless necessary 
    // todo: throw exception on construction if w_, h_ == 0
    // todo: decide whether or not to guarantee and use uint in ix
    // todo: decide where to put ix % size_ validation, if its before or upon setting of text, color or mask
    // todo: TEST ALL functions

    /**
     * Constructors, Assignments and 
     * Destructors 
     * 
     */

    public:
    
        text_image() = default;

        text_image(const std::string &filename) {
            load(filename);
        }

        text_image(const std::string &t, const color c, const mask_bit m = ON) :
            w_(static_cast<uint16_t>(t.size())), h_(1), size_(w_),
            color_(std::make_unique<color[]>(size_)),
            text_(std::make_unique<text[]>(size_)),
            size_of_mask8bit_(size_ % 8 == 0 ? size_ / 8 : size_ / 8 + 1),
            mask8bit_(std::make_unique<mask8bit[]>(size_of_mask8bit_)) {
            for (uint16_t i = 0; i < size_; ++i) set_text(i, t[i]);
            fill_color(c);
            if (m) set_all_mask8bit(); else clear_all_mask8bit();
        } 
            
        text_image(const int16_t w, const int16_t h, const color c = 7, const text t = ' ', const mask_bit m = ON) :
            w_(w), h_(h), size_(w_ * h_),
            color_(std::make_unique<color[]>(size_)),
            text_(std::make_unique<text[]>(size_)),
            size_of_mask8bit_(size_ % 8 == 0 ? size_ / 8 : size_ / 8 + 1),
            mask8bit_(std::make_unique<mask8bit[]>(size_of_mask8bit_)) {
            fill_color(c);
            fill_text(t);
            if (m) set_all_mask8bit(); else clear_all_mask8bit();
        }

        text_image(const text_image &rhs) :
            w_(rhs.w_), h_(rhs.h_), size_(w_ * h_),
            size_of_mask8bit_(rhs.size_of_mask8bit_) {
            const color *c = rhs.craw_color().get();
            std::copy(c, c + size_, color_.get());
            const text *t = rhs.craw_text().get();
            std::copy(t, t + size_, text_.get());
            const mask8bit *m = rhs.craw_mask8bit().get();
            std::copy(m, m + size_of_mask8bit_, mask8bit_.get());
        }

        text_image(text_image &&rhs) :
            w_(rhs.w_), h_(rhs.h_), size_(w_ * h_),
            size_of_mask8bit_(rhs.size_of_mask8bit_) {
            color_.reset(rhs.raw_color().get());
            rhs.color_.release();
            text_.reset(rhs.raw_text().get());
            rhs.text_.release();
            mask8bit_.reset(rhs.raw_mask8bit().get());
            rhs.mask8bit_.release();
        }

        auto operator=(const text_image &rhs) -> text_image & {
            if (this != &rhs) {
                w_ = {rhs.w_};
                h_ = {rhs.h_};
                size_ = {rhs.size_};
                size_of_mask8bit_ = (rhs.size_of_mask8bit_);
                color_.reset(new color[size_]);
                std::copy(rhs.color_.get(), rhs.color_.get() + size_, color_.get());
                text_.reset(new text[size_]);
                std::copy(rhs.text_.get(), rhs.text_.get() + size_, text_.get());
                mask8bit_.reset(new mask8bit[size_of_mask8bit_]);
                std::copy(rhs.mask8bit_.get(), rhs.mask8bit_.get() + size_, mask8bit_.get());
            }
            return *this;
        }
        
        auto operator=(text_image &&rhs) -> text_image & {
            if (this != &rhs) {
                w_ = {rhs.w_};
                h_ = {rhs.h_};
                size_ = {rhs.size_};
                size_of_mask8bit_ = (rhs.size_of_mask8bit_);
                color_.reset(rhs.raw_color().get());
                rhs.color_.release();
                text_.reset(rhs.raw_text().get());
                rhs.text_.release();
                mask8bit_.reset(rhs.raw_mask8bit().get());
                rhs.mask8bit_.release();
            }
            return *this;
        }
        
        ~text_image() = default;

    /**
     * Protected Property Getters
     * 
     */

    public:

        inline auto width() const -> uint16_t {
            return w_;
        }

        inline auto height() const -> uint16_t {
            return h_;
        }

        inline auto size() const -> uint16_t {
            return size_;
        }

        inline auto size_mask8bit() const -> uint16_t {
            return size_of_mask8bit_;
        }        

        inline auto raw_color() -> uptr_color & {
            return color_;
        }

        inline auto craw_color() const -> const uptr_color & {
            return color_;
        }

        inline auto raw_text() -> uptr_text & {
            return text_;
        }

        inline auto craw_text() const -> const uptr_text & {
            return text_;
        }

        inline auto raw_mask8bit() -> uptr_mask8bit & {
            return mask8bit_;
        }
        
        inline auto craw_mask8bit() const -> const uptr_mask8bit & {
            return mask8bit_;
        }


    /** 
     * Debuggers
     * 
     */

    public:

        auto show_color() const -> void {
            std::stringstream output;
            
            output << "\033[2J";
            uint16_t next_line = w_;
            for (uint16_t i = 0; i < size_; ++i) {
                if (i == next_line) {output << "\n"; next_line += w_;} 
                output << std::setw(3) << std::hex << static_cast<int>(color_[i]);
            }

            output << "\033[0m\n";
            std::cout << output.str();              
        }

        auto show_text() const -> void {
            std::stringstream output;
            
            output << "\033[2J";
            uint16_t next_line = w_;
            for (uint16_t i = 0; i < size_; ++i) {
                if (i == next_line) {output << "\n"; next_line += w_;} 
                output << text_[i];
            }

            output << "\033[0m\n";
            std::cout << output.str();              
        }

        auto show_mask(int16_t marker) const -> void {
            std::stringstream output;
            
            mask8bit mask = 0x01;

            // Draw header label
            for (int16_t i = 0; i < w_; ++i) output << i % 10;
            output << "\n";
            
            // Draw bits
            uint16_t next_line = w_;
            for (uint16_t i = 0; i < size_; ++i) {
                if (i == next_line) {output << "\n"; next_line += w_;} 
                
                if (i == marker) output << "_";
                else output << ((mask8bit_[i / 8] & mask) ? "1" : ".");
                
                mask <<= 1;
                if (!mask) mask = 0x01;
            }

            output << "\033[0m\n\n";
            std::cout << output.str();
        }

        auto show_mask_value() const -> void {
            for (uint16_t i = 0; i < size_of_mask8bit_; ++i)
                if (mask8bit_[i]) std::cout << "i: " << i << " - " << static_cast<uint16_t>(mask8bit_[i]) << "\n";
            std::cout << std::endl;            
        }


    /**
     * Text, Color and Mask 
     * Getters and Setters
     * 
     */

    public:

        inline auto set_text(const uint16_t &i, const text t) -> void {
            set_text(i, t);
        }
        
        inline auto set_text(const int16_t x, const int16_t y, const text t) -> void {
            set_text(ix(x, y), t);
        }

        inline auto get_text(const uint16_t i) const -> text {
            return text_[i];
        }
        
        inline auto get_text(const int16_t x, const int16_t y) const -> text {
            return get_text(ix(x, y));
        }

        auto fill_text(const text t) -> void {
            std::fill_n(&text_[0], size_, t);      
        }

        inline auto set_color(const uint16_t i, const color c) -> void {
            set_color(i, c);
        }

        inline auto set_color(const int16_t x, const int16_t y, const color c) -> void {
            set_color(ix(x, y), c);
        }

        inline auto get_color(const uint16_t ix) const -> color {
            return color_[ix];
        }

        inline auto get_color(const int16_t x, const int16_t y) const -> color {
            return get_color(ix(x, y));
        }
        
        auto fill_color(const color c) const -> void {
            std::fill_n(&color_[0], size_, c);      
        }

        inline auto set_mask(const uint16_t i, mask_bit m) -> void {
            {
                uint16_t offset = i % 8;
                mask8bit and_mask = ~(1 << offset);
                mask8bit or_mask = m << offset;
                mask8bit_[i / 8] &= and_mask;
                mask8bit_[i / 8] |= or_mask;
            }
        }
        
        inline auto set_mask(const int16_t x, const int16_t y, mask_bit m) -> void {
            set_mask(ix(x, y), m);
        }

        inline auto get_mask(const uint16_t &ix) const -> mask_bit {
            uint16_t offset = ix % 8;
            mask8bit value = mask8bit_[ix / 8];
            value = mask8bit_[ix / 8] & (1 << offset);
            return value ? ON : OFF;
        }

        inline auto get_mask(const int16_t x, int16_t y) const -> mask_bit {
            return get_mask(ix(x, y));
        }
        
        auto set_all_mask8bit() -> void {
            std::fill_n(&mask8bit_[0], size_of_mask8bit_, 0xff);            
        }

        auto clear_all_mask8bit() -> void {
            std::fill_n(&mask8bit_[0], size_of_mask8bit_, 0x00);           
        }

        auto create_mask_if_color(const color c) -> void {
            for (uint16_t i = 0; i < size_; ++i) 
                set_mask(i, color_[i] == c ? ON : OFF);
        }

        auto create_mask_if_text(const text &t) -> void {
            for (uint16_t i = 0; i < size_; ++i) 
                set_mask(i, text_[i] == t ? ON : OFF);
        }

        auto invert_mask() -> void {
            for (uint16_t i = 0; i < size_of_mask8bit_; ++i)
                mask8bit_[i] = ~mask8bit_[i];            
        }

        auto put_text_color(const int16_t x, const int16_t y, const std::string &t, const color c) -> void {
            uint16_t start = ix(x, y);
            for (uint16_t i = 0; i < t.size(); ++i) {
                uint16_t j = (start + i);
                set_text(j, t[i]);
                set_color(j, c);
            }
        }
        
    /**
     * Image get and put
     * 
     */

    private:

        auto get_mask8bit_value(const uint16_t ix, const uint16_t size, const uint16_t init_offset = 0) const -> mask8bit {
            if (size == 0 || size > 8) return 0;

            uint16_t ix8 = ix / 8;
            uint16_t nix8 = (ix + size - 1) / 8;
            uint16_t offset = ix % 8 - init_offset;

            mask8bit value;
            if (offset >= 0) {
                value = mask8bit_[ix8] >> offset;
                value &= ~((1 << init_offset) - 1);
                if (nix8 > ix8 && nix8 < size_of_mask8bit_)
                    value |= mask8bit_[nix8] << (8 - offset);
                value &= (1 << (size + init_offset)) - 1;
            } else {
                value = mask8bit_[ix8];
                value = mask8bit_[ix8] << -offset;
                value &= ~((1 << init_offset) - 1);
                value &= (1 << (size + init_offset)) - 1;
            } 

            return value;
        }

    public:

        auto get_image(const int16_t x, const int16_t y, const uint16_t w, const uint16_t h) const -> text_image {
            
            text_image dest_text_image(w, h, 7, ' ', OFF);
            text *text_ptr = dest_text_image.raw_text().get();
            color *color_ptr = dest_text_image.raw_color().get();

            uint16_t start = ix(x, y);
            for (uint16_t row = 0; row < h; ++row) {
                uint16_t i = (start + row * w_) % size_;
                memcpy(text_ptr, &text_[i], sizeof(text) * w);
                memcpy(color_ptr, &color_[i], sizeof(color) * w);
                text_ptr += w;
                color_ptr += w;
            }

            // Copy mask in chunks of 8-bits
            // which is faster by a factor of 3 than 
            // copying mask bit by bit

            mask8bit *mask8bit_ptr = dest_text_image.raw_mask8bit().get();
            uint16_t total_copied = 0;
            for (uint16_t row = 0; row < h; ++row) {
                uint16_t ix = (start + row * w_) % size_;
                uint16_t size = w;
                do {
                    uint16_t init_offset = total_copied % 8;
                    uint16_t s = 8 - total_copied % 8;
                    s = s > size ? size : s;
                    mask8bit mask = get_mask8bit_value(ix, s, init_offset);
                    mask8bit_ptr[total_copied / 8] |= mask;
                    total_copied += s;
                    ix += s;
                    size -= s;
                } while(size > 0);
            }

            return dest_text_image;            
        }

    private:

        auto bit_image(const int16_t x, const int16_t y, const text_image &timg, 
        const std::function<auto (uint16_t, uint16_t, const text_image &) -> bool> &conditional) {
            for (uint16_t r = 0; r < timg.h_; ++r) {
                uint16_t tix = ix(x, y + r); 
                for (uint16_t six = timg.ix(0, r), sixm = six + timg.w_; six < sixm; ++six) {           
                    if (conditional(tix, six, timg)) {
                        set_text(tix, timg.craw_text().get()[six]);
                        set_color(tix, timg.craw_color().get()[six]);
                    }
                    ++tix;
                }
            }            
        }

    public:

        auto put_image(const int16_t x, const int16_t y, const text_image &timg) -> void {
            static const auto conditional = [&](uint16_t, uint16_t, const text_image &) -> bool {return true;};
            bit_image(x, y, timg, conditional);
        }

        auto and_image(const int16_t x, const int16_t y, const text_image &timg) -> void {
            static const auto conditional = [&](uint16_t tix, uint16_t six, const text_image &timg) -> bool {return (get_mask(tix) & timg.get_mask(six)) == ON;};
            bit_image(x, y, timg, conditional);
        }

        auto or_image(const int16_t x, const int16_t y, const text_image &timg) -> void {
            static const auto conditional = [&](uint16_t tix, uint16_t six, const text_image &timg) -> bool {return (get_mask(tix) | timg.get_mask(six)) == ON;};
            bit_image(x, y, timg, conditional);
        }

        auto xor_image(const int16_t x, const int16_t y, const text_image &timg) -> void {
            static const auto conditional = [&](uint16_t tix, uint16_t six, const text_image &timg) -> bool {return (get_mask(tix) ^ timg.get_mask(six)) == ON;};
            bit_image(x, y, timg, conditional);
        }

    /**
     * Image translations
     * 
     */

    public:

        auto xlat_shift_left(uint16_t shift, const text_image_attrib tia = ALL, const text &default_text = ' ', const color &default_color = 0, const mask_bit &default_mask_bit = OFF) -> void {
            
            if (shift > size_) shift = size_;
            else if (shift == 0) return;

            if (tia & TEXT) {
                for (uint16_t i = 0; i < size_ - shift; ++i) set_text(i, text_[i + shift]);
                for (uint16_t i = size_ - shift; i < size_; ++i) set_text(i, default_text);
            }

            if (tia & COLOR) {
                for (uint16_t i = 0; i < size_ - shift; ++i) set_color(i, color_[i + shift]);
                for (uint16_t i = size_ - shift; i < size_; ++i) set_color(i, default_color);
            }

            if (tia & MASK) {
                for (uint16_t i = 0; i < size_ - shift; ++i) set_mask(i, get_mask(i + shift));
                for (uint16_t i = size_ - shift; i < size_; ++i) set_mask(i, default_mask_bit);      
            }            
        }

        auto xlat_shift_right(uint16_t shift, const text_image_attrib tia = ALL, const text &default_text = ' ', const color &default_color = 0, const mask_bit &default_mask_bit = OFF) -> void {

            if (shift > size_) shift = size_;
            else if (shift == 0) return;

            if (tia & TEXT) {
                for (uint16_t i = size_ - 1; i >= shift; --i) set_text(i, text_[i - shift]);
                for (uint16_t i = 0; i < shift; ++i) set_text(i, default_text);
            }

            if (tia & COLOR) {
                for (uint16_t i = size_ - 1; i >= shift; --i) set_color(i, color_[i - shift]);
                for (uint16_t i = 0; i < shift; ++i) set_color(i, default_color);
            }

            if (tia & MASK) {
                for (uint16_t i = size_ - 1; i >= shift; --i) set_mask(i, get_mask(i - shift));
                for (uint16_t i = 0; i < shift; ++i) set_mask(i, default_mask_bit);
            }
        }

        auto xlat_reverse(const uint16_t start, const uint16_t end, const text_image_attrib tia = ALL) -> void {
            if (tia & TEXT) {
                uint16_t i = start, j = end;
                while (i < j) std::swap(text_[i++], text_[j--]);
            }

            if (tia & COLOR) {
                uint16_t i = start, j = end;
                while (i < j) std::swap(color_[i++], color_[j--]);
            }

            if (tia & MASK) {
                uint16_t i = start, j = end;
                while (i < j) {
                    mask_bit t = get_mask(i);
                    set_mask(i, get_mask(j));
                    set_mask(j, t);
                    ++i;
                    --j;
                }
            }            
        }

        auto xlat_rotate_left(const uint16_t rotate, const text_image_attrib tia) -> void {
            uint16_t r = rotate >= size_ ? rotate % size_ : rotate;
            if (r > 0) {
                xlat_reverse(0, r - 1, tia);
                xlat_reverse(r, size_ - 1, tia);
                xlat_reverse(0, size_ - 1, tia);
            }
        }
        
        auto xlat_rotate_right(const uint16_t rotate, const text_image_attrib tia) -> void {
            uint16_t r = rotate >= size_ ? rotate % size_ : rotate;
            if (r > 0) {
                xlat_reverse(0, size_ - 1, tia);
                xlat_reverse(0, r - 1, tia);
                xlat_reverse(r, size_ - 1, tia);
            }            
        }

        auto xlat_flip_horizontal(const text_image_attrib tia) -> void {
            for (uint16_t i = 0; i < h_; ++i) {
                uint16_t j = ix(0, i);
                xlat_reverse(j, j + w_ - 1, tia);
            }
        }
        
        auto xlat_flip_vertical(const text_image_attrib tia) -> void {
            if (tia & TEXT) {
                uint16_t i = 0;
                uint16_t j = h_ - 1;
                while (i < j) {
                    uint16_t k = ix(0, i++);
                    uint16_t kmax = k + w_;
                    uint16_t l = ix(0, j--);
                    while (k < kmax) std::swap(text_[k++], text_[l++]);
                }
            }

            if (tia & COLOR) {
                uint16_t i = 0;
                uint16_t j = h_ - 1;
                while (i < j) {
                    uint16_t k = ix(0, i++);
                    uint16_t kmax = k + w_;
                    uint16_t l = ix(0, j--);
                    while (k < kmax) std::swap(color_[k++], color_[l++]);
                }
            }

            if (tia & MASK) {
                uint16_t i = 0;
                uint16_t j = h_ - 1;
                while (i < j) {
                    uint16_t k = ix(0, i++);
                    uint16_t kmax = k + w_;
                    uint16_t l = ix(0, j--);
                    while (k < kmax) {
                        mask_bit t = get_mask(l);
                        set_mask(l, get_mask(k));
                        set_mask(k, t);
                        ++l;
                        ++k; 
                    }
                }
            }            
        }


    /**
     * Text Graphics: Point
     * 
     */

    public:

        auto gfx_point(const int16_t x, const int16_t y, const text t, const color c, const mask_bit m) -> void {
            uint16_t i = ix(x, y);
            {
                set_text(i, t);
                set_color(i, c);
                set_mask(i, m);
            }         
        }

    /**
     * Text Graphics: Line
     * 
     */

    private:

        auto gfx_line_loop(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2,  const std::function<void(const uint16_t)> &set_tia) -> void {
            int16_t dx = x2 - x1;
            int16_t dy = y2 - y1;
            int16_t sdx = dx < 0 ? -1 : 1;
            int16_t sdy = dy < 0 ? -w_ : w_;
            int16_t adx = dx < 0 ? dx * -1 : dx;
            int16_t ady = dy < 0 ? dy * -1 : dy;
            uint16_t curr_point = ix(x1, y1);

            auto draw_line = [&](const int16_t adg, const int16_t sdg, const int16_t adl, const int16_t sdl) -> void {
                for (int16_t i = 0, t = adl; i <= adg; ++i, t += adl) {
                    set_tia(curr_point);
                    if (t >= adg) {curr_point += sdl; t -= adg; }
                    curr_point +=sdg;
                }
            };

            if (adx >= ady) draw_line(adx, sdx, ady, sdy);  
            else draw_line(ady, sdy, adx, sdx);  
        }

    public:

        auto gfx_line_color(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2, const color c) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_color(i, c);};
            gfx_line_loop(x1, y1, x2, y2, set_tia);
        }

        auto gfx_line_text(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2, const text t) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_text(i, t);};
            gfx_line_loop(x1, y1, x2, y2, set_tia);
        }

        auto gfx_line_mask(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2, const mask_bit m) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_mask(i, m);};
            gfx_line_loop(x1, y1, x2, y2, set_tia);
        }

        auto gfx_line(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2, const color c, const text t, const mask_bit m) -> void {
            gfx_line_color(x1, y1, x2, y2, c);
            gfx_line_text(x1, y1, x2, y2, t);
            gfx_line_mask(x1, y1, x2, y2, m);
        }

    /**
     * Text Graphics: Circle
     * 
     */

    private:

        auto gfx_circle_loop(const int16_t cx, const int16_t cy, const int16_t r, const std::function<auto (const uint16_t) -> void> &set_tia) -> void {
            
            uint16_t center_point = ix(cx, cy);

            int16_t x = r;
            int16_t y = 0;

            int16_t bx = x * w_;
            int16_t by = y * w_;

            int16_t dx = 1 - (r << 1);
            int16_t dy = 1;
            int16_t re = 0;

            while (x >= y) {

                set_tia(center_point + x - by);
                set_tia(center_point + y - bx);
                set_tia(center_point - y - bx);
                set_tia(center_point - x - by);
                set_tia(center_point + x + by);
                set_tia(center_point + y + bx);
                set_tia(center_point - y + bx);
                set_tia(center_point - x + by);

                ++y;
                re += dy;
                dy += 2;
                if ((re << 1) + dx > 0) {
                    --x;
                    bx -= w_;
                    re += dx;
                    dx += 2;
                }
                by += w_;
            }            
        }
    
    public:
        
        auto gfx_circle_color(const int16_t cx, const int16_t cy, const int16_t r, const color c) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_color(i, c);};
            gfx_circle_loop(cx, cy, r, set_tia);
        }
        
        auto gfx_circle_text(const int16_t cx, const int16_t cy, const int16_t r, const text t) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_text(i, t);};
            gfx_circle_loop(cx, cy, r, set_tia);
        }

        auto gfx_circle_mask(const int16_t cx, const int16_t cy, const int16_t r, const mask_bit m) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_mask(i, m);};
            gfx_circle_loop(cx, cy, r, set_tia);
        }

        auto gfx_circle(const int16_t cx, const int16_t cy, const int16_t r, const color c, const text t, const mask_bit m) -> void {
            gfx_circle_color(cx, cy, r, c);
            gfx_circle_text(cx, cy, r, t);
            gfx_circle_mask(cx, cy, r, m);
        }

    /**
     * Text Graphics: Arc
     * 
     */

    private:

        auto gfx_arc_loop(const int16_t cx, const int16_t cy, const int16_t r, const int16_t sa, const int16_t ea, const std::function<auto (const uint16_t) -> void> &set_tia) -> void {
            uint16_t center_point = ix(cx, cy);

            int16_t x = r;
            int16_t y = 0;
            
            int16_t bx = x * w_;
            int16_t by = y * w_;

            int16_t dx = 1 - (r << 1);
            int16_t dy = 1;
            int16_t re = 0;

            int16_t n_sa, n_ea;
            if (sa > ea) {n_sa = ea; n_ea = sa;} 
            else {n_sa = sa; n_ea = ea;}

            // Reduce to n_sa < 360 only
            if (n_sa > 360 && n_ea > 360) {
                int16_t t = n_sa / 360;
                n_sa -= 360 * t;
                n_ea -= 360 * t;
            }

            int16_t extended_sa, extended_ea;
            if (n_ea > 360) {extended_sa = 0; extended_ea = n_ea % 360;} 
            else {extended_sa = -1; extended_ea = -1;}

            struct octa_bound {int16_t sx, ex, *xy, xy_mul, *bxy, bxy_mul, type; };
            std::unordered_map<int16_t, octa_bound> octa_bounds;

            int16_t t_sa = n_sa;
            int16_t t_ea = n_ea;
            for (int16_t j = 0; j <= 8; j += 8) {
                for (int16_t i = j, a = 0; i < j + 8; ++i, a += 45) {
                    octa_bound ob;
                    
                    // 0: n_sa ----| a  ------ a45 | ------- n_ea 
                    if (a >= t_sa && a + 45 <= t_ea) {
                        ob.sx = static_cast<int16_t>(cos(a * M_PI / 180) * r);
                        ob.ex = static_cast<int16_t>(cos((a + 45) * M_PI / 180) * r);
                        ob.type = 0;
                    
                    // 1: a ----| n_sa  ------ n_ea | ------- a45 
                    } else if (t_sa >= a && t_ea <= a + 45) {
                        ob.sx = static_cast<int16_t>(cos(t_sa * M_PI / 180) * r);
                        ob.ex = static_cast<int16_t>(cos(t_ea * M_PI / 180) * r);
                        ob.type = 1;
                    
                    // 2: a ----| n_sa  ------ a45 | ------- n_ea 
                    } else if (t_sa >= a && t_sa <= a + 45) {
                        ob.sx = static_cast<int16_t>(cos(t_sa * M_PI / 180) * r);
                        ob.ex = static_cast<int16_t>(cos((a + 45) * M_PI / 180) * r);
                        ob.type = 2;
                    
                    // 3: n_sa ---- | a ---- n_ea  ------ | a45 
                    } else if (t_ea > a && t_ea <= a + 45) {
                        ob.sx = static_cast<int16_t>(cos(a * M_PI / 180) * r);
                        ob.ex = static_cast<int16_t>(cos(t_ea * M_PI / 180) * r);
                        ob.type = 3;
                    
                    // Beyond scope of octant
                    } else {
                        continue;
                    }

                    int16_t ix = i % 8;
                    if (ix < 4) {
                        ob.bxy_mul = -1;
                        std::swap(ob.sx, ob.ex);
                    } else {
                        ob.bxy_mul = +1;
                    }

                    if (ix == 0 || ix == 1 || ix == 6 || ix == 7) ob.xy_mul = +1;
                    else ob.xy_mul = -1;

                    if (ix == 0 || ix == 3 || ix == 4 || ix == 7) {
                        ob.xy = &x;
                        ob.bxy = &by;
                    } else {
                        ob.xy = &y;
                        ob.bxy = &bx;
                    }

                    auto f = octa_bounds.find(ix);
                    if (f == octa_bounds.end()) {
                        octa_bounds.insert({ix, ob});
                    } else {
                        if (f->second.type == 1 || f->second.type == 2) {
                            octa_bounds.insert({i, ob});
                            break;
                        }
                    }
                }

                if (extended_sa < 0) break;
                t_sa = extended_sa;
                t_ea = extended_ea;
            }

            auto draw_arc = [&](const octa_bound &ob) -> void { 
                if (*ob.xy * ob.xy_mul >= ob.sx && 
                    *ob.xy * ob.xy_mul <= ob.ex) {
                    uint16_t i = center_point + (*ob.xy * ob.xy_mul) + (*ob.bxy * ob.bxy_mul);
                    set_tia(i);
                }
            };

            while (x >= y)
            {
                for (auto &o : octa_bounds) draw_arc(o.second);

                ++y;
                re += dy;
                dy += 2;
                if ((re << 1) + dx > 0)
                {
                    --x;
                    bx -= w_;
                    re += dx;
                    dx += 2;
                }
                by += w_;
            }    
        }

    public:

        auto gfx_arc_color(const int16_t cx, const int16_t cy, const int16_t r, const int16_t sa, const int16_t ea, const color c) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_color(i, c);};
            gfx_arc_loop(cx, cy, r, sa, ea, set_tia);
        }
        
        auto gfx_arc_text(const int16_t cx, const int16_t cy, const int16_t r, const int16_t sa, const int16_t ea, const text t) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_text(i, t);};
            gfx_arc_loop(cx, cy, r, sa, ea, set_tia);
        }

        auto gfx_arc_mask(const int16_t cx, const int16_t cy, const int16_t r, const int16_t sa, const int16_t ea, const mask_bit m) -> void {
            static const std::function<auto (const int16_t &) -> void> set_tia = [&](const uint16_t i) -> void {set_mask(i, m);};
            gfx_arc_loop(cx, cy, r, sa, ea, set_tia);
        }

        auto gfx_arc(const int16_t cx, const int16_t cy, const int16_t r, const int16_t sa, const int16_t ea, const color c, const text t, const mask_bit m) -> void {
            gfx_arc_color(cx, cy, r, sa, ea, c);
            gfx_arc_text(cx, cy, r, sa, ea, t);
            gfx_arc_mask(cx, cy, r, sa, ea, m);
        }
    
    /**
     * Text Graphics: Fill
     * 
     */

    private:

        auto gfx_fill_loop(const int16_t sx, const int16_t sy, const std::function<auto (const uint16_t) -> void> &set_tia, const std::function<auto (const uint16_t) -> bool> &is_border) -> void {
            std::vector<std::tuple<int16_t, int16_t>> points(size_);
            int16_t si = -1;
            if (!is_border(ix(sx, sy))) points[++si] = {sx, sy};
            while (si >= 0) {
                auto [x, y] = points[si--];
                uint16_t i = ix(x, y);
                set_tia(i);
                if (y - 1 >= 0 && !is_border(ix(x, y - 1))) points[++si] = {x, y - 1};
                if (y + 1 < h_ && !is_border(ix(x, y + 1))) points[++si] = {x, y + 1};
                if (x - 1 >= 0 && !is_border(ix(x - 1, y))) points[++si] = {x - 1, y};
                if (x + 1 < w_ && !is_border(ix(x + 1, y))) points[++si] = {x + 1, y};
            }
        }
    
    public:

        auto gfx_fill_color(const int16_t x, const int16_t y, const color c) -> void {
            static const std::function<auto (const uint16_t) -> void> set_tia = [&](const uint16_t i) -> void {set_color(i, c);};
            static const std::function<auto (const uint16_t) -> bool> is_border = [&](const uint16_t i) -> bool {if (i >= size_) return true; return color_[i] == c;};
            gfx_fill_loop(x, y, set_tia, is_border);
        }

        auto gfx_fill_text(const int16_t x, const int16_t y, const text t) -> void {
            static const std::function<auto (const uint16_t) -> void> set_tia = [&](const uint16_t i) -> void {set_text(i, t);};
            static const std::function<auto (const uint16_t) -> bool> is_border = [&](const uint16_t i) -> bool {if (i >= size_) return true; return text_[i] == t;};
            gfx_fill_loop(x, y, set_tia, is_border);
        }

        auto gfx_fill_mask(const int16_t x, const int16_t y, const mask_bit m) -> void {
            static const std::function<auto (const uint16_t) -> void> set_tia = [&](const uint16_t i) -> void {set_mask(i, m);};
            static const std::function<auto (const uint16_t) -> bool> is_border = [&](const uint16_t i) -> bool {if (i >= size_) return true; return get_mask(i) == m;};
            gfx_fill_loop(x, y, set_tia, is_border);
        }

        auto gfx_fill_with_text_border(const int16_t x, const int16_t y, const color c, const text t, const mask_bit m) -> void {
            static const std::function<auto (const uint16_t) -> void> set_tia = [&](const uint16_t i) -> void {
                {
                    set_color(i, c);
                    set_text(i, t);
                    set_mask(i, m);
                }
            };
            static const std::function<auto (const uint16_t) -> bool> is_border = [&](const uint16_t i) -> bool {if (i >= size_) return true; return text_[i] == t;};
            gfx_fill_loop(x, y, set_tia, is_border);
        }

    /**
     * Misc Helper functions
     * 
     */

    public:

        inline auto ix(const int16_t x, const int16_t y) const -> uint16_t {
            return static_cast<uint16_t>(y * w_ + x);
        }

        auto save(const std::string &filename) const -> void {
            std::ofstream file (filename, std::ios::binary);
            file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            file.write(static_cast<const char *>(static_cast<const void*>(&w_)), sizeof(w_));
            file.write(static_cast<const char *>(static_cast<const void*>(&h_)), sizeof(h_));
            file.write(static_cast<const char *>(static_cast<const void*>(color_.get())), size_);
            file.write(static_cast<const char *>(static_cast<const void*>(text_.get())), size_);
            file.write(static_cast<const char *>(static_cast<const void*>(mask8bit_.get())), size_of_mask8bit_);
        }

        auto load(const std::string &filename) -> void {
            std::ifstream file (filename, std::ios::binary);
            file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            file.read(static_cast<char *>(static_cast<void *>(&w_)), sizeof(w_));
            file.read(static_cast<char *>(static_cast<void *>(&h_)), sizeof(h_));
            size_ = w_ * h_;
            size_of_mask8bit_ = size_ % 8 == 0 ? size_ / 8 : size_ / 8 + 1;
            color_.reset(new color[size_]);
            file.read(static_cast<char *>(static_cast<void *>(color_.get())), size_);
            text_.reset(new text[size_]);            
            file.read(static_cast<char *>(static_cast<void *>(text_.get())), size_);
            mask8bit_.reset(new mask8bit[size_of_mask8bit_]);           
            file.read(static_cast<char *>(static_cast<void *>(mask8bit_.get())), size_of_mask8bit_);
        }
        
        auto show() const -> void {
            static const std::string color[] { "\033[30m", "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m", "\033[37m" };
            static const uint16_t size_of_color = sizeof(color) / sizeof(std::string);
            std::stringstream output;
            
            output << "\033[2J";
            uint16_t prev_color = size_of_color;
            uint16_t next_line = w_;
            for (uint16_t i = 0; i < size_; ++i) {
                if (prev_color != color_[i]) {prev_color = color_[i]; output << color[prev_color];}
                if (i == next_line) {output << "\n"; next_line += w_;} 
                output << text_[i];
            }

            output << "\033[0m\n";
            std::cout << output.str();            
        }
        
    /**
     *  Protected Properties
     * 
     */

    protected:

        uint16_t w_, h_, size_;
        uptr_color color_{nullptr};
        uptr_text text_{nullptr};
        uint16_t size_of_mask8bit_{0};
        uptr_mask8bit mask8bit_{nullptr};

    };
}

#endif 