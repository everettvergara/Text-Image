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
#include <functional>
#include <sstream>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <type_traits>

namespace g80 {

    /**
     * Type shortcuts
     * 
     * Uncomment #define UNSAFE_OPTIM
     * if you are sure that your program doesn't 
     * go beyond the boundaries of your canvass
     * 
     * #define UNSAFE_OPTIM
     * 
     */

    using color = uint8_t;
    using text = uint8_t;
    using mask8bit = uint8_t;

    using uptr_color = std::unique_ptr<color[]>;
    using uptr_text = std::unique_ptr<text[]>;
    using uptr_mask8bit = std::unique_ptr<mask8bit[]>;
    
    enum mask_bit {OFF = 0x00, ON = 0x01};
    enum text_image_attrib {TEXT = 1, COLOR = 2, MASK = 4, ALL = 7}; 

    /**
     * validator_if_less_than
     * A template validator for function parameters if 
     * less than a certain number.
     * 
     * T, type 
     * less_than, value 
     */

    template<typename T, T less_than>
    class validator_if_less_than {
    public:
        validator_if_less_than(const T &n) : n_(n) {if (n_ < less_than) throw std::runtime_error(std::string("Invalid parameter."));}
        operator const T &(void) const {return n_;}   
    private:
        T n_;
    };

    /**
     * text_image
     * A class to handle the console graphics. 
     * 
     * int_type, the type of int to be used
     * uint_type, type type of uint to be used
     */

    template<typename int_type, typename uint_type>
    class text_image {
    
    static_assert(sizeof(int_type) == sizeof(uint_type), "Size of int_type and uint_type must match");
    static_assert(
        std::is_same<int_type, int8_t>::value || std::is_same<int_type, int16_t>::value || 
        std::is_same<int_type, int32_t>::value || std::is_same<int_type, int64_t>::value, 
        "int_type must be one of: int8_t, int16_t, int32_t, int64_t");
    static_assert(
        std::is_same<uint_type, uint8_t>::value || std::is_same<uint_type, uint16_t>::value || 
        std::is_same<uint_type, uint32_t>::value || std::is_same<uint_type, uint64_t>::value, 
        "uint_type must be one of: uint8_t, uint_type, uint32_t, uint64_t");

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
            w_(validator_if_less_than<uint_type, 1>(static_cast<uint_type>(t.size()))), h_(1), size_(w_),
            color_(std::make_unique<color[]>(size_)),
            text_(std::make_unique<text[]>(size_)),
            size_of_mask8bit_(size_ % 8 == 0 ? size_ / 8 : size_ / 8 + 1),
            mask8bit_(std::make_unique<mask8bit[]>(size_of_mask8bit_)) {
            for (uint_type i = 0; i < size_; ++i) set_text(i, t[i]);
            fill_color(c);
            if (m) set_all_mask8bit(); else clear_all_mask8bit();
        } 
            
        text_image(const int_type w, const int_type h, const color c = 7, const text t = ' ', const mask_bit m = ON) :
            w_(validator_if_less_than<uint_type, 1>(w)), h_(validator_if_less_than<uint_type, 1>(h)), size_(w_ * h_),
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

        inline auto width() const -> uint_type {
            return w_;
        }

        inline auto height() const -> uint_type {
            return h_;
        }

        inline auto size() const -> uint_type {
            return size_;
        }

        inline auto size_mask8bit() const -> uint_type {
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
            uint_type next_line = w_;
            for (uint_type i = 0; i < size_; ++i) {
                if (i == next_line) {output << "\n"; next_line += w_;} 
                output << std::setw(3) << std::hex << static_cast<int>(color_[i]);
            }

            output << "\033[0m\n";
            std::cout << output.str();              
        }

        auto show_text() const -> void {
            std::stringstream output;
            
            output << "\033[2J";
            uint_type next_line = w_;
            for (uint_type i = 0; i < size_; ++i) {
                if (i == next_line) {output << "\n"; next_line += w_;} 
                output << text_[i];
            }

            output << "\033[0m\n";
            std::cout << output.str();              
        }

