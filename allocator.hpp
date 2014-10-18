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
 * @file    cinek/allocator.hpp
 * @author  Samir Sinha
 * @date    1/6/2013
 * @brief   std library allocator using custom allocation hooks
 * @copyright Cinekine
 */

#ifndef CINEK_ALLOC_HPP
#define CINEK_ALLOC_HPP

#include "cinek/types.hpp"

#include <memory>

namespace cinekine {

/** Callbacks used for memory allocation/deallocation */
struct cinek_memory_callbacks
{
    /** Invoked when a subsystem allocates memory. */
    void*   (*alloc)(void* ctx, size_t numBytes);
    /** Invoked when a subsystem frees memory. */
    void    (*free)(void* ctx, void* ptr);
    /** Invoked when a subsystem reallocates memory given a block of memory
     * previously allocated by alloc. */
    void*   (*realloc)(void* ctx, void* ptr, size_t numBytes);
    /** An application specific context pointer passed to the callback function
     * pointers in callbacks. */
    void*   context;
};

/** Specify a custom memory allocator with an application specific context for
 *  CineK systems.
 *
 *  All applications must call this function with its own set of memory callbacks
 *  before using any CineK systems.
 *
 *  @param  callbacks       The custom allocator defined by a series of application
 *                          defined callbacks.
 */
void cinek_alloc_set_callbacks(
        const cinek_memory_callbacks* callbacks
    );

/** Returns the allocation callbacks and context supplied by
 *  cinek_alloc_set_callbacks.
 *
 *  @param  callbacks Pointer to a struct to hold the memory allocation
 *                    callbacks.
 */
void cinek_get_alloc_callbacks(cinek_memory_callbacks* callbacks);

/**
 * @class Allocator
 * @brief Wraps a cinek_alloc based memory allocator for use in
 * compliant C++ objects.
 *
 */
class Allocator
{
public:
	Allocator() {
		cinek_get_alloc_callbacks(&_callbacks);
	}
    Allocator(Allocator&& other) :
        _callbacks(other._callbacks)
    {
        cinek_get_alloc_callbacks(&other._callbacks);
    }
    Allocator& operator=(Allocator&& other)
    {
        _callbacks = other._callbacks;
        cinek_get_alloc_callbacks(&other._callbacks);
        return *this;
    }
    Allocator(const Allocator& other) :
        _callbacks(other._callbacks) {}
    Allocator& operator=(const Allocator& other) {
        _callbacks = other._callbacks;
        return *this;
    }
	/**
	 * Constructor.
	 * @param allocCallbacks Memory allocation callbacks.
	 * @param context 		 Context passed to the callbacks supplied via
	 *                   	 allocCallbacks.
	 */
	Allocator(const cinek_memory_callbacks& allocCallbacks)
		: _callbacks(allocCallbacks) {}
	/**
	 * Allocates a block of memory of the supplied size.
	 * @param  size Size of the memory block to allocate.
	 * @return A pointer to the allocated block or nullptr.
	 */
	void* alloc(size_t size) {
		return (*_callbacks.alloc)(_callbacks.context, size);
	}
	/**
	 * Allocates and constucts instance of T.
     * @param  args Initialization arguments to the constructor for T.
	 * @return      A pointer to T.
	 */
	template<typename T, class... Args> T* newItem(Args&&... args) {
        T* p = reinterpret_cast<T*>(alloc(sizeof(T)));
        ::new(p) T(std::forward<Args>(args)...);
        return p;
	}
    /**
     * Destroys an instance of T allocated via newItem.
     * @param item The item to delete.
     */
    template<typename T> void deleteItem(T* item) {
        item->~T();
        free(item);
    }
	/**
	 * Allocates an array of T.
	 * @param  count Number of items in array.
	 * @return       A pointer to the array
	 */
	template<typename T> T* allocItems(size_t count) {
        T* p = reinterpret_cast<T*>(alloc(sizeof(T)*count));
		return p;
	}
	/**
	 * Frees memory allocated by alloc.
	 * @param ptr Pointer to the memory block to free.
	 */
	void free(void* ptr) {
		(*_callbacks.free)(_callbacks.context, ptr);
	}

private:
	cinek_memory_callbacks _callbacks;
	friend bool operator==(const Allocator& lha, const Allocator& rha);
	friend bool operator!=(const Allocator& lha, const Allocator& rha);
};
/** @cond */
inline bool operator==(const Allocator& lha, const Allocator& rha)
{
    return memcmp(&lha._callbacks, &rha._callbacks, sizeof(cinek_memory_callbacks))==0;
}
inline bool operator!=(const Allocator& lha, const Allocator& rha)
{
    return memcmp(&lha._callbacks, &rha._callbacks, sizeof(cinek_memory_callbacks))!=0;
}
/** @endcond */

/**
 * @class std_allocator
 * @brief A std::allocator compliant allocator for STL containers.
 */
template <typename T, class Allocator>
struct std_custom_allocator
{
    /** @cond */
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U> struct rebind { typedef std_custom_allocator<U, Allocator> other; };
    std_custom_allocator() {}
#ifdef CK_CPP_11_BASIC  // libc++11 supports stateful allocators
    std_custom_allocator(const Allocator& allocator): _allocator(allocator) {}
#endif
    std_custom_allocator(const std_custom_allocator& source): _allocator(source._allocator) {}

    template <class U> std_custom_allocator(const std_custom_allocator<U, Allocator>& source): _allocator(source._allocator) {}

    ~std_custom_allocator() {}

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

#ifdef CK_CPP_11_BASIC
    template<class U, class... Args> void construct(U* p, Args&&... args)
    {
        ::new((void *)p) U(std::forward<Args>(args)...);
    }
#else
    void construct(pointer p, const T& val)
    {
        new((void *)p) T(val);
    }
#endif

    void destroy(pointer p)
    {
        p->~T();
    }

    Allocator _allocator;

    /** @endcond */
};

/** @cond */
template<typename T, class Allocator>
inline bool operator==(const std_custom_allocator<T, Allocator>& lha,
                       const std_custom_allocator<T, Allocator>& rha)
{
    return lha._allocator == rha._allocator;
}
template<typename T, class Allocator>
inline bool operator!=(const std_custom_allocator<T, Allocator>& lha,
                       const std_custom_allocator<T, Allocator>& rha)
{
    return lha._allocator != rha._allocator;
}
/** @endcond */

/** std_allocator using the generic Allocator */
template <typename T>
using std_allocator = std_custom_allocator<T, Allocator>;


#ifdef CK_CPP_11_BASIC
/**
 * @class AllocatorDeleter
 * @brief Used by std pointer types that refer to memory allocated using external
 * memory managers.
 */
template<typename T>
struct AllocatorDeleter
{
    /** @cond */
    AllocatorDeleter() {}
    AllocatorDeleter(const Allocator& allocator): _allocator(allocator) {}
    void operator()(T* ptr)  {
        _allocator.deleteItem(ptr);
    }
    Allocator _allocator;
    /** @endcond */
};

/** unique_ptr using the shared deleter. */
template<typename T>
using unique_ptr = std::unique_ptr<T, AllocatorDeleter<T>>;

template<typename T, typename... Args>
unique_ptr<T> allocate_unique(Allocator& allocator, Args&&... args) {
    T* ptr = allocator.newItem<T>(std::forward<Args>(args)...);
    return unique_ptr<T>(ptr, allocator);
}

#endif


}	// namespace cinekine

#endif