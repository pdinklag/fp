/**
 * rk61.hpp
 * part of pdinklag/fp
 * 
 * MIT License
 * 
 * Copyright (c) Patrick Dinklage, Jonas Ellert and Alexander Herlez
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _FP_RK61_HPP
#define _FP_RK61_HPP

#include <cassert>
#include <cstdint>

namespace fp {

/**
 * \brief 64-bit Rabin-Karp fingerprinting using the Mersenne prime 2^61-1
 */
class RabinKarp61 {
public:
    /**
     * \brief Alias for the fingerprint type
     */
    using Fingerprint = uint64_t;
    
private:
    using uint128_t = __uint128_t;

    static constexpr uint64_t e61 = 61;
    static constexpr uint64_t p61 = (uint64_t(1) << e61) - 1;
    static constexpr uint128_t sq61 = uint128_t(p61) * p61;

    inline static uint64_t mod(uint128_t const value) {
        uint128_t const v = value + 1;
        uint64_t const z = ((v >> e61) + v) >> e61;
        return (value + z) & p61;
    }

    inline static uint64_t power(uint64_t base, uint64_t exponent) {
        uint64_t result = 1;
        while(exponent > 0) {
            if(exponent & 1ULL) {
                result = mod(uint128_t(base) * result);
            }
            base = mod(uint128_t(base) * base);
            exponent >>= 1;
        }
        return result;
    }

    uint64_t base_;
    uint128_t pop_left_precomp_[256] = {};

public:
    RabinKarp61() : base_(0) {
    }

    /**
     * \brief Constructs a fingerprinter for the given base and window size
     * 
     * \param base the base for fingerprint computation
     * \param window the window size for rolling fingerprints (pass zero if not needed)
     */
    RabinKarp61(uint64_t const base, uint64_t const window = 0) : base_(mod(base)) {
        if(window > 0) {
            auto const max_exponent_exclusive = power(base_, window);
            for(uint64_t c = 0; c < 256; c++) {
                pop_left_precomp_[c] = sq61 - uint128_t(max_exponent_exclusive) * c;
            }
        }
    }

    RabinKarp61(RabinKarp61&&) = default;
    RabinKarp61& operator=(RabinKarp61&&) = default;

    RabinKarp61(RabinKarp61 const&) = default;
    RabinKarp61& operator=(RabinKarp61 const&) = default;

    /**
     * \brief Appends a character to a fingerprint
     * 
     * \param fp the fingerprint to extend
     * \param push_right the character to append on the right
     * \return the new fingerprint
     */
    __attribute__((always_inline))
    inline Fingerprint push(Fingerprint const fp, uint8_t const push_right) const {
        const uint128_t shifted_fingerprint = uint128_t(base_) * fp;
        const uint128_t pop = sq61;
        return mod(shifted_fingerprint + pop + uint128_t(push_right));
    }

    /**
     * \brief Rolls a fingerprint
     * 
     * \param fp the fingerprint to roll
     * \param pop_left the character to remove from the beginning of the string
     * \param push_right the character to append to the end of the string
     * \return the new fingerprint
     */
    __attribute__((always_inline))
    inline Fingerprint roll(Fingerprint const fp, uint8_t const pop_left, uint8_t const push_right) const {
        const uint128_t shifted_fingerprint = uint128_t(base_) * fp;
        const uint128_t pop = pop_left_precomp_[pop_left];
        return mod(shifted_fingerprint + pop + uint128_t(push_right));
    }
};

}

#endif