        auto show_mask(int_type marker) const -> void {
            std::stringstream output;
            
            mask8bit mask = 0x01;

            // Draw header label
            for (int_type i = 0; i < w_; ++i) output << i % 10;
            output << "\n";
            
            // Draw bits
            uint_type next_line = w_;
            for (uint_type i = 0; i < size_; ++i) {
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
            for (uint_type i = 0; i < size_of_mask8bit_; ++i)
                if (mask8bit_[i]) std::cout << "i: " << i << " - " << static_cast<uint_type>(mask8bit_[i]) << "\n";
            std::cout << std::endl;            
        }


    /**
     * Text, Color and Mask 
     * Getters and Setters
     * 
     */

    public:

        inline auto set_text(const uint_type &i, const text t) -> void {
            #ifdef UNSAFE_OPTIM     
            text_[i] = t;
            #else
            if (i < size_) text_[i] = t;
            #endif
        }
        
        inline auto set_text(const int_type x, const int_type y, const text t) -> void {
            set_text(ix(x, y), t);
        }

        inline auto get_text(const uint_type i) const -> text {
            return text_[i];
        }
        
        inline auto get_text(const int_type x, const int_type y) const -> text {
            return get_text(ix(x, y));
        }

        auto fill_text(const text t) -> void {
            std::fill_n(&text_[0], size_, t);      
        }

        auto fill_text(const std::string &s) -> void {
            for (uint_type i = 0, j = 0; i < size_; ++i, ++j) {
                if (j == static_cast<uint_type>(s.size())) j = 0;
                text_[i] = s[j];
            }
        } 

        inline auto set_color(const uint_type i, const color c) -> void {
            #ifdef UNSAFE_OPTIM     
            color_[i] = c;
            #else
            if (i < size_) color_[i] = c;
            #endif
        }

        inline auto set_color(const int_type x, const int_type y, const color c) -> void {
            set_color(ix(x, y), c);
        }

        inline auto get_color(const uint_type ix) const -> color {
            return color_[ix];
        }

        inline auto get_color(const int_type x, const int_type y) const -> color {
            return get_color(ix(x, y));
        }
        
        auto fill_color(const color c) const -> void {
            std::fill_n(&color_[0], size_, c);      
        }

        inline auto set_mask(const uint_type i, mask_bit m) -> void {
            #ifdef UNSAFE_OPTIM     
                uint_type offset = i % 8;
                mask8bit and_mask = ~(1 << offset);
                mask8bit or_mask = m << offset;
                mask8bit_[i / 8] &= and_mask;
                mask8bit_[i / 8] |= or_mask;
            #else 
                if (i < size_) {
                    uint_type offset = i % 8;
                    mask8bit and_mask = ~(1 << offset);
                    mask8bit or_mask = m << offset;
                    mask8bit_[i / 8] &= and_mask;
                    mask8bit_[i / 8] |= or_mask;
                }
            #endif 
        }
        
        inline auto set_mask(const int_type x, const int_type y, mask_bit m) -> void {
            set_mask(ix(x, y), m);
        }

        inline auto get_mask(const uint_type &ix) const -> mask_bit {
            uint_type offset = ix % 8;
            mask8bit value = mask8bit_[ix / 8];
            value = mask8bit_[ix / 8] & (1 << offset);
            return value ? ON : OFF;
        }

        inline auto get_mask(const int_type x, int_type y) const -> mask_bit {
            return get_mask(ix(x, y));
        }
        
        auto set_all_mask8bit() -> void {
            std::fill_n(&mask8bit_[0], size_of_mask8bit_, 0xff);            
        }

        auto clear_all_mask8bit() -> void {
            std::fill_n(&mask8bit_[0], size_of_mask8bit_, 0x00);           
        }

        auto create_mask_if_color(const color c) -> void {
            for (uint_type i = 0; i < size_; ++i) 
                set_mask(i, color_[i] == c ? ON : OFF);
        }

        auto create_mask_if_text(const text &t) -> void {
            for (uint_type i = 0; i < size_; ++i) 
                set_mask(i, text_[i] == t ? ON : OFF);
        }

        auto invert_mask() -> void {
            for (uint_type i = 0; i < size_of_mask8bit_; ++i)
                mask8bit_[i] = ~mask8bit_[i];            
        }

        auto put_text_color(const int_type x, const int_type y, const std::string &t, const color c) -> void {
            uint_type start = ix(x, y);
            for (uint_type i = 0; i < static_cast<uint_type>(t.size()); ++i) {
                uint_type j = (start + i);
                set_text(j, t[i]);
                set_color(j, c);
            }
        }
        
    /**
     * Image get and put
     * 
     */

    private:

        auto get_mask8bit_value(const uint_type ix, const uint_type size, const uint_type init_offset = 0) const -> mask8bit {
            if (size == 0 || size > 8) return 0;

            uint_type ix8 = ix / 8;
            uint_type nix8 = (ix + size - 1) / 8;
            uint_type offset = ix % 8 - init_offset;

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

        auto get_image(const int_type x, const int_type y, const uint_type w, const uint_type h) const -> text_image {

            if (w == 0 || h == 0) return text_image();

            text_image dest_text_image(w, h, 7, ' ', OFF);
            text *text_ptr = dest_text_image.raw_text().get();
            color *color_ptr = dest_text_image.raw_color().get();

            uint_type start = ix(x, y);
            for (uint_type row = 0; row < h; ++row) {
                uint_type i = (start + row * w_) % size_;
                memcpy(text_ptr, &text_[i], sizeof(text) * w);
                memcpy(color_ptr, &color_[i], sizeof(color) * w);
                text_ptr += w;
                color_ptr += w;
            }

            // Copy mask in chunks of 8-bits
            // which is faster by a factor of 3 than 
            // copying mask bit by bit

            mask8bit *mask8bit_ptr = dest_text_image.raw_mask8bit().get();
            uint_type total_copied = 0;
            for (uint_type row = 0; row < h; ++row) {
                uint_type ix = (start + row * w_) % size_;
                uint_type size = w;
                do {
                    uint_type init_offset = total_copied % 8;
                    uint_type s = 8 - total_copied % 8;
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

        auto bit_image(const int_type x, const int_type y, const text_image &timg, 
        const std::function<auto (uint_type, uint_type, const text_image &) -> bool> &conditional) {
            for (uint_type r = 0; r < timg.h_; ++r) {
                uint_type tix = ix(x, y + r); 
                for (uint_type six = timg.ix(0, r), sixm = six + timg.w_; six < sixm; ++six) {           
                    if (conditional(tix, six, timg)) {
                        set_text(tix, timg.craw_text().get()[six]);
                        set_color(tix, timg.craw_color().get()[six]);
                    }
                    ++tix;
                }
            }            
        }

    public:

        auto put_image(const int_type x, const int_type y, const text_image &timg) -> void {
            static const auto conditional = [&](uint_type, uint_type, const text_image &) -> bool {return true;};
            bit_image(x, y, timg, conditional);
        }

        auto and_image(const int_type x, const int_type y, const text_image &timg) -> void {
            static const auto conditional = [&](uint_type tix, uint_type six, const text_image &timg) -> bool {return (get_mask(tix) & timg.get_mask(six)) == ON;};
            bit_image(x, y, timg, conditional);
        }

        auto or_image(const int_type x, const int_type y, const text_image &timg) -> void {
            static const auto conditional = [&](uint_type tix, uint_type six, const text_image &timg) -> bool {return (get_mask(tix) | timg.get_mask(six)) == ON;};
            bit_image(x, y, timg, conditional);
        }

        auto xor_image(const int_type x, const int_type y, const text_image &timg) -> void {
            static const auto conditional = [&](uint_type tix, uint_type six, const text_image &timg) -> bool {return (get_mask(tix) ^ timg.get_mask(six)) == ON;};
            bit_image(x, y, timg, conditional);
        }

    /**
     * Image translations
     * 
     */

    public:

        auto xlat_shift_left(uint_type shift, const text_image_attrib tia = ALL, const text &default_text = ' ', const color &default_color = 0, const mask_bit &default_mask_bit = OFF) -> void {
            
            if (shift > size_) shift = size_;
            else if (shift == 0) return;

            if (tia & TEXT) {
                for (uint_type i = 0; i < size_ - shift; ++i) set_text(i, text_[i + shift]);
                for (uint_type i = size_ - shift; i < size_; ++i) set_text(i, default_text);
            }

            if (tia & COLOR) {
                for (uint_type i = 0; i < size_ - shift; ++i) set_color(i, color_[i + shift]);
                for (uint_type i = size_ - shift; i < size_; ++i) set_color(i, default_color);
            }

            if (tia & MASK) {
                for (uint_type i = 0; i < size_ - shift; ++i) set_mask(i, get_mask(i + shift));
                for (uint_type i = size_ - shift; i < size_; ++i) set_mask(i, default_mask_bit);      
            }            
        }

        auto xlat_shift_right(uint_type shift, const text_image_attrib tia = ALL, const text &default_text = ' ', const color &default_color = 0, const mask_bit &default_mask_bit = OFF) -> void {

            if (shift > size_) shift = size_;
            else if (shift == 0) return;

            if (tia & TEXT) {
                for (uint_type i = size_ - 1; i >= shift; --i) set_text(i, text_[i - shift]);
                for (uint_type i = 0; i < shift; ++i) set_text(i, default_text);
            }

            if (tia & COLOR) {
                for (uint_type i = size_ - 1; i >= shift; --i) set_color(i, color_[i - shift]);
                for (uint_type i = 0; i < shift; ++i) set_color(i, default_color);
            }

            if (tia & MASK) {
                for (uint_type i = size_ - 1; i >= shift; --i) set_mask(i, get_mask(i - shift));
                for (uint_type i = 0; i < shift; ++i) set_mask(i, default_mask_bit);
            }
        }

        auto xlat_reverse(const uint_type start, const uint_type end, const text_image_attrib tia = ALL) -> void {
            if (tia & TEXT) {
                uint_type i = start, j = end;
                while (i < j) std::swap(text_[i++], text_[j--]);
            }

            if (tia & COLOR) {
                uint_type i = start, j = end;
                while (i < j) std::swap(color_[i++], color_[j--]);
            }

            if (tia & MASK) {
                uint_type i = start, j = end;
                while (i < j) {
                    mask_bit t = get_mask(i);
                    set_mask(i, get_mask(j));
                    set_mask(j, t);
                    ++i;
                    --j;
                }
            }            
        }

        auto xlat_rotate_left(const uint_type rotate, const text_image_attrib tia) -> void {
            uint_type r = rotate >= size_ ? rotate % size_ : rotate;
            if (r > 0) {
                xlat_reverse(0, r - 1, tia);
                xlat_reverse(r, size_ - 1, tia);
                xlat_reverse(0, size_ - 1, tia);
            }
        }
        
        auto xlat_rotate_right(const uint_type rotate, const text_image_attrib tia) -> void {
            uint_type r = rotate >= size_ ? rotate % size_ : rotate;
            if (r > 0) {
                xlat_reverse(0, size_ - 1, tia);
                xlat_reverse(0, r - 1, tia);
                xlat_reverse(r, size_ - 1, tia);
            }            
        }

        auto xlat_flip_horizontal(const text_image_attrib tia) -> void {
            for (uint_type i = 0; i < h_; ++i) {
                uint_type j = ix(0, i);
                xlat_reverse(j, j + w_ - 1, tia);
            }
        }
        
        auto xlat_flip_vertical(const text_image_attrib tia) -> void {
            if (tia & TEXT) {
                uint_type i = 0;
                uint_type j = h_ - 1;
                while (i < j) {
                    uint_type k = ix(0, i++);
                    uint_type kmax = k + w_;
                    uint_type l = ix(0, j--);
                    while (k < kmax) std::swap(text_[k++], text_[l++]);
                }
            }

            if (tia & COLOR) {
                uint_type i = 0;
                uint_type j = h_ - 1;
                while (i < j) {
                    uint_type k = ix(0, i++);
                    uint_type kmax = k + w_;
                    uint_type l = ix(0, j--);
                    while (k < kmax) std::swap(color_[k++], color_[l++]);
                }
            }

            if (tia & MASK) {
                uint_type i = 0;
                uint_type j = h_ - 1;
                while (i < j) {
                    uint_type k = ix(0, i++);
                    uint_type kmax = k + w_;
                    uint_type l = ix(0, j--);
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

        auto gfx_point(const int_type x, const int_type y, const text t, const color c, const mask_bit m) -> void {
            uint_type i = ix(x, y);
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

        auto gfx_line_loop(const int_type x1, const int_type y1, const int_type x2, const int_type y2,  const std::function<void(const uint_type)> &set_tia) -> void {
            int_type dx = x2 - x1;
            int_type dy = y2 - y1;
            int_type sdx = dx < 0 ? -1 : 1;
            int_type sdy = dy < 0 ? -w_ : w_;
            int_type adx = dx < 0 ? dx * -1 : dx;
            int_type ady = dy < 0 ? dy * -1 : dy;
            uint_type curr_point = ix(x1, y1);

            auto draw_line = [&](const int_type adg, const int_type sdg, const int_type adl, const int_type sdl) -> void {
                for (int_type i = 0, t = adl; i <= adg; ++i, t += adl) {
                    set_tia(curr_point);
                    if (t >= adg) {curr_point += sdl; t -= adg; }
                    curr_point +=sdg;
                }
            };

            if (adx >= ady) draw_line(adx, sdx, ady, sdy);  
            else draw_line(ady, sdy, adx, sdx);  
        }

    public:

        auto gfx_line_color(const int_type x1, const int_type y1, const int_type x2, const int_type y2, const color c) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_color(i, c);};
            gfx_line_loop(x1, y1, x2, y2, set_tia);
        }

        auto gfx_line_text(const int_type x1, const int_type y1, const int_type x2, const int_type y2, const text t) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_text(i, t);};
            gfx_line_loop(x1, y1, x2, y2, set_tia);
        }

        auto gfx_line_mask(const int_type x1, const int_type y1, const int_type x2, const int_type y2, const mask_bit m) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_mask(i, m);};
            gfx_line_loop(x1, y1, x2, y2, set_tia);
        }

        auto gfx_line(const int_type x1, const int_type y1, const int_type x2, const int_type y2, const color c, const text t, const mask_bit m) -> void {
            gfx_line_color(x1, y1, x2, y2, c);
            gfx_line_text(x1, y1, x2, y2, t);
            gfx_line_mask(x1, y1, x2, y2, m);
        }

    /**
     * Text Graphics: Circle
     * 
     */

    private:

        auto gfx_circle_loop(const int_type cx, const int_type cy, const int_type r, const std::function<auto (const uint_type) -> void> &set_tia) -> void {
            
            uint_type center_point = ix(cx, cy);

            int_type x = r;
            int_type y = 0;

            int_type bx = x * w_;
            int_type by = y * w_;

            int_type dx = 1 - (r << 1);
            int_type dy = 1;
            int_type re = 0;

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
        
        auto gfx_circle_color(const int_type cx, const int_type cy, const int_type r, const color c) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_color(i, c);};
            gfx_circle_loop(cx, cy, r, set_tia);
        }
        
        auto gfx_circle_text(const int_type cx, const int_type cy, const int_type r, const text t) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_text(i, t);};
            gfx_circle_loop(cx, cy, r, set_tia);
        }

