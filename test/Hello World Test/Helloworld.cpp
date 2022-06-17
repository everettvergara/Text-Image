#include <iostream>
#include <map>
#include <string>
#include <functional>

struct Point {int16_t x, y;};
using namespace std::placeholders;

class Buffer {
public:
// TODO: Fix bug wherein do not draw if no prev x,y
    Buffer(const uint16_t w, const uint16_t h) : w_(w + 1), out_(w_ * h, ' ') {
        for (uint16_t i = w; i < h * w_; i += w_) out_[i] = '\n';
        exps_['r'] = std::bind(&Buffer::r, this, _1);
        exps_['l'] = std::bind(&Buffer::l, this, _1);
        exps_['d'] = std::bind(&Buffer::d, this, _1);
        exps_['u'] = std::bind(&Buffer::u, this, _1);
        exps_['x'] = std::bind(&Buffer::x, this, _1);
        exps_['y'] = std::bind(&Buffer::y, this, _1);
    }
    inline auto ix_pt() -> uint16_t {uint16_t t = pt_.y * w_ + pt_.x; return t >= out_.size() ? t % out_.size() : t;}
    inline auto x(const uint16_t x) -> bool {if (!x) return false; pt_.x = x; return true;};
    inline auto y(const uint16_t y) -> bool {if (!y) return false; pt_.y = y; return true;};
    inline auto addx(const int16_t n) -> void {pt_.x += n;}
    inline auto addy(const int16_t n) -> void {pt_.y += n;}
    inline auto looper(int16_t c, void (Buffer::*adder)(const int16_t), int16_t n) -> bool {
        if (!c) return false;
        while (c-- > 0) {out_[ix_pt()] = '*'; (this->*adder)(n);} 
        return true;
    }
    auto r(int16_t c) -> bool {return looper(c, &Buffer::addx, +1);} 
    auto l(int16_t c) -> bool {return looper(c, &Buffer::addx, -1);} 
    auto d(int16_t c) -> bool {return looper(c, &Buffer::addy, +1);} 
    auto u(int16_t c) -> bool {return looper(c, &Buffer::addy, -1);} 
    auto evaluate(const char exp, int16_t c) -> bool {
        auto f = exps_.find(exp);
        if (f != exps_.end()) return (f->second)(c);
        return false;
    };
    auto print() {std::cout << out_;}

private:
    const uint16_t w_;
    Point pt_{0, 0};
    std::string out_;
    std::map<char, std::function<bool(int16_t)>> exps_;
};

int main() {
    const std::string hello_world {
        "x1y1d5x1y3r3d5x6y3d4r4x6y3r4d2l4x12y1d7r2x15y1d7r2x17y2r4d5l3u5"
        "x23y4d4r3u3d3r3u5x30y1r4d5l3u5x36y2d4u4r4x41y1d7r2x48y2d5l4u3r3"};
    Buffer buffer(100, 10);
    uint16_t i = 0;
    while(i < hello_world.size()) {
        char exp {hello_world[i++]};
        int16_t c = 0;
        while (hello_world[i] >= '0' && hello_world[i] <= '9') {c *= 10; c += hello_world[i++] - '0';}
        if (!buffer.evaluate(exp, c)) break;
    }
    buffer.print();
}