/**
 * @file index_bin.hpp
 * @author Everett Gaius S. Vergara (me@everettgaius.com)
 * @brief A fast but expensive datastruct for hashtables 
 * @version 0.1
 * @date 2022-06-10
 * 
 * @copyright Copyright (c) 2022
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 
 * @note: 
 * 
 * Index Bin is a datastructure concept wherein you have an N-sized vector
 * paritioned into used and unused indexes. 
 * 
 * Indices under each partition are not in any particular order. But even 
 * though this is the case, searching for an index will take O(1) because
 * of the index mapper, which monitors the position of the index under the
 * bin.
 * 
 * Below is a sample data structure of a 10-sized element Index Bin.
 * 
 * 
 * Bin:
 * Used     | Unused
 * 0 1 2 3    4 5 6 7 8 9 10
 * 4 5 2 10 | 9 0 1 3 6 7 8 
 * 
 * Index Mapper
 * 0 1 2 3 4 5 6 7 8  9 10
 * 5 6 2 7 0 1 8 9 10 4 3  
 * 
 */

#ifndef INDEX_BIN_HPP
#define INDEX_BIN_HPP

#include <type_traits>

namespace g80 {

    /**
     * Uncomment #define UNSAFE_OPTIM if you are sure that your 
     * program won't go beyond the boundaries of uint_type
     * 
     * #define UNSAFE_OPTIM 
     * 
     */
    
    #define UNSAFE_OPTIM

    /**
     * Template class proper for 
     * index_bin
     * 
     */

    template<typename uint_type>
    class index_bin {
    
    /**
     * Template validation
     * 
     */

    static_assert(
        std::is_same<uint_type, unsigned char>::value || std::is_same<uint_type, unsigned int>::value || 
        std::is_same<uint_type, unsigned short>::value || std::is_same<uint_type, unsigned long>::value || 
        std::is_same<uint_type, unsigned long long>::value || 

        std::is_same<uint_type, uint8_t>::value || std::is_same<uint_type, uint16_t>::value || 
        std::is_same<uint_type, uint32_t>::value || std::is_same<uint_type, uint64_t>::value,

        "Must be of unsigned integral type");

    /**
     * Constructor, Destructor and 
     * Assignment Helpers
     * 
     */

    private:

        auto del_bin_and_mapper() -> void {
            delete []bin_;
            delete []mapper_;
            bin_ = {nullptr};
            mapper_ = {nullptr};
        }

        auto new_bin_and_mapper() -> void {
            bin_ = new uint_type[size_];
            mapper_ = new uint_type[size_];
        }

        auto reset_mapper_() -> void {

            for (int i = 0; i < size_; ++i) {bin_[i] = i; mapper_[i] = i;}
        }
    
        auto copy_index_bin(const index_bin &rhs) -> void {
            std::copy(rhs.bin_, rhs.bin_ + size_, bin_);
            std::copy(rhs.mapper_, rhs.mapper_ + size_, mapper_);
        }

        auto reset_rhs(index_bin &rhs) {
            rhs.size_ = {0};
            rhs.start_of_unused_ix_ = {0};
            rhs.ix_bin_ = {nullptr};
            rhs.bin_loc_ = {nullptr};
        }

    public:

        auto reset(const uint_type size) {
            size_ = {size};
            start_of_unused_ix_ = {0};
            del_bin_and_mapper();
            reset_mapper_();
        }

        inline auto reset_start_of_unused_ix() {
            start_of_unused_ix_ = {0};
        }

    /**
     * Constructor, Destructor and 
     * Assignment Functions
     * 
     */

    public:
        
        index_bin(uint_type size) : size_(size) {
            new_bin_and_mapper();
            reset_mapper_();
        }

        index_bin(const index_bin &rhs) : 
            size_(rhs.size_), 
            start_of_unused_ix_(rhs.start_of_unused_ix_), 
            bin_(new uint_type[size_]), 
            mapper_(new uint_type[size_]) {
            copy_index_bin(rhs);
        }

        index_bin(index_bin &&rhs) : 
            size_(rhs.size_), 
            start_of_unused_ix_(rhs.start_of_unused_ix_), 
            bin_(rhs.bin_), 
            mapper_(rhs.mapper_) {
            
            reset_rhs(rhs);
        }

        auto operator=(const index_bin &rhs) -> index_bin & {
            del_bin_and_mapper();
            
            size_ = rhs.size_;
            start_of_unused_ix_ = rhs.start_of_unused_ix_;
            new_bin_and_mapper();
            copy_index_bin(rhs);
            return *this;
        }

        auto operator=(index_bin &&rhs) -> index_bin & {
            
            del_bin_and_mapper();
            
            size_ = rhs.size_;
            start_of_unused_ix_ = rhs.start_of_unused_ix_;
            bin_ = rhs.bin_;
            mapper_ = rhs.mapper_;

            reset_rhs(rhs);

            return *this;
        }

        ~index_bin() {
            del_bin_and_mapper();
        }

    /**
     * Public Interface
     * 
     */

    public:



        auto use(const uint_type ix_to_use) -> bool {
            if (ix_to_use >= size_) return false;
            if (mapper_[ix_to_use] < start_of_unused_ix_) return false;
            
            std::swap(bin_[start_of_unused_ix_], bin_[mapper_[ix_to_use]]);
            std::swap(mapper_[bin_[mapper_[ix_to_use]]], mapper_[ix_to_use]);
            ++start_of_unused_ix_;

            return true;
        }

        auto unuse(const uint_type ix_to_unuse) -> bool {
            if (ix_to_unuse >= size_) return false;
            if (mapper_[ix_to_unuse] >= start_of_unused_ix_) return false;
            
            --start_of_unused_ix_;
            std::swap(bin_[start_of_unused_ix_], bin_[mapper_[ix_to_unuse]]);
            std::swap(mapper_[bin_[mapper_[ix_to_unuse]]], mapper_[ix_to_unuse]);
            
            return true;
        }

        auto is_used(const uint_type ix_to_check) -> bool {
            return ix_to_check < start_of_unused_ix_;
        }
        

        inline auto size() const -> uint_type {
            return size_;
        }

        inline auto get_mapper() const -> uint_type * {
            return mapper_;
        }

        inline auto start_of_unused_ix() const -> uint_type {
            return start_of_unused_ix_;
        }

        inline auto cbegin_used() const -> const uint_type * {
            return start_of_unused_ix_ == 0 ? nullptr : &bin_[0];
        }

        inline auto cend_used() const -> const uint_type * {
            return start_of_unused_ix_ == 0 ? nullptr : &bin_[0] + start_of_unused_ix_;
        }

        inline auto cbegin_unused() const -> const uint_type * {
            return &bin_[0] + start_of_unused_ix_;
        }
        
        inline auto cend_unused() const -> const uint_type * {
            return &bin_[0] + size_;
        }

    /**
     * Iterator functions
     * 
     */

    public:

    /**
     * Internal table index variables 
     * 
     */

    private:
    
        uint_type size_, start_of_unused_ix_{0};
        uint_type *bin_{nullptr}, *mapper_{nullptr};
    };
}

#endif