        auto gfx_circle_mask(const int_type cx, const int_type cy, const int_type r, const mask_bit m) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_mask(i, m);};
            gfx_circle_loop(cx, cy, r, set_tia);
        }

        auto gfx_circle(const int_type cx, const int_type cy, const int_type r, const color c, const text t, const mask_bit m) -> void {
            gfx_circle_color(cx, cy, r, c);
            gfx_circle_text(cx, cy, r, t);
            gfx_circle_mask(cx, cy, r, m);
        }

    /**
     * Text Graphics: Arc
     * 
     */

    private:

        auto gfx_arc_loop(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const std::function<auto (const uint_type) -> void> &set_tia) -> void {
            uint_type center_point = ix(cx, cy);

            int_type x = r;
            int_type y = 0;
            
            int_type bx = x * w_;
            int_type by = y * w_;

            int_type dx = 1 - (r << 1);
            int_type dy = 1;
            int_type re = 0;

            int_type n_sa, n_ea;
            if (sa > ea) {n_sa = ea; n_ea = sa;} 
            else {n_sa = sa; n_ea = ea;}

            // Reduce to n_sa < 360 only
            if (n_sa > 360 && n_ea > 360) {
                int_type t = n_sa / 360;
                n_sa -= 360 * t;
                n_ea -= 360 * t;
            }

            int_type extended_sa, extended_ea;
            if (n_ea > 360) {extended_sa = 0; extended_ea = n_ea % 360;} 
            else {extended_sa = -1; extended_ea = -1;}

            struct octa_bound {int_type sx, ex, *xy, xy_mul, *bxy, bxy_mul, type; };
            std::unordered_map<int_type, octa_bound> octa_bounds;

            int_type t_sa = n_sa;
            int_type t_ea = n_ea;
            for (int_type j = 0; j <= 8; j += 8) {
                for (int_type i = j, a = 0; i < j + 8; ++i, a += 45) {
                    octa_bound ob;
                    
                    // 0: n_sa ----| a  ------ a45 | ------- n_ea 
                    if (a >= t_sa && a + 45 <= t_ea) {
                        ob.sx = static_cast<int_type>(cos(a * M_PI / 180) * r);
                        ob.ex = static_cast<int_type>(cos((a + 45) * M_PI / 180) * r);
                        ob.type = 0;
                    
                    // 1: a ----| n_sa  ------ n_ea | ------- a45 
                    } else if (t_sa >= a && t_ea <= a + 45) {
                        ob.sx = static_cast<int_type>(cos(t_sa * M_PI / 180) * r);
                        ob.ex = static_cast<int_type>(cos(t_ea * M_PI / 180) * r);
                        ob.type = 1;
                    
                    // 2: a ----| n_sa  ------ a45 | ------- n_ea 
                    } else if (t_sa >= a && t_sa <= a + 45) {
                        ob.sx = static_cast<int_type>(cos(t_sa * M_PI / 180) * r);
                        ob.ex = static_cast<int_type>(cos((a + 45) * M_PI / 180) * r);
                        ob.type = 2;
                    
                    // 3: n_sa ---- | a ---- n_ea  ------ | a45 
                    } else if (t_ea > a && t_ea <= a + 45) {
                        ob.sx = static_cast<int_type>(cos(a * M_PI / 180) * r);
                        ob.ex = static_cast<int_type>(cos(t_ea * M_PI / 180) * r);
                        ob.type = 3;
                    
                    // Beyond scope of octant
                    } else {
                        continue;
                    }

                    int_type ix = i % 8;
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
                    uint_type i = center_point + (*ob.xy * ob.xy_mul) + (*ob.bxy * ob.bxy_mul);
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

        auto gfx_arc_color(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const color c) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_color(i, c);};
            gfx_arc_loop(cx, cy, r, sa, ea, set_tia);
        }
        
        auto gfx_arc_text(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const text t) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_text(i, t);};
            gfx_arc_loop(cx, cy, r, sa, ea, set_tia);
        }

        auto gfx_arc_mask(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const mask_bit m) -> void {
            static const std::function<auto (const int_type &) -> void> set_tia = [&](const uint_type i) -> void {set_mask(i, m);};
            gfx_arc_loop(cx, cy, r, sa, ea, set_tia);
        }

        auto gfx_arc(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const color c, const text t, const mask_bit m) -> void {
            gfx_arc_color(cx, cy, r, sa, ea, c);
            gfx_arc_text(cx, cy, r, sa, ea, t);
            gfx_arc_mask(cx, cy, r, sa, ea, m);
        }
    
    /**
     * Text Graphics: Fill
     * 
     */

    private:

        auto gfx_fill_loop(const int_type sx, const int_type sy, const std::function<auto (const uint_type) -> void> &set_tia, const std::function<auto (const uint_type) -> bool> &is_border) -> void {
            std::vector<std::tuple<int_type, int_type>> points(size_);
            int_type si = -1;
            if (!is_border(ix(sx, sy))) points[++si] = {sx, sy};
            while (si >= 0) {
                auto [x, y] = points[si--];
                uint_type i = ix(x, y);
                set_tia(i);
                if (y - 1 >= 0 && !is_border(ix(x, y - 1))) points[++si] = {x, y - 1};
                if (y + 1 < h_ && !is_border(ix(x, y + 1))) points[++si] = {x, y + 1};
                if (x - 1 >= 0 && !is_border(ix(x - 1, y))) points[++si] = {x - 1, y};
                if (x + 1 < w_ && !is_border(ix(x + 1, y))) points[++si] = {x + 1, y};
            }
        }
    
    public:

        auto gfx_fill_color(const int_type x, const int_type y, const color c) -> void {
            static const std::function<auto (const uint_type) -> void> set_tia = [&](const uint_type i) -> void {set_color(i, c);};
            static const std::function<auto (const uint_type) -> bool> is_border = [&](const uint_type i) -> bool {if (i >= size_) return true; return color_[i] == c;};
            gfx_fill_loop(x, y, set_tia, is_border);
        }

        auto gfx_fill_text(const int_type x, const int_type y, const text t) -> void {
            static const std::function<auto (const uint_type) -> void> set_tia = [&](const uint_type i) -> void {set_text(i, t);};
            static const std::function<auto (const uint_type) -> bool> is_border = [&](const uint_type i) -> bool {if (i >= size_) return true; return text_[i] == t;};
            gfx_fill_loop(x, y, set_tia, is_border);
        }

        auto gfx_fill_mask(const int_type x, const int_type y, const mask_bit m) -> void {
            static const std::function<auto (const uint_type) -> void> set_tia = [&](const uint_type i) -> void {set_mask(i, m);};
            static const std::function<auto (const uint_type) -> bool> is_border = [&](const uint_type i) -> bool {if (i >= size_) return true; return get_mask(i) == m;};
            gfx_fill_loop(x, y, set_tia, is_border);
        }

        auto gfx_fill_with_text_border(const int_type x, const int_type y, const color c, const text t, const mask_bit m) -> void {
            static const std::function<auto (const uint_type) -> void> set_tia = [&](const uint_type i) -> void {
                {
                    set_color(i, c);
                    set_text(i, t);
                    set_mask(i, m);
                }
            };
            static const std::function<auto (const uint_type) -> bool> is_border = [&](const uint_type i) -> bool {if (i >= size_) return true; return text_[i] == t;};
            gfx_fill_loop(x, y, set_tia, is_border);
        }

    /**
     * Misc Helper functions
     * 
     */

    public:

        inline auto ix(const int_type x, const int_type y) const -> uint_type {
            return static_cast<uint_type>(y * w_ + x);
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
            size_ = validator_if_less_than<uint_type, 1>(w_) * validator_if_less_than<uint_type, 1>(h_);
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
            static const uint_type size_of_color = sizeof(color) / sizeof(std::string);
            std::stringstream output;
            
            output << "\033[2J";
            uint_type prev_color = size_of_color;
            uint_type next_line = w_;
            for (uint_type i = 0; i < size_; ++i) {
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

        uint_type w_, h_, size_;
        uptr_color color_{nullptr};
        uptr_text text_{nullptr};
        uint_type size_of_mask8bit_{0};
        uptr_mask8bit mask8bit_{nullptr};

    };
}

#endif 