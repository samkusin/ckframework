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
#include <utility>

namespace ckmsg {

template<typename Alloc>
class Buffer
{
public:
    Buffer() = default;
    Buffer(size_t sz, Alloc alloc);
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
    const uint8_t* peekp(size_t cnt) const;

    void updateRead();
    void revertRead();

    void reset();

private:
    void moveFrom(Buffer& buffer);
    Alloc _alloc;
    uint8_t* _start = nullptr;
    uint8_t* _head = nullptr;
    uint8_t* _tail = nullptr;
    uint8_t* _limit = nullptr;
    uint8_t* _writeHead = nullptr;
    uint8_t* _readHead = nullptr;
};

template<typename Alloc>
Buffer<Alloc>::Buffer(size_t sz, Alloc alloc) :
    _alloc(alloc)
{
    _start = reinterpret_cast<uint8_t*>(_alloc.alloc(sz));
    _head = _start;
    _tail = _head;
    _limit = _start ? _start + sz : nullptr;
    _writeHead = _tail;
    _readHead = _head;
}

template<typename Alloc>
Buffer<Alloc>::~Buffer()
{
    _alloc.free(_start);
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
    _alloc = std::move(buffer._alloc);
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
        return (_writeHead - _head) >= (ptrdiff_t)sz;
    }
    else if (_limit - _head >= (ptrdiff_t)sz) {
        return true;
    }
    else if (_writeHead - _start >= (ptrdiff_t)sz) {
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
    ptrdiff_t avail = 0;
	ptrdiff_t amt = (ptrdiff_t)cnt;

    //  enforce that write requests must be contiguous (in memory) blocks
    if (_tail >= _readHead) {
        avail = _limit - _tail;
        if (avail < amt) {
            p = _start;
        }
    }
    if (avail < amt && p <= _readHead) {
        avail = _readHead - p;
        if (avail <= amt)       // wraparound, tail == head is overflow
            return nullptr;
    }
    _tail = p + amt;
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
    const uint8_t* p = peekp(cnt);
    if (!p)
        return nullptr;

    _head = const_cast<uint8_t*>(p) + cnt;
    return p;
}

/*
const uint8_t* calcHead(size_t offset)
{
    const uint8_t* head = _head + offset;
    if (head > _limit) {
        head = _start + (head )
}
*/

template<typename Alloc>
const uint8_t* Buffer<Alloc>::peekp(size_t cnt) const
{
    const uint8_t* p = _head;
    ptrdiff_t avail = 0;
	ptrdiff_t amt = (ptrdiff_t)cnt;
    //  enforce that read requests are for contiguous memory
    if (_head > _writeHead) {
        avail = _limit - _head;
        if (avail < amt) {
            p = _start;
        }
    }
    if (avail < amt && p <= _writeHead) {
        avail = _writeHead - p;
        if (avail < amt)        // head < tail always means available read data
            return nullptr;
    }
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
