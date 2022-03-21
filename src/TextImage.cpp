/*
 *  TextImage.cpp for g80::TextImage Class
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

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "TextImage.h"


g80::TextImage::TextImage() {

}

g80::TextImage::TextImage(const String &filename) {
    load(filename);
}

g80::TextImage::TextImage(const String &text, const Color &color, MASK_BIT mask_bit) : 
    area_({static_cast<Dim>(text.size()), 1}),
    color_(std::make_unique<Color[]>(area_())),
    text_(std::make_unique<Text[]>(area_())),
    size_of_mask8bit_(area_() % 8 == 0 ? area_() / 8 : area_() / 8 + 1),
    mask8bit_(std::make_unique<Mask8bit[]>(size_of_mask8bit_)) {
    
    for (Dim i = 0; i < area_(); ++i)
        text_[i] = text[i];

    fill_color(color);
    
    if (mask_bit)
        set_all_mask8bit();
    else 
        clear_all_mask8bit();
}

g80::TextImage::TextImage(const Area &area, MASK_BIT mask_bit) : 
    area_(area),
    color_(std::make_unique<Color[]>(area_())),
    text_(std::make_unique<Text[]>(area_())),
    size_of_mask8bit_(area_() % 8 == 0 ? area_() / 8 : area_() / 8 + 1),
    mask8bit_(std::make_unique<Mask8bit[]>(size_of_mask8bit_)) {
    
    if (mask_bit)
        set_all_mask8bit();
    else 
        clear_all_mask8bit();
}

g80::TextImage::TextImage(const Area &area, const Color &color, const Text &text, MASK_BIT mask_bit) :
    TextImage(area, mask_bit) {
    fill_color(color);
    fill_text(text);
}

g80::TextImage::TextImage(const TextImage &rhs) : 
    TextImage(rhs.area_) {
            
    const Color *color = rhs.craw_color().get();
    std::copy(color, color + area_(), color_.get());

    const Text *text = rhs.craw_text().get();
    std::copy(text, text + area_(), text_.get());

    const Mask8bit *mask8bit = rhs.craw_mask8bit().get();
    std::copy(mask8bit, mask8bit + size_of_mask8bit_, mask8bit_.get());
}

g80::TextImage::TextImage(TextImage &&rhs) : 
    area_(rhs.area_), size_of_mask8bit_(rhs.size_of_mask8bit_) {

    color_.reset(rhs.raw_color().get());
    rhs.raw_color().release();

    text_.reset(rhs.raw_text().get());
    rhs.raw_text().release();

    mask8bit_.reset(rhs.raw_mask8bit().get());
    rhs.raw_mask8bit().release();
}


auto g80::TextImage::operator=(const TextImage &rhs) -> TextImage & {
    if (this != &rhs) {
        area_ = rhs.area_;
        color_.release();
        color_ = std::make_unique<Color[]>(area_());
        const Color *color = rhs.craw_color().get();
        std::copy(color, color + area_(), color_.get());
        
        text_.release();
        text_ = std::make_unique<Text[]>(area_());
        const Text *text = rhs.craw_text().get();
        std::copy(text, text + area_(), text_.get());

        size_of_mask8bit_ = rhs.size_of_mask8bit_;
        mask8bit_.release();
        mask8bit_ = std::make_unique<Mask8bit[]>(size_of_mask8bit_);
        const Mask8bit *mask8bit = rhs.craw_mask8bit().get();
        std::copy(mask8bit, mask8bit + size_of_mask8bit_, mask8bit_.get());
    }
    return *this;
}

auto g80::TextImage::operator=(TextImage &&rhs) -> TextImage & {
    if (this != &rhs) {
        area_ = rhs.area_;
        size_of_mask8bit_ = rhs.size_of_mask8bit_;
        color_.reset(rhs.raw_color().get());
        rhs.raw_color().release();
        text_.reset(rhs.raw_text().get());
        rhs.raw_text().release();
        mask8bit_.reset(rhs.raw_mask8bit().get());
        rhs.raw_mask8bit().release();
    }
    return *this;
}

auto g80::TextImage::area() const -> const Area & {
    return area_;
}

auto g80::TextImage::raw_color() -> Uptr_color & {
    return color_;
}

auto g80::TextImage::craw_color() const -> const Uptr_color & {
    return color_;
}

auto g80::TextImage::raw_text() -> Uptr_text & {
    return text_;
}

auto g80::TextImage::craw_text() const -> const Uptr_text & {
    return text_;
}

auto g80::TextImage::size_of_mask8bit() -> Sizeof_mask8bit {
    return size_of_mask8bit_;
}

auto g80::TextImage::raw_mask8bit() -> Uptr_mask8bit & {
    return mask8bit_;
}

auto g80::TextImage::craw_mask8bit() const -> const Uptr_mask8bit & {
    return mask8bit_;
}

auto g80::TextImage::fill_color(const Color &color) const -> void {
    for (Dim i = 0; i < area_(); ++i)
        color_[i] = color;
}

auto g80::TextImage::fill_text(const Text &text) -> void {
    for (Dim i = 0; i < area_(); ++i)
        text_[i] = text;
}

auto g80::TextImage::fill_text(const String &text) -> void {
    for (Dim i = 0; i < area_(); ++i)
        text_[i] = text[i % text.size()];
} 

auto g80::TextImage::set_all_mask8bit() -> void {
    for (Dim i = 0; i < size_of_mask8bit_; ++i)
        mask8bit_[i] = 0xff;
}             
auto g80::TextImage::clear_all_mask8bit() -> void {
    for (Dim i = 0; i < size_of_mask8bit_; ++i)
        mask8bit_[i] = 0x00;
}

auto g80::TextImage::show() const -> void {
    static const std::string color[] { "\033[30m", "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m", "\033[37m" };
    static const size_t sizeof_color = sizeof(color) / sizeof(std::string);
    std::stringstream output;
    
    output << "\033[2J";
    size_t prev_color = sizeof_color;
    for (Dim i = 0; i < area_(); ++i) {

        if (prev_color != color_[i]) {
            prev_color = color_[i];
            output << color[prev_color];
        }

        if (i % area_.w == 0 && i > 0)
            output << "\n"; 
        
        output << text_[i];
    }

    output << "\033[0m\n";
    std::cout << output.str();
}

auto g80::TextImage::show_text() const -> void {
    std::stringstream output;
    
    // Clear Screen
    output << "\033[2J";

    // Text content
    for (Dim i = 0; i < area_(); ++i) {
        if (i % area_.w == 0 && i > 0)
            output << "\n"; 

        output << text_[i];
    }

    // Reset Defaults
    output << "\033[0m\n";

    // Output to console
    std::cout << output.str();
}

auto g80::TextImage::show_color() const -> void {
    std::stringstream output;
    
    // Clear Screen
    output << "\033[2J";

    // Text content
    for (Dim i = 0; i < area_(); ++i) {
        if (i % area_.w == 0 && i > 0)
            output << "\n"; 

        output << std::setw(3) << std::hex << static_cast<int>(color_[i]);
    }

    // Reset Defaults
    output << "\033[0m\n";

    // Output to console
    std::cout << output.str();
}

auto g80::TextImage::show_mask(Dim marker) const -> void {
    std::stringstream output;
    
    Mask8bit mask = 0x01;
    for (Dim i = 0; i < area_.w; ++i) 
        output << i % 10;
    output << "\n";
    
    for (Dim i = 0; i < area_(); ++i) {
        if (i % area_.w == 0 && i > 0)
            output << "\n"; 
        
        if (i == marker)
            output << "_";
        else
            output << ((mask8bit_[i / 8] & mask) ? "1" : ".");
        
        mask <<= 1;
        if (!mask) 
            mask = 0x01;
    }

    output << "\033[0m\n\n";
    std::cout << output.str();
}

auto g80::TextImage::show_mask_value() const -> void {

    for (Dim i = 0; i < size_of_mask8bit_; ++i) {
        if (mask8bit_[i])
            std::cout << "i: " << i << " - " << (Dim)mask8bit_[i] << "\n";
    }
    std::cout << std::endl;
}

auto g80::TextImage::index(const Point &point) const -> Dim {
    return point.y * area_.w + point.x;
}
            
auto g80::TextImage::set_text(const Point &point, const Text &text) -> void {
    set_text(index(point), text);
}
     
auto g80::TextImage::set_text(const Dim &ix, const Text &text) -> void {
    text_[ix] = text;
}

auto g80::TextImage::get_text(const Point &point) const -> Text {
    return get_text(index(point));
}

auto g80::TextImage::get_text(const Dim &ix) const -> Text {
    return text_[ix];
}

auto g80::TextImage::set_color(const Point &point, const Color &color) -> void {
    set_color(index(point), color);
}
     
auto g80::TextImage::set_color(const Dim &ix, const Color &color) -> void {
    color_[ix] = color;
}

auto g80::TextImage::get_color(const Point &point) const -> Color {
    return get_color(index(point));
}

auto g80::TextImage::get_color(const Dim &ix) const -> Color {
    return color_[ix];
}

auto g80::TextImage::set_mask(const Point &point, MASK_BIT mask_bit) -> void {
    set_mask(index(point), mask_bit);
}
     
auto g80::TextImage::set_mask(const Dim &ix, MASK_BIT mask_bit) -> void {
    Dim offset = ix % 8;
    Mask8bit and_mask = ~(1 << offset);
    Mask8bit or_mask = mask_bit << offset;
    mask8bit_[ix / 8] &= and_mask;
    mask8bit_[ix / 8] |= or_mask;
}

auto g80::TextImage::get_mask(const Point &point) const -> MASK_BIT {
    return get_mask(index(point));
}

auto g80::TextImage::get_mask(const Dim &ix) const -> MASK_BIT {
    Dim offset = ix % 8;
    Mask8bit value = mask8bit_[ix / 8];
    value = mask8bit_[ix / 8] & (1 << offset);
    return value ? ON : OFF;
}

auto g80::TextImage::get_image(const Rect &rect) const -> TextImage {
    TextImage dest_text_image(rect.a, OFF);
    Text *text_ptr = dest_text_image.raw_text().get();
    Color *color_ptr = dest_text_image.raw_color().get();

    Dim start = index(rect.p);
    for (Dim row = 0; row < rect.a.h; ++row) {
        Dim ix = start + row * area_.w;
        memcpy(text_ptr, &text_[ix], sizeof(Text) * rect.a.w);
        memcpy(color_ptr, &color_[ix], sizeof(Color) * rect.a.w);
        text_ptr += rect.a.w;
        color_ptr += rect.a.w;
    }

    // Copy mask in chunks of 8-bits
    // which is faster by a factor of 3 than 
    // copying mask bit by bit
    Mask8bit *mask8bit_ptr = dest_text_image.raw_mask8bit().get();
    Dim total_copied = 0;
    for (Dim row = 0; row < rect.a.h; ++row) {
        Dim ix = start + row * area_.w;
        Dim size = rect.a.w;
        do {
            Dim init_offset = total_copied % 8;
            Dim s = 8 - total_copied % 8;
            s = s > size ? size : s;
            Mask8bitOp mask = get_mask8bit_value(ix, s, init_offset);
            mask8bit_ptr[total_copied / 8] |= mask.value();
            total_copied += s;
            ix += s;
            size -= s;
        } while(size > 0);
    }

    return dest_text_image;
}

auto g80::TextImage::put_image(const TextImage &text_image, const Point &point) -> void {
    for (Dim r = 0; r < text_image.area_.h; ++r) {
        Dim tix = index({point.x, static_cast<Dim>(point.y + r)}); 
        for (Dim six = text_image.index({0, r}), sixm = six + text_image.area_.w; six < sixm; ++six) {           
            text_[tix] = text_image.craw_text().get()[six];
            color_[tix] = text_image.craw_color().get()[six];
            ++tix;
        }
    }
}

auto g80::TextImage::and_image(const TextImage &text_image, const Point &point) -> void {
    for (Dim r = 0; r < text_image.area_.h; ++r) {
        Dim tix = index({point.x, static_cast<Dim>(point.y + r)}); 
        for (Dim six = text_image.index({0, r}), sixm = six + text_image.area_.w; six < sixm; ++six) {           
            if ((get_mask(tix) & text_image.get_mask(six)) == ON) {
                text_[tix] = text_image.craw_text().get()[six];
                color_[tix] = text_image.craw_color().get()[six];
            }
            ++tix;
        }
    }
}

auto g80::TextImage::or_image(const TextImage &text_image, const Point &point) -> void {
    for (Dim r = 0; r < text_image.area_.h; ++r) {
        Dim tix = index({point.x, static_cast<Dim>(point.y + r)}); 
        for (Dim six = text_image.index({0, r}), sixm = six + text_image.area_.w; six < sixm; ++six) {           
            if ((get_mask(tix) | text_image.get_mask(six)) == ON) {
                text_[tix] = text_image.craw_text().get()[six];
                color_[tix] = text_image.craw_color().get()[six];
            }
            ++tix;
        }
    }
}

auto g80::TextImage::xor_image(const TextImage &text_image, const Point &point) -> void {
    for (Dim r = 0; r < text_image.area_.h; ++r) {
        Dim tix = index({point.x, static_cast<Dim>(point.y + r)}); 
        for (Dim six = text_image.index({0, r}), sixm = six + text_image.area_.w; six < sixm; ++six) {           
            if ((get_mask(tix) ^ text_image.get_mask(six)) == ON) {
                text_[tix] = text_image.craw_text().get()[six];
                color_[tix] = text_image.craw_color().get()[six];
            }
            ++tix;
        }
    }
}

auto g80::TextImage::put_text_color(const String &text, const Color &color, const Point &point) -> void {
    Dim start = index(point);
    for (int i = 0; i < text.size(); ++i) {
        text_[(start + i) % area_()] = text[i];
        color_[(start + i) % area_()] = color;
    }
}

auto g80::TextImage::create_mask_if_color(const Color &color) -> void {
    for (Dim i = 0; i < area_(); ++i)
        set_mask(i, color_[i] == color ? ON : OFF);
}

auto g80::TextImage::create_mask_if_text(const Text &text) -> void {
    for (Dim i = 0; i < area_(); ++i)
        set_mask(i, text_[i] == text ? ON : OFF);
}

auto g80::TextImage::invert_mask() -> void {
    for (Dim i = 0; i < size_of_mask8bit_; ++i)
        mask8bit_[i] = ~mask8bit_[i];
}

auto g80::TextImage::get_mask8bit_value(const Dim &ix, const Dim &size, const Dim &init_offset) const -> Mask8bitOp {
    if (size == 0 || size > 8) 
        return {};

    Dim ix8 = ix / 8;
    Dim nix8 = (ix + size - 1) / 8;
    Dim offset = ix % 8 - init_offset;

    Mask8bit value;
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

auto g80::TextImage::save(const String &filename) const -> void {
    std::ofstream file (filename, std::ios::binary);
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    // Save the dimensions
    file.write(static_cast<const char *>(static_cast<const void*>(&area_)), sizeof(area_.w) + sizeof(area_.h));

    // Save color, text and mask
    file.write(static_cast<const char *>(static_cast<const void*>(color_.get())), area_());
    file.write(static_cast<const char *>(static_cast<const void*>(text_.get())), area_());
    file.write(static_cast<const char *>(static_cast<const void*>(mask8bit_.get())), size_of_mask8bit_);

}

auto g80::TextImage::load(const String &filename) -> void {
    std::ifstream file (filename, std::ios::binary);
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    file.read(static_cast<char *>(static_cast<void *>(&area_.w)), sizeof(area_.w));
    file.read(static_cast<char *>(static_cast<void *>(&area_.h)), sizeof(area_.h));
    size_of_mask8bit_ = area_() % 8 == 0 ? area_() / 8 : area_() / 8 + 1;

    color_.reset(new Color[area_()]);
    file.read(static_cast<char *>(static_cast<void *>(color_.get())), area_());

    text_.reset(new Text[area_()]);            
    file.read(static_cast<char *>(static_cast<void *>(text_.get())), area_());

    mask8bit_.reset(new Mask8bit[size_of_mask8bit_]);           
    file.read(static_cast<char *>(static_cast<void *>(mask8bit_.get())), size_of_mask8bit_);
}

// Translate Functions
auto g80::TextImage::xlat_shift_left(Dim shift, TextImageAttribute tia, const Text &default_text, const Color &default_color, const MASK_BIT &default_mask_bit) -> void {
    
    if (tia & TEXT) {
        for (Dim i = 0; i < area_() - shift; ++i)
            text_[i] = text_[i + shift];

        for (Dim i = area_() - shift; i < area_(); ++i)
            text_[i] = default_text;
    }
    if (tia & COLOR) {
        for (Dim i = 0; i < area_() - shift; ++i)
            color_[i] = color_[i + shift];

        for (Dim i = area_() - shift; i < area_(); ++i)
            color_[i] = default_color;
    }
    if (tia & MASK) {
        for (Dim i = 0; i < area_() - shift; ++i)
            set_mask(i, get_mask(i + shift));

        for (Dim i = area_() - shift; i < area_(); ++i)
           set_mask(i, default_mask_bit);      
    }
}

auto g80::TextImage::xlat_shift_right(Dim shift, TextImageAttribute tia, const Text &default_text, const Color &default_color, const MASK_BIT &default_mask_bit) -> void {
    
    if (tia & TEXT) {
        for (Dim i = area_() - 1; i >= shift; --i)
            text_[i] = text_[i - shift];

        for (Dim i = 0; i < shift; ++i)
            text_[i] = default_text;
    }

    if (tia & COLOR) {
        for (Dim i = area_() - 1; i >= shift; --i)
            color_[i] = color_[i - shift];

        for (Dim i = 0; i < shift; ++i)
            color_[i] = default_color;
    }

    if (tia & MASK) {
        for (Dim i = area_() - 1; i >= shift; --i)
            set_mask(i, get_mask(i - shift));

        for (Dim i = 0; i < shift; ++i)
            set_mask(i, default_mask_bit);
    }
}
auto g80::TextImage::xlat_reverse(Dim start, Dim end, TextImageAttribute tia) -> void {

    if (tia & TEXT) {
        int i = start, j = end;
        while (i < j)
            std::swap(text_[i++], text_[j--]);
    }

    if (tia & COLOR) {
        int i = start, j = end;
        while (i < j)
            std::swap(color_[i++], color_[j--]);
    }

    if (tia & MASK) {
        int i = start, j = end;
        while (i < j) {
            MASK_BIT t = get_mask(i);
            set_mask(i, get_mask(j));
            set_mask(j, t);
            ++i;
            --j;
        }
    }
}

auto g80::TextImage::xlat_rotate_left(Dim rotate, TextImageAttribute tia) -> void {
    Dim r = rotate % area_.w;
    if (r > 0) {
        xlat_reverse(0, r - 1, tia);
        xlat_reverse(r, area_() - 1, tia);
        xlat_reverse(0, area_() - 1, tia);
    }
}

auto g80::TextImage::xlat_rotate_right(Dim rotate, TextImageAttribute tia) -> void {
    Dim r = rotate % area_.w;
    if (r > 0) {
        xlat_reverse(0, area_() - 1, tia);
        xlat_reverse(0, r - 1, tia);
        xlat_reverse(r, area_() - 1, tia);
    }
}

auto g80::TextImage::xlat_flip_horizontal(TextImageAttribute tia) -> void {
    for (Dim i = 0; i < area_.h; ++i) {
        Dim j = index({0, i});
        xlat_reverse(j, j + area_.w - 1, tia);
    }
}

auto g80::TextImage::xlat_flip_vertical(TextImageAttribute tia) -> void {

    if (tia & TEXT) {
        Dim i = 0;
        Dim j = area_.h - 1;
        while (i < j) {
            Dim k = index({0, i++});
            Dim kmax = k + area_.w;
            Dim l = index({0, j--});
            while (k < kmax)
                std::swap(text_[k++], text_[l++]);
        }
    }

    if (tia & COLOR) {
        Dim i = 0;
        Dim j = area_.h - 1;
        while (i < j) {
            Dim k = index({0, i++});
            Dim kmax = k + area_.w;
            Dim l = index({0, j--});
            while (k < kmax)
                std::swap(color_[k++], color_[l++]);
        }
    }

    if (tia & MASK) {
        Dim i = 0;
        Dim j = area_.h - 1;
        while (i < j) {
            Dim k = index({0, i++});
            Dim kmax = k + area_.w;
            Dim l = index({0, j--});
            while (k < kmax) {
                MASK_BIT t = get_mask(l);
                set_mask(l, get_mask(k));
                set_mask(k, t);
                ++l;
                ++k; 
            }
                
        }
    }
}

auto g80::TextImage::gfx_point(const Point &point, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void {
    Dim ix = index(point);
    text_[ix] = text;
    color_[ix] = color;
    set_mask(ix, mask_bit);
}

auto g80::TextImage::gfx_line(const Point &point1, const Point &point2, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void {
    Dim dx = point2.x - point1.x;
    Dim dy = point2.y - point1.y;
    Dim sdx = dx < 0 ? -1 : 1;
    Dim sdy = dy < 0 ? -area_.w : area_.w;
    Dim adx = dx < 0 ? dx * -1 : dx;
    Dim ady = dy < 0 ? dy * -1 : dy;
    Dim curr_point = index(point1);
    if (adx >= ady) {    
        for (Dim i = 0, t = ady; i <= adx; ++i, t += ady) {
            text_[curr_point] = text;
            color_[curr_point] = color;
            set_mask(curr_point, mask_bit);
            if (t >= adx) {
                curr_point += sdy;
                t -= adx;
            }
            curr_point +=sdx;
        }
    } else {
        for (Dim i = 0, t = adx; i <= ady; ++i, t += adx) {
            text_[curr_point] = text;
            color_[curr_point] = color;
            set_mask(curr_point, mask_bit);
            if (t >= ady) {
                curr_point += sdx;
                t -= ady;
            }
            curr_point += sdy;
        }
    }
}

auto g80::TextImage::gfx_circle(const Point &point, const Dim &radius, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void {
    
    Dim center_point = index(point);

    Dim x = radius;
    Dim y = 0;

    Dim bx = x * area_.w;
    Dim by = y * area_.w;

    Dim dx = 1 - (radius << 1);
    Dim dy = 1;
    Dim re = 0;

    while (x >= y)
    {
        // 1st Octant
        text_[center_point + x - by] = text;
        color_[center_point + x - by] = color;
        set_mask(center_point + x - by, mask_bit);
        
        // The rest of the octant
        text_[center_point + y - bx] = text;
        text_[center_point - y - bx] = text;
        text_[center_point - x - by] = text;
        text_[center_point + x + by] = text;
        text_[center_point + y + bx] = text;
        text_[center_point - y + bx] = text;
        text_[center_point - x + by] = text;

        color_[center_point + y - bx] = color;
        color_[center_point - y - bx] = color;
        color_[center_point - x - by] = color;
        color_[center_point + x + by] = color;
        color_[center_point + y + bx] = color;
        color_[center_point - y + bx] = color;
        color_[center_point - x + by] = color;

        set_mask(center_point + y - bx, mask_bit);
        set_mask(center_point - y - bx, mask_bit);
        set_mask(center_point - x - by, mask_bit);
        set_mask(center_point + x + by, mask_bit);
        set_mask(center_point + y + bx, mask_bit);
        set_mask(center_point - y + bx, mask_bit);
        set_mask(center_point - x + by, mask_bit);

        ++y;
        re += dy;
        dy += 2;
        if ((re << 1) + dx > 0)
        {
            --x;
            bx -= area_.w;
            re += dx;
            dx += 2;
        }
        by += area_.w;
    }
}

auto g80::TextImage::gfx_fill_with_text_border(const Point &point, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void {

    // The preferential method of fill is always stack over recursion, to prevent stackoverflow
    // 
    // The max stack allocation based on observation is (col - 1) x (row - 1) + 1
    // Since this is known, it is better to use a std::vector instead of std::stack
    // to represent a stack

    std::vector<Point> points((point.x - 1) * (point.y - 1) + 1);
    int si = -1;

    if (text_[index(point)] != text)
        points[++si] = point;

    while (si >= 0) {
        Point p = points[si--];

        Dim ix = index(p);
        text_[ix] = text;
        color_[ix] = color;
        set_mask(ix, mask_bit);

        if (p.y - 1 >= 0 && text_[index({p.x, static_cast<Dim>(p.y - 1)})] != text)
            points[++si] = {p.x, static_cast<Dim>(p.y - 1)};

        if (p.y + 1 < area_.h && text_[index({p.x, static_cast<Dim>(p.y + 1)})] != text)
            points[++si] = {p.x, static_cast<Dim>(p.y + 1)};

        if (p.x - 1 >= 0 && text_[index({static_cast<Dim>(p.x - 1), p.y})] != text)
            points[++si] = {static_cast<Dim>(p.x - 1), p.y};

        if (p.x + 1 < area_.w && text_[index({static_cast<Dim>(p.x + 1), p.y})] != text)
            points[++si] = {static_cast<Dim>(p.x + 1), p.y};
    }
}

auto g80::TextImage::gfx_fill_with_color_border(const Point &point, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void {

    // The preferential method of fill is always stack over recursion, to prevent stackoverflow
    // 
    // The max stack allocation based on observation is (col - 1) x (row - 1) + 1
    // Since this is known, it is better to use a std::vector instead of std::stack
    // to represent a stack

    std::vector<Point> points((point.x - 1) * (point.y - 1) + 1);
    int si = -1;

    if (color_[index(point)] != color)
        points[++si] = point;

    while (si >= 0) {
        Point p = points[si--];

        Dim ix = index(p);
        text_[ix] = text;
        color_[ix] = color;
        set_mask(ix, mask_bit);

        if (p.y - 1 >= 0 && color_[index({p.x, static_cast<Dim>(p.y - 1)})] != color)
            points[++si] = {p.x, static_cast<Dim>(p.y - 1)};

        if (p.y + 1 < area_.h && color_[index({p.x, static_cast<Dim>(p.y + 1)})] != color)
            points[++si] = {p.x, static_cast<Dim>(p.y + 1)};

        if (p.x - 1 >= 0 && color_[index({static_cast<Dim>(p.x - 1), p.y})] != color)
            points[++si] = {static_cast<Dim>(p.x - 1), p.y};

        if (p.x + 1 < area_.w && color_[index({static_cast<Dim>(p.x + 1), p.y})] != color)
            points[++si] = {static_cast<Dim>(p.x + 1), p.y};
    }
}


auto g80::TextImage::gfx_fill_with_mask_border(const Point &point, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void {

    // The preferential method of fill is always stack over recursion, to prevent stackoverflow
    // 
    // The max stack allocation based on observation is (col - 1) x (row - 1) + 1
    // Since this is known, it is better to use a std::vector instead of std::stack
    // to represent a stack

    std::vector<Point> points((point.x - 1) * (point.y - 1) + 1);
    int si = -1;

    if (get_mask(index(point)) != mask_bit)
        points[++si] = point;

    while (si >= 0) {
        Point p = points[si--];

        Dim ix = index(p);
        text_[ix] = text;
        color_[ix] = color;
        set_mask(ix, mask_bit);

        if (p.y - 1 >= 0 && get_mask(index({p.x, static_cast<Dim>(p.y - 1)})) != mask_bit)
            points[++si] = {p.x, static_cast<Dim>(p.y - 1)};

        if (p.y + 1 < area_.h && get_mask(index({p.x, static_cast<Dim>(p.y + 1)})) != mask_bit)
            points[++si] = {p.x, static_cast<Dim>(p.y + 1)};

        if (p.x - 1 >= 0 && get_mask(index({static_cast<Dim>(p.x - 1), p.y})) != mask_bit)
            points[++si] = {static_cast<Dim>(p.x - 1), p.y};

        if (p.x + 1 < area_.w && get_mask(index({static_cast<Dim>(p.x + 1), p.y})) != mask_bit)
            points[++si] = {static_cast<Dim>(p.x + 1), p.y};
    }
}

