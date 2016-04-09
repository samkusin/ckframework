//
//  EngineMath.hpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CINEK_MATH_TYPES_HPP
#define CINEK_MATH_TYPES_HPP

#include <cinek/ckdefs.h>
#include <cmath>

namespace ckm {

using scalar = float;

constexpr scalar kEpsilon = 1e-6;
constexpr scalar kEpsilonSmall = 1e-9;
constexpr scalar kPi = M_PI;

inline bool nearZero(scalar v) {
    return v > -kEpsilon && v < kEpsilon;
}

inline bool nearZeroSmall(scalar v) {
    return v > -kEpsilonSmall && v < kEpsilonSmall;
}

template<typename T>
struct vector2
{
    typedef T value_type;
    union
    {
        struct { value_type x, y; };
        struct { value_type u, v; };
        value_type comp[2];
    };
    operator value_type*() { return comp; }
    operator const value_type*() const { return comp; }
    
    vector2();
    vector2(value_type v);
    vector2(value_type x, value_type y);
    vector2& set(value_type x, value_type y);
};

/// A 3x1 uniform
template<typename T>
struct vector3
{
    typedef T value_type;
    
    static const vector3 kUnitX;
    static const vector3 kUnitY;
    static const vector3 kUnitZ;
    static const vector3 kZero;
    
    union
    {
        struct { value_type x, y, z; };
        struct { value_type r, g, b; };
        value_type comp[3];
    };
    operator value_type*() { return comp; }
    operator const value_type*() const { return comp; }
    
    vector3();
    vector3(value_type v);
    vector3(value_type x, value_type y, value_type z);
    vector3& set(value_type x, value_type y, value_type z);
};

/// A 4x1 uniform
template<typename T>
struct vector4
{
    typedef T value_type;
    
    static const vector4 kUnitX;
    static const vector4 kUnitY;
    static const vector4 kUnitZ;
    static const vector4 kUnitW;
    static const vector4 kZero;
    
    union
    {
        struct { value_type x, y, z, w; };
        struct { value_type r, g, b, a; };
        value_type comp[4];
    };
    operator value_type*() { return comp; }
    operator const value_type*() const { return comp; }
    
    vector4() {}
    vector4(value_type v) : comp { v,v,v,v }
    {
    }
    vector4(value_type x, value_type y, value_type z, value_type w) :
        comp { x, y, z, w }
    {
    }
    vector4(const vector3<T>& v, value_type w) :
        comp { v.x, v.y, v.z, w }
    {
    }
    vector4& set(value_type x, value_type y, value_type z, value_type w) {
        comp[0] = x;
        comp[1] = y;
        comp[2] = z;
        comp[3] = w;
        return *this;
    }
};

template<typename T>
struct matrix3
{
    typedef T value_type;
    
    static const matrix3 kIdentity;
    
    value_type comp[9];
    
    matrix3() {}
    matrix3(value_type v) :
        comp { v,0,0,0,v,0,0,0,v }
    {
    }
    operator value_type*() { return comp; }
    operator const value_type*() const { return comp; }
};

/// A 4x4 uniform
template<typename T>
struct matrix4
{
    typedef T value_type;
    
    static const matrix4 kIdentity;
    
    value_type comp[16];
    
    matrix4() {}
    matrix4(value_type v) :
        comp { v,0,0,0,0,v,0,0,0,0,v,0,0,0,0,v }
    {
    }
    operator float*() { return comp; }
    operator const float*() const { return comp; }
};

//  Forward declarations of class types not defined in this file
template<typename _Point> struct AABB;
template<typename vec_type> struct Plane3;
template<typename vec_type> class Frustrum;

//  Convenience
using vector2f = vector2<float>;
using vector3f = vector3<float>;
using vector4f = vector4<float>;
using matrix3f = matrix3<float>;
using matrix4f = matrix4<float>;

}

#endif
