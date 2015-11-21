/**
 * @file        buffer.hpp
 * @author      Samir Sinha
 * @date        11/17/2015
 * @brief       A ring buffer implementation
 * @copyright   Copyright 2015 Cinekine.  This project is release under the MIT
 *              license
 *
 * This is a modified version of cinek/buffer.hpp to remove dependencies on
 * the cinek project, and to add messenger specific behavior.
 *
 * Changes:
 * - Allocator scheme changed
 * - Reads and write chunks must remain contiguous in memory (no wraparound of
 *   of data from end to start
 */

#ifndef CINEK_MSG_BUFFER_HPP
#define CINEK_MSG_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace ckmsg {

template<typename Alloc>
class Buffer
{
public:
    Buffer() = default;
    Buffer(size_t sz);
    ~Buffer();
    
    Buffer(const Buffer& r) = delete;
    Buffer& operator=(const Buffer& r) = delete;
    Buffer(Buffer&& r);
    Buffer& operator=(Buffer&& r);
    
    size_t size() const;
    size_t readSize() const;
    bool readSizeContiguous(size_t sz) const;
    size_t writeSize() const;
    bool writeSizeContiguous(size_t sz) const;
    
    uint8_t* writep(size_t cnt);
    void updateWrite();
    void revertWrite();
    
    const uint8_t* readp(size_t cnt);
    void updateRead();
    void revertRead();
    
    void reset();
    
private:
    void moveFrom(Buffer& buffer);
    
    uint8_t* _start = nullptr;
    uint8_t* _head = nullptr;
    uint8_t* _tail = nullptr;
    uint8_t* _limit = nullptr;
    uint8_t* _writeHead = nullptr;
    uint8_t* _readHead = nullptr;
};

template<typename Alloc>
Buffer<Alloc>::Buffer(size_t sz)
{
    _start = Alloc::allocate(sz);
    _head = _start;
    _tail = _head;
    _limit = _start ? _start + sz : nullptr;
    _writeHead = _tail;
    _readHead = _head;
}

template<typename Alloc>
Buffer<Alloc>::~Buffer()
{
    Alloc::free(_start);
    _start = _head = _tail = _limit = nullptr;
}

template<typename Alloc>
Buffer<Alloc>::Buffer(Buffer&& r)
{
    moveFrom(r);
}

template<typename Alloc>
Buffer<Alloc>& Buffer<Alloc>::operator=(Buffer&& r)
{
    moveFrom(r);
    return *this;
}

template<typename Alloc>
void Buffer<Alloc>::moveFrom(Buffer& buffer)
{
    _start = buffer._start;
    _head = buffer._head;
    _tail = buffer._tail;
    _limit = buffer._limit;
    _writeHead = buffer._writeHead;
    _readHead = buffer._readHead;
    buffer._start = nullptr;
    buffer._head = nullptr;
    buffer._tail = nullptr;
    buffer._limit = nullptr;
    buffer._writeHead = nullptr;
    buffer._readHead = nullptr;
}

template<typename Alloc>
size_t Buffer<Alloc>::size() const
{
    return _limit - _start;
}

template<typename Alloc>
size_t Buffer<Alloc>::readSize() const
{
    return (_writeHead>=_head) ? (_writeHead-_head) : (_limit-_head) + (_writeHead-_start);
}

template<typename Alloc>
bool Buffer<Alloc>::readSizeContiguous(size_t sz) const
{
    if (_writeHead >= _head) {
        return (_writeHead - _head) >= sz;
    }
    else if (_limit - _head >= sz) {
        return true;
    }
    else if (_writeHead - _start >= sz) {
        return true;
    }
    return false;
}

template<typename Alloc>
size_t Buffer<Alloc>::writeSize() const
{
    return (_readHead>_tail) ? (_readHead-_tail)-1 :  (_limit-_tail) + (_readHead-_start);
}

template<typename Alloc>
bool Buffer<Alloc>::writeSizeContiguous(size_t sz) const
{
    if (_readHead > _tail) {
        return (_readHead - _tail - 1) >= sz;
    }
    else if (_limit - _tail >= sz) {
        return true;
    }
    else if (_readHead - _start >= sz) {
        return true;
    }
    return false;
}

template<typename Alloc>
uint8_t* Buffer<Alloc>::writep(size_t cnt)
{
    uint8_t* p = _tail;
    size_t avail;
    
    //  enforce that write requests must be contiguous (in memory) blocks
    if (_tail >= _head) {
        avail = _limit - _tail;
        if (avail < cnt) {
            if (_start < _readHead)
                p = _start;
        }
    }
    if (p < _readHead) {
        avail = _readHead - p - 1;
        if (avail < cnt)
            return nullptr;
    }
    _tail = p + cnt;
    return p;
}

template<typename Alloc>
void Buffer<Alloc>::updateWrite()
{
    _writeHead = _tail;
}

template<typename Alloc>
void Buffer<Alloc>::revertWrite()
{
    _tail = _writeHead;
}

template<typename Alloc>
const uint8_t* Buffer<Alloc>::readp(size_t cnt)
{
    uint8_t* p = _head;
    size_t avail;
    //  enforce that read requests are for contiguous memory
    if (_head >= _tail+1) {
        avail = _limit - _head;
        if (avail < cnt) {
            if (_start < _writeHead)
                p = _start;
        }
    }
    if (p < _writeHead) {
        avail = _writeHead - p;
        if (avail < cnt)
            return nullptr;
    }
    _head = p + cnt;
    return p;
}

template<typename Alloc>
void Buffer<Alloc>::updateRead()
{
    _readHead = _head;
}

template<typename Alloc>
void Buffer<Alloc>::revertRead()
{
    _head = _readHead;
}


template<typename Alloc>
void Buffer<Alloc>::reset()
{
    _readHead = _head = _start;
    _writeHead = _tail = _start;
}

} /* namespace ckmsg */


#endif
