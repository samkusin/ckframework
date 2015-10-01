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

#include "ckdefs.h"

#include <type_traits>

namespace cinek {
    template<typename _T> class ObjectPool;
    template<typename _Object, typename _Delegate> class ManagedObjectPool;
}

namespace cinek {

    /** A handle type */
    typedef uint32_t Handle;
    /** A null handle constant */
    const Handle kNullHandle = 0;
    
    /** A UUID array (128-bit) */
    struct UUID
    {
        uint8_t bytes[16];
    };

    bool operator==(const UUID& l, const UUID& r);
    bool operator<(const UUID& l, const UUID& r);
    bool operator!=(const UUID& l, const UUID& r);

    template<typename _HandleValue, typename _HandleOwner>
    class ManagedHandle
    {
        friend _HandleOwner;
        
    public:
        using value_type = _HandleValue;
        
        ManagedHandle(std::nullptr_t) noexcept : _resource(nullptr) {}
        ManagedHandle() noexcept : _resource(nullptr) {}
        ~ManagedHandle() {
            releaseInt();
        }
        ManagedHandle(const ManagedHandle& other) noexcept : _resource(other._resource) {
            acquire();
        }
        ManagedHandle& operator=(const ManagedHandle& other) noexcept {
            _resource = other._resource;
            acquire();
            return *this;
        }
        ManagedHandle(ManagedHandle&& other) noexcept : _resource(other._resource) {
            other._resource = nullptr;
        }
        ManagedHandle& operator=(ManagedHandle&& other) noexcept {
            _resource = other._resource;
            other._resource = nullptr;
            return *this;
        }
        ManagedHandle& operator=(std::nullptr_t) {
            releaseInt();
            return *this;
        }
        
        operator bool() const {
            return _resource != nullptr;
        }
        
        value_type& operator*() const {
            return *_resource;
        }
        
        value_type* operator->() const {
            return _resource;
        }
        
        value_type* resource() {
            return _resource;
        }
        const value_type* resource() const {
            return _resource;
        }
        
    private:
        ManagedHandle(value_type* _resource) : _resource(_resource) {
            acquire();
        }
        
        void releaseInt() {
            release();
            _resource = nullptr;
        }
        
        void acquire();
        void release();
        
        value_type* _resource;
    };


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