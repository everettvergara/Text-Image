/**
 * @file index.hpp
 * @author Everett Gaius S. Vergara (me@everettgaius.com)
 * @brief A fast but expensive datastruct for hashtables 
 * @version 0.1
 * @date 2022-06-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef INDEX_HPP
#define INDEX_HPP

#include <array>

/**
 *              0   1   2   3   4   ....    n-1
 * ix_bin:      4   2   n-1 50  
 * ix                       ^
 * 
 * bin_loc:     ?    ?   1  ?                2
 * 
 */

namespace g80 {

    template<typename uint_type, uint_type N>
    class index {
    public:
        index() {reset();}

        auto reset() -> void {
            bin_ptr_ = {invalid_ptr_};
            memset(bin_loc_.data(), ~0, sizeof(uint_type) * N);
        }

        auto add_to_bin(uint_type ix) -> bool {
            ix_bin_[++bin_ptr_] = ix;
            bin_loc[ix] = bin_ptr_;
            return bin_ptr_ != invalid_ptr_;
        }

        auto remove_from_bin(uint_type ix) -> void {
            std::swap(ix_bin_[bin_loc_[ix]], ix_bin_[bin_ptr_--]);
            bin_loc_[ix] = invalid_ptr_;
        }

        inline auto size() -> uint_type {
            return bin_ptr_ + 1;
        }

        inline auto get_ix_bin() const -> const std::array<uint_type, N> {return ix_bin_;}
        inline auto bin_loc() const -> const std::array<uint_type, N> {return bin_loc_;}

        // TODO: Add forward iterator for list of available indexes in the bin

    private:
        uint_type invalid_ptr_{~static_cast<uint_type>(0);};
        std::array<uint_type, N> ix_bin_;
        std::array<uint_type, N> bin_loc_;
        uint_type bin_ptr_{invalid_ptr_};
        
    };
}

#endif