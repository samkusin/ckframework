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
 * @file    value.hpp
 * @author  Samir Sinha
 * @date    9/25/2014
 * @brief   Attribute Map (Dictionary name value) definition
 * @copyright Cinekine
 */

#ifndef CINEK_VALUE_HPP
#define CINEK_VALUE_HPP

#include "types.hpp"
#include "allocator.hpp"
#include "value_convert.hpp"

namespace cinek {

    template<typename StringType>
    class Value
    {
    public:
        typedef StringType          string_type;

        Value();
        template<typename U> Value(const U& val);
        ~Value();

        void set(const int32_t val);
        void set(const int64_t val);
        void set(const float val);
        void set(const double val);
        void set(const string_type& val);

        template<typename T> T get() const;

    private:
        enum class value_class {
            kNull, kInt32, kInt64, kFloat, kDouble, kString
        };
        enum {
            kBufSize = sizeof(string_type)
        };
        value_class vclass;
        uint8_t buf[kBufSize];
    };

    ////////////////////////////////////////////////////////////////////////////
    template<typename StringType>
    Value<StringType>::Value() :
        vclass(value_class::kNull)
    {
    }

    template<typename StringType>
    template<typename U>
    Value<StringType>::Value(const U& val) :
        vclass(value_class::kNull)
    {
        set(val);
    }

    template<typename StringType>
    Value<StringType>::~Value()
    {
        switch(vclass)
        {
        case value_class::kString: reinterpret_cast<string_type*>(buf)->~string_type(); break;
        default: break;
        }
    }

    template<typename StringType>
    void Value<StringType>::set(const int32_t val)
    {
        this->~Value();
        vclass = value_class::kInt32;
        *reinterpret_cast<int32_t*>(buf) = val;
    }
    template<typename StringType>
    void Value<StringType>::set(const int64_t val)
    {
        this->~Value();
        vclass = value_class::kInt64;
        *reinterpret_cast<int64_t*>(buf) = val;
    }
    template<typename StringType>
    void Value<StringType>::set(const float val)
    {
        this->~Value();
        vclass = value_class::kFloat;
        *reinterpret_cast<float*>(buf) = val;
    }
    template<typename StringType>
    void Value<StringType>::set(const double val)
    {
        this->~Value();
        vclass = value_class::kDouble;
        *reinterpret_cast<double*>(buf) = val;
    }
    template<typename StringType>
    void Value<StringType>::set(const string_type& val)
    {
        this->~Value();
        vclass = value_class::kString;
        ::new(buf) string_type(val);
    }

    template<typename StringType>
    template<typename T>
    T Value<StringType>::get() const
    {
        switch (vclass)
        {
        case value_class::kInt32:
            return value_convert<int32_t, T>::value(*reinterpret_cast<const int32_t*>(buf));
        case value_class::kInt64:
            return value_convert<int64_t, T>::value(*reinterpret_cast<const int64_t*>(buf));
        case value_class::kFloat:
            return value_convert<float, T>::value(*reinterpret_cast<const float*>(buf));
        case value_class::kDouble:
            return value_convert<double, T>::value(*reinterpret_cast<const double*>(buf));
        case value_class::kString:
            return value_convert<string_type, T>::value(*reinterpret_cast<const string_type*>(buf));
        default:
            break;
        }

        return T();
    }

}   /* namespace cinek */

#endif
