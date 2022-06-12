# Text Image

Text Image is a simple C++17 class to handle console text graphics. Applications of which ranges from text console games and other software that requires text console animation.

This repo also has a Text Video Animation, which is a supplementary class to Text Image if you need to perform animations and games. 

```c++
namespace g80 {

    // Types Used
    using color = uint8_t;
    using text = uint8_t;
    using mask8bit = uint8_t;
    using uptr_color = std::unique_ptr<color[]>;
    using uptr_text = std::unique_ptr<text[]>;
    using uptr_mask8bit = std::unique_ptr<mask8bit[]>;
    enum mask_bit {OFF = 0x00, ON = 0x01};
    enum text_image_attrib {TEXT = 1, COLOR = 2, MASK = 4, ALL = 7}; 

    // The text_image class template
    template<typename int_type, typename uint_type>
    class text_image {

    // Instance Vars
    protected:
        uint_type w_, h_, size_;
        uptr_color color_{nullptr};
        uptr_text text_{nullptr};
        uint_type size_of_mask8bit_{0};
        uptr_mask8bit mask8bit_{nullptr};     

    // Constructors and Assignments
    public:
        text_image() = default;
        text_image(const std::string &filename);
        text_image(const std::string &t, const color c, const mask_bit m = ON);
        text_image(const int_type w, const int_type h, const color c = 7, const text t = ' ', const mask_bit m = ON);
        text_image(const text_image &rhs);
        text_image(text_image &&rhs);
        auto operator=(const text_image &rhs) -> text_image &;
        auto operator=(text_image &&rhs) -> text_image &;

    // Protected property getters
    public:
        inline auto width() const -> uint_type;
        inline auto height() const -> uint_type;
        inline auto size() const -> uint_type;
        inline auto size_mask8bit() const -> uint_type;
        inline auto raw_color() -> uptr_color &;
        inline auto craw_color() const -> const uptr_color &;
        inline auto raw_text() -> uptr_text &;
        inline auto craw_text() const -> const uptr_text &;
        inline auto raw_mask8bit() -> uptr_mask8bit &;
        inline auto craw_mask8bit() const -> const uptr_mask8bit &;

    // Debuggers
    public:
        auto show_color() const -> void;
        auto show_text() const -> void;
        auto show_mask(int_type marker) const -> void;
        auto show_mask_value() const -> void;

    // Getters for Colors, Texts and Masks    
    public:
        inline auto set_text(const uint_type &i, const text t) -> void;
        inline auto set_text(const int_type x, const int_type y, const text t) -> void;
        inline auto get_text(const uint_type i) const -> text;
        inline auto get_text(const int_type x, const int_type y) const -> text;
        auto fill_text(const text t) -> void;
        auto fill_text(const std::string &s) -> void;
        inline auto set_color(const uint_type i, const color c) -> void;
        inline auto set_color(const int_type x, const int_type y, const color c) -> void;
        inline auto get_color(const uint_type ix) const -> color;
        inline auto get_color(const int_type x, const int_type y) const -> color;
        auto fill_color(const color c) const -> void;
        inline auto set_mask(const uint_type i, mask_bit m) -> void;
        inline auto set_mask(const int_type x, const int_type y, mask_bit m) -> void;
        inline auto get_mask(const uint_type &ix) const -> mask_bit;
        inline auto get_mask(const int_type x, int_type y) const -> mask_bit;
        auto set_all_mask8bit() -> void;
        auto clear_all_mask8bit() -> void;
        auto create_mask_if_color(const color c) -> void;
        auto create_mask_if_text(const text &t) -> void;
        auto invert_mask() -> void;
        auto put_text_color(const int_type x, const int_type y, const std::string &t, const color c) -> void;

    // Image get and put
    public:
        auto get_image(const int_type x, const int_type y, const uint_type w, const uint_type h) const -> text_image;

    public:
        auto put_image(const int_type x, const int_type y, const text_image &timg) -> void;
        auto and_image(const int_type x, const int_type y, const text_image &timg) -> void;
        auto or_image(const int_type x, const int_type y, const text_image &timg) -> void;
        auto xor_image(const int_type x, const int_type y, const text_image &timg) -> void;

    // Text Image Transactions 
    public:
        auto xlat_shift_left(uint_type shift, const text_image_attrib tia = ALL, const text &default_text = ' ', const color &default_color = 0, const mask_bit &default_mask_bit = OFF) -> void;
        auto xlat_shift_right(uint_type shift, const text_image_attrib tia = ALL, const text &default_text = ' ', const color &default_color = 0, const mask_bit &default_mask_bit = OFF) -> void;
        auto xlat_reverse(const uint_type start, const uint_type end, const text_image_attrib tia = ALL) -> void;
        auto xlat_rotate_left(const uint_type rotate, const text_image_attrib tia) -> void;
        auto xlat_rotate_right(const uint_type rotate, const text_image_attrib tia) -> void;
        auto xlat_flip_horizontal(const text_image_attrib tia) -> void;
        auto xlat_flip_vertical(const text_image_attrib tia) -> void;

    // Draw Point 
    public:
        auto gfx_point(const int_type x, const int_type y, const text t, const color c, const mask_bit m) -> void;

    // Draw Line using color, text or mask
    public:
        auto gfx_line_color(const int_type x1, const int_type y1, const int_type x2, const int_type y2, const color c) -> void;
        auto gfx_line_text(const int_type x1, const int_type y1, const int_type x2, const int_type y2, const text t) -> void;
        auto gfx_line_mask(const int_type x1, const int_type y1, const int_type x2, const int_type y2, const mask_bit m) -> void;
        auto gfx_line(const int_type x1, const int_type y1, const int_type x2, const int_type y2, const color c, const text t, const mask_bit m) -> void;

    // Draw Circle using color, text or mask
    public:
        auto gfx_circle_color(const int_type cx, const int_type cy, const int_type r, const color c) -> void;
        auto gfx_circle_text(const int_type cx, const int_type cy, const int_type r, const text t) -> void;
        auto gfx_circle_mask(const int_type cx, const int_type cy, const int_type r, const mask_bit m) -> void;
        auto gfx_circle(const int_type cx, const int_type cy, const int_type r, const color c, const text t, const mask_bit m) -> void;

    // Draw Arc using color, text or mask
    public:
        auto gfx_arc_color(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const color c) -> void;
        auto gfx_arc_text(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const text t) -> void;
        auto gfx_arc_mask(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const mask_bit m) -> void;
        auto gfx_arc(const int_type cx, const int_type cy, const int_type r, const int_type sa, const int_type ea, const color c, const text t, const mask_bit m) -> void;
    
    // Fill an area using color, text or mask
    public:
        auto gfx_fill_color(const int_type x, const int_type y, const color c) -> void;
        auto gfx_fill_text(const int_type x, const int_type y, const text t) -> void;
        auto gfx_fill_mask(const int_type x, const int_type y, const mask_bit m) -> void;
        auto gfx_fill_with_text_border(const int_type x, const int_type y, const color c, const text t, const mask_bit m) -> void;

    // Misc Helpers
    public:
        inline auto ix(const int_type x, const int_type y) const -> uint_type;
        auto save(const std::string &filename) const -> void;
        auto load(const std::string &filename) -> void;
        auto show() const -> void;
    };
}

```

How to use it:
---

This section is a work-in-progress.

```c++
/*

1. Choose the right integral type for your project:
--

Since we're working with text graphics, a screen text resolution of 130 x 40 would already occupy most the normal
screens. Therefore, a smaller integral type like int16_t or uint16_t would suffice. Unless you're working with large
text graphics you may choose 32 - 64 bit integers.

*/

```

Some things you can do with TextImage Class
---

Game of Life Simulation
---
![alt text](https://github.com/everettvergara/TextImage/blob/main/assets/Game%20of%20life.png)

Waving Flag Animation
---
![alt text](https://github.com/everettvergara/TextImage/blob/main/assets/PinoyFlag.png)
