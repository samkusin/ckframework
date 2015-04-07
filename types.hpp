/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @file    cinek/types.hpp
 * @author  Samir Sinha
 * @date    2/17/2014
 * @brief   Common framework-level types
 * @copyright Cinekine
 */

#ifndef CINEK_TYPES_HPP
#define CINEK_TYPES_HPP

#include "cinek/ckdefs.h"

#if CK_COMPILER_HAS_STDINT
  #ifdef __cplusplus
    #include <cstdint>
  #else
    #include <stdint.h>
  #endif
#endif

#include <type_traits>

namespace cinek {
    class JobQueue;
}

namespace cinek {

    /** A handle type */
    typedef uint32_t Handle;
    /** A null handle constant */
    const Handle kNullHandle = 0;
    
    /** A specialized handle where the offset points to a vector element */
    struct OffsetHandle
    {
        uint32_t iter;  /**< Instance iteration */
        uint32_t offs;  /**< Instance offset */
    
        OffsetHandle() : iter(0), offs(0) {}
        OffsetHandle(std::nullptr_t): iter(0), offs(0) {}
    
        /** test operators for handle validity */
        operator bool() const {
            return iter != 0;
        }
        bool operator==(const std::nullptr_t&) const {
            return !iter;
        }
        bool operator!=(const std::nullptr_t&) const {
            return iter != 0;
        }
        OffsetHandle& operator=(const std::nullptr_t&) {
            iter = offs = 0;
            return *this;
        }
        OffsetHandle& operator++() {
            ++iter;
            if (!iter) iter = 1;
            return *this;
        }
        OffsetHandle operator++(int) {
            OffsetHandle h = *this;
            ++(*this);
            return h;
        }
    };
    
    /** A UUID array (128-bit) */
    struct UUID
    {
        uint8_t bytes[16];
    };

    bool operator==(const UUID& l, const UUID& r);
    bool operator<(const UUID& l, const UUID& r);
    bool operator!=(const UUID& l, const UUID& r);

    //  Cribbed from http://stackoverflow.com/a/23815961

    /**
     * Varadic min function (compile-time deduced)
     */
    template <typename T0, typename T1>
    constexpr typename std::common_type<T0, T1>::type multi_min(T0&& v0, T1&& v1)
    {
        return v0 < v1 ? std::forward<T0>(v0) : std::forward<T1>(v1);
    }


    template <typename T, typename... Ts>
    constexpr typename std::common_type<T, Ts...>::type multi_min(T&& v, Ts&&... vs)
    {
        return multi_min(std::forward<T>(v), multi_min(std::forward<Ts>(vs)...));
    }

    /**
     * Varadic max function (compile-tile deduced)
     */
    template <typename T0, typename T1>
    constexpr typename std::common_type<T0, T1>::type multi_max(T0&& v0, T1&& v1)
    {
        return v0 < v1 ? std::forward<T1>(v1) : std::forward<T0>(v0);
    }

    template <typename T, typename... Ts>
    constexpr typename std::common_type<T, Ts...>::type multi_max(T&& v, Ts&&... vs)
    {
        return multi_max(std::forward<T>(v), multi_max(std::forward<Ts>(vs)...));
    }

    template<typename... Ts>
    struct sizeof_max;

    template<>
    struct sizeof_max<>
    {
        enum { size = 0 };
    };

    template<typename T0, typename... Ts>
    struct sizeof_max<T0, Ts...>
    {
        enum { size = sizeof(T0) < sizeof_max<Ts...>::size ?
                      sizeof_max<Ts...>::size : sizeof(T0) };
    };

} /* namespace cinek */


#endif