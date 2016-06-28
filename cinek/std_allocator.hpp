/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
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
 * @file    cinek/std_allocator.hpp
 * @author  Samir Sinha
 * @date    1/6/2013
 * @brief   std library allocator using custom allocation hooks
 * @copyright Cinekine
 */

#ifndef CINEK_STD_ALLOCATOR_HPP
#define CINEK_STD_ALLOCATOR_HPP

#include <memory>

namespace cinek {
/**
 * @class std_allocator
 * @brief A std::allocator compliant allocator for STL containers.
 */
template <typename T, class Allocator>
struct std_allocator
{
    /** @cond */
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U> struct rebind {
        typedef std_allocator<U, Allocator> other;
    };

    std_allocator() {}
    std_allocator(const Allocator& allocator): _allocator(allocator) {}
    std_allocator(const std_allocator& source): _allocator(source._allocator) {}

    template <class U>
    std_allocator(const std_allocator<U, Allocator>& source): _allocator(source._allocator) {}

    ~std_allocator() {}

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }

    pointer allocate(size_type s, const void* = 0)
    {
        if (s == 0)
            return nullptr;
        pointer temp = static_cast<pointer>(_allocator.alloc(s*sizeof(T)));

    #if CK_CPP_EXCEPTIONS
        if (temp == nullptr)
            throw std::bad_alloc();
    #endif
        return temp;
    }

    void deallocate(pointer p, size_type)
    {
        _allocator.free((void* )p);
    }

    size_type max_size() const
    {
        return std::numeric_limits<size_t>::max() / sizeof(T);
    }

    template<class U, class... Args> void construct(U* p, Args&&... args)
    {
        ::new((void *)p) U(std::forward<Args>(args)...);
    }

    void destroy(pointer p)
    {
        p->~T();
    }

    Allocator _allocator;

    /** @endcond */
};

/** @cond */
template<typename T, class Allocator>
inline bool operator==(const std_allocator<T, Allocator>& lha,
                       const std_allocator<T, Allocator>& rha)
{
    return lha._allocator == rha._allocator;
}
template<typename T, class Allocator>
inline bool operator!=(const std_allocator<T, Allocator>& lha,
                       const std_allocator<T, Allocator>& rha)
{
    return lha._allocator != rha._allocator;
}
/** @endcond */

/**
 * @class AllocatorDeleter
 * @brief Used by std pointer types that refer to memory allocated using external
 * memory managers.
 */
template<typename T, typename Allocator>
struct AllocatorDeleter
{
    /** @cond */
    AllocatorDeleter() {}
    AllocatorDeleter(const Allocator& allocator): _allocator(allocator) {}
    template<typename U>
    AllocatorDeleter(const AllocatorDeleter<U, Allocator>& other): _allocator(other._allocator) {}
    void operator()(T* ptr)  {
        _allocator.deleteItem(ptr);
    }
    Allocator _allocator;
    /** @endcond */
};

/** unique_ptr using the shared deleter. */
template<typename T, typename Allocator>
using unique_ptr = std::unique_ptr<T, AllocatorDeleter<T, Allocator>>;

template<typename T, typename TBase, typename Allocator, typename... Args>
unique_ptr<TBase, Allocator> allocate_unique(Allocator& allocator, Args&&... args) {
    TBase* ptr = allocator.template newItem<T>(std::forward<Args>(args)...);
    return unique_ptr<TBase, Allocator>(ptr, allocator);
}

template<typename T, typename Allocator, typename... Args>
unique_ptr<T, Allocator> allocate_unique(Allocator& allocator, Args&&... args) {
    T* ptr = allocator.template newItem<T>(std::forward<Args>(args)...);
    return unique_ptr<T, Allocator>(ptr, allocator);
}

/*
template<typename T, typename Allocator, typename... Args>
unique_ptr<T, Allocator> allocate_unique(Args&&... args) {
    Allocator allocator;
    T* ptr = allocator.template newItem<T>(std::forward<Args>(args)...);
    return unique_ptr<T, Allocator>(ptr, allocator);
}
*/

} // namespace cinek

#endif
