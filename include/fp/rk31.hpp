/**
 * rk31.hpp
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

#ifndef _FP_RK31_HPP
#define _FP_RK31_HPP

#include <cassert>
#include <cstdint>

namespace fp {

/**
 * \brief 32-bit Rabin-Karp fingerprinting using the Mersenne prime 2^31-1
 */
class RabinKarp31 {
public:
    /**
     * \brief Alias for the fingerprint type
     */
    using Fingerprint = uint32_t;

private:
    static constexpr uint32_t e31 = 31;
    static constexpr uint32_t p31 = (1ULL << 31) - 1;

    static inline constexpr uint32_t mod(uint64_t const value) {
        uint32_t const i = (value & p31) + (value >> e31);
        return (i >= p31) ? (i - p31) : i;
    }

    static constexpr uint32_t power(uint32_t base, uint32_t exponent) {
        uint32_t result = 1;
        while (exponent > 0) {
            if (exponent & 1ULL) {
                result = mod(uint64_t(base) * result);
            }
            base = mod(uint64_t(base) * base);
            exponent >>= 1;
        }
        return result;
    }

    uint32_t base_;
    uint32_t table_[256][256] = {};

public:
    RabinKarp31() : base_(0) {
    }

    /**
     * \brief Constructs a fingerprinter for the given base and window size
     * 
     * \param base the base for fingerprint computation
     * \param window the window size for rolling fingerprints (pass zero if not needed)
     */
    RabinKarp31(uint32_t const base, uint32_t const window = 0) : base_(mod(base)) {
        if(window > 0) {
            uint32_t const max_exponent = power(base_, window);
            for (uint32_t i = 0; i < 256; ++i) {
                table_[i][0] = mod(p31 - mod(i * (uint64_t)max_exponent));
                for (uint32_t j = 1; j < 256; ++j) {
                    table_[i][j] = mod(table_[i][j - 1] + 1);
                }
            }
        }
    };

    RabinKarp31(RabinKarp31&&) = default;
    RabinKarp31& operator=(RabinKarp31&&) = default;

    RabinKarp31(RabinKarp31 const&) = default;
    RabinKarp31& operator=(RabinKarp31 const&) = default;

    /**
     * \brief Extends a fingerprint
     * 
     * \param fp the fingerprint to extend
     * \param push_right the character to append on the right
     * \return the new fingerprint
     */
    __attribute__((always_inline)) 
    inline Fingerprint push(Fingerprint const fp, uint8_t const push_right) const {
        assert(base_ < p31 && fp < p31);
        return mod(((uint64_t)base_) * fp + push_right);
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
        auto const lookup = table_[pop_left][push_right];
        assert(base_ < p31 && fp < p31 && lookup < p31);
        return mod(((uint64_t)base_) * fp + lookup);
    }
};

}

#endif
