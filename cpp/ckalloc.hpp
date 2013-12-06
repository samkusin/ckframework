/**
 *  @file   cpp/ckalloc.cpp
 *
 *  @note   Created by Samir Sinha on 6/22/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_ALLOC_HPP
#define CINEK_ALLOC_HPP

#include "ckdefs.hpp"
#include "cinek/core/ckalloc.h" 

#include <memory>

namespace cinekine {
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
	/**
	 * Constructor.
	 * @param allocCallbacks Memory allocation callbacks.
	 * @param context 		 Context passed to the callbacks supplied via
	 *                   	 allocCallbacks. 
	 */
	Allocator(const cinek_memory_callbacks_t& allocCallbacks)
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
 * @class SharedPtrDeleter
 * @brief Used by std pointer types that refer to memory allocated using external
 * memory managers. 
 */
template<typename T>
struct SharedPtrDeleter
{
    /** @cond */
    SharedPtrDeleter() {}
    SharedPtrDeleter(const Allocator& allocator): _allocator(allocator) {}
    void operator()(T* ptr)  {
        _allocator.deleteItem(ptr);
    }
    Allocator _allocator;
    /** @endcond */
};

/** unique_ptr using the shared deleter. */
template<typename T>
class unique_ptr: public std::unique_ptr<T, SharedPtrDeleter<T>>
{
public:
    unique_ptr() = default;
    unique_ptr(T* ptr, const Allocator& allocator) : 
        std::unique_ptr<T, SharedPtrDeleter<T>>(ptr, SharedPtrDeleter<T>(allocator)) {}
};

#endif


}	// namespace cinekine

#endif