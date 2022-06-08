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

#include <iostream>
#include <fstream>
#include <memory>
#include <cstdint>
#include <optional>
#include <functional>
#include <sstream>



namespace g80 {

    // struct rect {int16_t x, y, w, h;};

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
    // todo: fix warnings size_t vs. uint16_t in for loops
    // TODO, remove reference if parameter type is primitive 
    // TODO, remove on return type unless necessary 

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
            w_(t.size()), h_(1), size_(w_),
            color_(std::make_unique<color[]>(size_)),
            text_(std::make_unique<text[]>(size_)),
            size_of_mask8bit_(size_ % 8 == 0 ? size_ / 8 : size_ / 8 + 1),
            mask8bit_(std::make_unique<mask8bit[]>(size_of_mask8bit_)) {
            for (size_t i = 0; i < size_; ++i) text_[i] = t[i];
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
            size_t next_line = w_;
            for (size_t i = 0; i < size_; ++i) {
                if (i == next_line) {output << "\n"; next_line += w_;} 
                output << std::setw(3) << std::hex << static_cast<int>(color_[i]);
            }

            output << "\033[0m\n";
            std::cout << output.str();              
        }
        
        auto show_text() const -> void {
            std::stringstream output;
            
            output << "\033[2J";
            size_t next_line = w_;
            for (size_t i = 0; i < size_; ++i) {
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
            size_t next_line = w_;
            for (size_t i = 0; i < size_; ++i) {
                if (i == next_line) {output << "\n"; next_line += w_;} 
                
                if (i == marker) output << "_";
                else output << ((mask8bit_[i / 8] & mask) ? "1" : ".");
                
                mask <<= 1;
                if (!mask) mask = 0x01;
            }

            output << "\033[0m\n\n";
            std::cout << output.str();
        }

    /**
     * Text, Color and Mask 
     * Getters and Setter
     * 
     */

    public:

        inline auto set_text(const int16_t &ix, const text t) -> void {
            text_[ix] = t;
        }
        
        inline auto set_text(const int16_t x, const int16_t y, const text t) -> void {
            set_text(ix(x, y), t);
        }

        inline auto get_text(const int16_t ix) const -> text {
            return text_[ix];
        }
        
        inline auto get_text(const int16_t x, const int16_t y) const -> text {
            return get_text(ix(x, y));
        }

        auto fill_text(const text t) -> void {
            std::fill_n(&text_[0], size_, t);      
        }

        //     auto fill_text(const String &text) -> void;

        //     // Color functions
        //     auto set_color(const Point &point, const Color &color) -> void;
        //     auto set_color(const int16_t &ix, const Color &color) -> void;
        //     auto get_color(const Point &point) const -> Color;
        //     auto get_color(const int16_t &ix) const -> Color;
        //     auto show_color() const -> void;
        auto fill_color(const color c) const -> void {
            std::fill_n(&color_[0], size_, c);      
        }

        //     // Mask functions
        //     auto set_mask(const Point &point, MASK_BIT mask_bit) -> void;
        //     auto set_mask(const int16_t &ix, MASK_BIT mask_bit) -> void;
        //     auto get_mask(const Point &point) const -> MASK_BIT;
        //     auto get_mask(const int16_t &ix) const -> MASK_BIT;
        auto set_all_mask8bit() -> void {
            std::fill_n(&mask8bit_[0], size_of_mask8bit_, 0xff);            
        }

        auto clear_all_mask8bit() -> void {
            std::fill_n(&mask8bit_[0], size_of_mask8bit_, 0x00);           
        }
        //     auto create_mask_if_color(const Color &color) -> void;
        //     auto create_mask_if_text(const Text &text) -> void;
        //     auto invert_mask() -> void;
        //     auto show_mask(int16_t marker = 0xffff) const -> void;
        //     auto show_mask_value() const -> void;

        //     // Text Image functions
        //     auto get_image(const Rect &rect) const -> TextImage;
        //     auto put_image(const TextImage &text_image, const Point &point) -> void;
        //     auto and_image(const TextImage &text_image, const Point &point) -> void;
        //     auto or_image(const TextImage &text_image, const Point &point) -> void;
        //     auto xor_image(const TextImage &text_image, const Point &point) -> void;
        //     auto put_text_color(const String &text, const Color &color, const Point &point) -> void;
            
        auto show() const -> void {
            static const std::string color[] { "\033[30m", "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m", "\033[37m" };
            static const size_t size_of_color = sizeof(color) / sizeof(std::string);
            std::stringstream output;
            
            output << "\033[2J";
            size_t prev_color = size_of_color;
            size_t next_line = w_;
            for (size_t i = 0; i < size_; ++i) {
                if (prev_color != color_[i]) {prev_color = color_[i]; output << color[prev_color];}
                if (i == next_line) {output << "\n"; next_line += w_;} 
                output << text_[i];
            }

            output << "\033[0m\n";
            std::cout << output.str();            
        }

        //     // Translate Functions
        //     // TODO: TEST xlat
        //     auto xlat_shift_left(int16_t shift, TextImageAttribute tia, const Text &default_text = ' ', const Color &default_color = 0, const MASK_BIT &default_mask_bit = OFF) -> void;
        //     auto xlat_shift_right(int16_t shift, TextImageAttribute tia, const Text &default_text = ' ', const Color &default_color = 0, const MASK_BIT &default_mask_bit = OFF) -> void;
        //     auto xlat_reverse(int16_t start, int16_t end, TextImageAttribute tia) -> void;
        //     auto xlat_rotate_left(int16_t rotate, TextImageAttribute tia) -> void;
        //     auto xlat_rotate_right(int16_t rotate, TextImageAttribute tia) -> void;
        //     auto xlat_flip_horizontal(TextImageAttribute tia) -> void;
        //     auto xlat_flip_vertical(TextImageAttribute tia) -> void;

        //     // Graphics
        //     auto gfx_point(const Point &point, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            
            
        //     auto gfx_line(const Point &point1, const Point &point2, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
        //     auto gfx_line_text(const Point &point1, const Point &point2, const Text &text) -> void;
        //     auto gfx_line_color(const Point &point1, const Point &point2, const Color &color) -> void;
        //     auto gfx_line_mask(const Point &point1, const Point &point2, const MASK_BIT &mask_bit) -> void;
            
            
        //     auto gfx_circle(const Point &point, const int16_t &radius, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
        //     auto gfx_circle_text(const Point &point, const int16_t &radius, const Text &text) -> void;
        //     auto gfx_circle_color(const Point &point, const int16_t &radius, const Color &color) -> void;
        //     auto gfx_circle_mask(const Point &point, const int16_t &radius, const MASK_BIT &mask_bit) -> void;

        //     auto gfx_arc(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
        //     auto gfx_arc_text(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, const Text &text) -> void;
        //     auto gfx_arc_color(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, const Color &color) -> void;
        //     auto gfx_arc_mask(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, const MASK_BIT &mask_bit) -> void;
            
        //     auto gfx_fill_with_text_border(const Point &point, const Text &text) -> void;
        //     auto gfx_fill_color_border(const Point &point, const Color &color) -> void;
        //     auto gfx_fill_with_mask_border(const Point &point, const MASK_BIT &mask_bit) -> void;
            
    
    /**
     * Helper functions
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

        // private:
        //     auto get_mask8bit_value(const int16_t &ix, const int16_t &size, const int16_t &init_offset = 0) const -> Mask8bitOp;
        //     auto gfx_circle_loop(const Point &point, const int16_t &radius, std::function<void(const int16_t &)> &tia_set) -> void;
        //     auto gfx_line_loop(const Point &point1, const Point &point2, std::function<void(const int16_t &)> &tia_set) -> void;
        //     auto gfx_arc_loop(const Point &point, const int16_t &radius, const int16_t &sa, const int16_t &ea, std::function<void(const int16_t &)> &tia_set) -> void;
        //     auto gfx_fill_loop(const Point &point, std::function<void(const int16_t &)> &tia_set, std::function<bool(const int16_t &)> &border_check) -> void;
    };
}

#endif 