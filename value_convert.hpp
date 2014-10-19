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
 * @file    cinek/value.hpp
 * @author  Samir Sinha
 * @date    9/25/2014
 * @brief   Attribute Map (Dictionary name value) definition
 * @copyright Cinekine
 */

#ifndef CINEK_VALUE_CONVERT_HPP
#define CINEK_VALUE_CONVERT_HPP

#include "cinek/types.hpp"
#include "cinek/string.hpp"

namespace cinek {

template<typename TSource, typename TTarget>
class value_convert
{
public:
    static TTarget value(const TSource& src) {
        return TTarget();
    }
};

//  supported conversions

////////////////////////////////////////////////////////////////////////////////
//  int32_t -> string, int64_t, float, double
//
template<> class value_convert<int32_t, int32_t> {
public:
    static int32_t value(const int32_t& src) { return src; }
};

template<> class value_convert<int32_t, int64_t> {
public:
    static int64_t value(const int32_t& src) { return src; }
};

template<> class value_convert<int32_t, float> {
public:
    static float value(const int32_t& src) { return static_cast<float>(src); }
};

template<> class value_convert<int32_t, double> {
public:
    static double value(const int32_t& src) { return static_cast<double>(src); }
};

template<> class value_convert<int32_t, std::string> {
public:
    static std::string value(const int32_t& src) { return std::to_string(src); }
};

////////////////////////////////////////////////////////////////////////////////
//  int64_t -> string, double
//
template<> class value_convert<int64_t, int64_t> {
public:
    static int64_t value(const int64_t& src) { return src; }
};

template<> class value_convert<int64_t, double> {
public:
    static double value(const int64_t& src) { return static_cast<double>(src); }
};

template<> class value_convert<int64_t, std::string> {
public:
    static std::string value(const int64_t& src) { return std::to_string(src); }
};

////////////////////////////////////////////////////////////////////////////////
//  float -> string, int32_t, int64_t, double
template<> class value_convert<float, float> {
public:
    static float value(const float& src) { return src; }
};

template<> class value_convert<float, int32_t> {
public:
    static int32_t value(const float& src) { return static_cast<int32_t>(src); }
};

template<> class value_convert<float, int64_t> {
public:
    static int64_t value(const float& src) { return static_cast<int64_t>(src); }
};

template<> class value_convert<float, double> {
public:
    static double value(const float& src) { return src; }
};

template<> class value_convert<float, std::string> {
public:
    static std::string value(const float& src) { return std::to_string(src); }
};

////////////////////////////////////////////////////////////////////////////////
//  double -> string, int64_t
//
template<> class value_convert<double, double> {
public:
    static double value(const double& src) { return src; }
};

template<> class value_convert<double, int64_t> {
public:
    static int64_t value(const double& src) { return static_cast<int64_t>(src); }
};

template<> class value_convert<double, std::string> {
public:
    static std::string value(const double& src) { return std::to_string(src); }
};

////////////////////////////////////////////////////////////////////////////////
//  string -> int32_t, int64_t, float, double
//
template<> class value_convert<std::string, std::string> {
public:
    static std::string value(const std::string& src) { return src; }
};

template<> class value_convert<std::string, int32_t> {
public:
    static int32_t value(const std::string& src) { return std::stoi(src); }
};

template<> class value_convert<std::string, int64_t> {
public:
    static int64_t value(const std::string& src) { return std::stoll(src); }
};

template<> class value_convert<std::string, float> {
public:
    static float value(const std::string& src) { return std::stof(src); }
};

template<> class value_convert<std::string, double> {
public:
    static float value(const std::string& src) { return std::stod(src); }
};

}

#endif
