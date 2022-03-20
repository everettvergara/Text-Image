# TextImage

TextImage is a simple C++17 Compatible Class to handle console buffering. Applications of which ranges from text console games and other software that requires text console animation. I have yet to create a wiki page for this repo.

For reference, the class has the following functions:

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
            auto set_text(const Dim &col, const Dim &row, const Text &text) -> void;
            auto set_text(const Dim &ix, const Text &text) -> void;
            auto get_text(const Dim &col, const Dim &row) const -> Text;
            auto get_text(const Dim &ix) const -> Text;
            auto show_text() const -> void;
            auto fill_text(const Text &text) -> void;
            auto fill_text(const String &text) -> void;

            // Color functions
            auto set_color(const Dim &col, const Dim &row, const Color &color) -> void;
            auto set_color(const Dim &ix, const Color &color) -> void;
            auto get_color(const Dim &col, const Dim &row) const -> Color;
            auto get_color(const Dim &ix) const -> Color;
            auto show_color() const -> void;
            auto fill_color(const Color &color) const -> void;

            // Mask functions
            auto set_mask(const Dim &col, const Dim &row, MASK_BIT mask_bit) -> void;
            auto set_mask(const Dim &ix, MASK_BIT mask_bit) -> void;
            auto get_mask(const Dim &col, const Dim &row) const -> MASK_BIT;
            auto get_mask(const Dim &ix) const -> MASK_BIT;
            auto set_all_mask8bit() -> void;  
            auto clear_all_mask8bit() -> void;
            auto create_mask_if_color(const Color &color) -> void;
            auto create_mask_if_text(const Text &text) -> void;
            auto invert_mask() -> void;
            auto show_mask(Dim marker = 0xffff) const -> void;
            auto show_mask_value() const -> void;

            // Text Image functions
            auto get_image(const Rect &rect) const -> TextImage;
            auto put_image(const TextImage &text_image, const Dim &col, const Dim &row) -> void;
            auto and_image(const TextImage &text_image, const Dim &col, const Dim &row) -> void;
            auto or_image(const TextImage &text_image, const Dim &col, const Dim &row) -> void;
            auto xor_image(const TextImage &text_image, const Dim &col, const Dim &row) -> void;
            auto put_text_color(const String &text, const Color &color, const Dim &col, const Dim &row) -> void;
            auto show() const -> void;

            // Translate Functions
            // TODO: TEST xlat
            auto xlat_shift_left(Dim shift, TextImageAttribute tia, const Text &default_text = ' ', const Color &default_color = 0, const MASK_BIT &default_mask_bit = OFF) -> void;
            auto xlat_shift_right(Dim shift, TextImageAttribute tia, const Text &default_text = ' ', const Color &default_color = 0, const MASK_BIT &default_mask_bit = OFF) -> void;
            auto xlat_reverse(Dim start, Dim end, TextImageAttribute tia) -> void;
            auto xlat_rotate_left(Dim rotate, TextImageAttribute tia) -> void;
            auto xlat_rotate_right(Dim rotate, TextImageAttribute tia) -> void;
            auto xlat_flip_horizontal(TextImageAttribute tia) -> void;
            auto xlat_flip_vertical(TextImageAttribute tia) -> void;

            // Graphics
            auto gfx_point(const Dim &col, const Dim &row, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            auto gfx_line(const Dim &col1, const Dim &row1, const Dim &col2, const Dim &row2, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            auto gfx_circle(const Dim &col, const Dim &row, const Dim &radius, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            auto gfx_fill_with_text_border(const Dim &col, const Dim &row, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            auto gfx_fill_with_color_border(const Dim &col, const Dim &row, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;
            auto gfx_fill_with_mask_border(const Dim &col, const Dim &row, const Text &text, const Color &color, const MASK_BIT &mask_bit) -> void;

            // Save and Load
            auto save(const String &filename) const -> void;
            auto load(const String &filename) -> void;
            
            // Helper functions
            auto index(const Dim &col, const Dim &row) const -> Dim;

        protected:
            Area area_{0,0};
            Uptr_color color_{nullptr};
            Uptr_text text_{nullptr};
            Sizeof_mask8bit size_of_mask8bit_{0};
            Uptr_mask8bit mask8bit_{nullptr};

        private:
            auto get_mask8bit_value(const Dim &ix, const Dim &size, const Dim &init_offset = 0) const -> Mask8bitOp;

    };
