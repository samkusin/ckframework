/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Cinekine Media
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
 * @file    cinek/buffer.hpp
 * @author  Samir Sinha
 * @date    4/19/2015
 * @brief   A ring buffer implementation
 * @copyright Cinekine
 */


#ifndef CINEK_BUFFER_HPP
#define CINEK_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <cstring>

namespace cinek {

template<typename Alloc>
class Buffer
{
public:
    Buffer() = default;
    Buffer(size_t sz, const Alloc& allocator=Alloc());
    ~Buffer();
    
    Buffer(const Buffer& r) = delete;
    Buffer& operator=(const Buffer& r) = delete;
    Buffer(Buffer&& r);
    Buffer& operator=(Buffer&& r);
    
    size_t size() const;
    size_t readAvailable() const;
    size_t writeAvailable() const;
    
    size_t write(const uint8_t* buffer, size_t cnt);
    size_t read(uint8_t* buffer, size_t cnt);
    void reset();
    
private:
    Alloc _alloc;
    uint8_t* _start = nullptr;
    uint8_t* _head = nullptr;
    uint8_t* _tail = nullptr;
    uint8_t* _limit = nullptr;
};

template<typename Alloc>
Buffer<Alloc>::Buffer(size_t sz, const Alloc& alloc) :
    _alloc(alloc)
{
    _start = reinterpret_cast<uint8_t*>(_alloc.alloc(sz));
    _head = _start;
    _tail = _head;
    _limit = _start ? _start + sz : nullptr;
}

template<typename Alloc>
Buffer<Alloc>::~Buffer()
{
    _alloc.free(_start);
    _start = _head = _tail = _limit = nullptr;
}

template<typename Alloc>
Buffer<Alloc>::Buffer(Buffer<Alloc>&& r) :
    _alloc(std::move(r._alloc)),
    _start(r._start),
    _head(r._head),
    _tail(r._tail),
    _limit(r._limit)
{
    r._start = nullptr;
    r._head = nullptr;
    r._tail = nullptr;
    r._limit = nullptr;
}

template<typename Alloc>
Buffer<Alloc>& Buffer<Alloc>::operator=(Buffer<Alloc>&& r)
{
    _alloc = std::move(r._alloc);
    _start = r._start;
    _head = r._head;
    _tail = r._tail;
    _limit = r._limit;
    r._start = nullptr;
    r._head = nullptr;
    r._tail = nullptr;
    r._limit = nullptr;
    return *this;
}

template<typename Alloc>
size_t Buffer<Alloc>::size() const
{
    return _limit - _start;
}

template<typename Alloc>
size_t Buffer<Alloc>::readAvailable() const
{
    return (_tail>=_head) ? (_tail-_head) : (_limit-_head) + (_tail-_start);
}

template<typename Alloc>
size_t Buffer<Alloc>::writeAvailable() const
{
    if (_head > _tail)
        return (_head-_tail)-1;
    else
        return (_limit-_tail) + (_head-_start) ;
}

template<typename Alloc>
size_t Buffer<Alloc>::write(const uint8_t* buffer, size_t cnt)
{
    size_t amt = cnt;
    if (_tail >= _head) // write space split between end and start of buffer
    {
        size_t avail = _limit - _tail;
        if (avail)
        {
            if (avail > amt)
                avail = amt;
            memcpy(_tail, buffer, avail);
            amt -= avail;
            _tail += avail;
            buffer += avail;
        }
        if (amt)
        {
            if (_start < _head)
                _tail = _start;
        }
    }
    if (amt)
    {
        size_t avail = _head-_tail-1;
        if (avail)
        {
            if (avail > amt)
                avail = amt;
            memcpy(_tail, buffer, avail);
            amt -= avail;
            _tail += avail;
        }
    }
    
    return cnt - amt;
}

template<typename Alloc>
size_t Buffer<Alloc>::read(uint8_t* buffer, size_t cnt)
{
    size_t amt = cnt;
    if (_head>_tail+1)  // read space split between end and start of buffer
    {
        size_t avail = _limit-_head;
        if (avail)
        {
            if (avail > amt)
                avail = amt;
            memcpy(buffer, _head, avail);
            amt -= avail;
            _head += avail;
            buffer += avail;
        }
        if (amt)
        {
            //  wrap around to start to add more data
            if (_start < _tail)
                _head = _start;
        }
    }
    if (amt)
    {
        size_t avail = _tail-_head;
        if (avail)
        {
            if (avail > amt)
                avail = amt;
            memcpy(buffer, _head, avail);
            amt -= avail;
            _head += avail;
        }
    }
    
    return cnt - amt;
}

template<typename Alloc>
void Buffer<Alloc>::reset()
{
    _tail = _head = _start;
}

} /* namespace cinek */


#endif
