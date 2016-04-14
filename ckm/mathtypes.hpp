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
struct vector2_type
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
    
    vector2_type();
    vector2_type(value_type v);
    vector2_type(value_type x, value_type y);
    vector2_type& set(value_type x, value_type y);
};

/// A 3x1 uniform
template<typename T>
struct vector3_type
{
    typedef T value_type;
    
    static const vector3_type kUnitX;
    static const vector3_type kUnitY;
    static const vector3_type kUnitZ;
    static const vector3_type kZero;
    
    union
    {
        struct { value_type x, y, z; };
        struct { value_type r, g, b; };
        value_type comp[3];
    };
    operator value_type*() { return comp; }
    operator const value_type*() const { return comp; }
    
    vector3_type();
    vector3_type(value_type v);
    vector3_type(value_type x, value_type y, value_type z);
    vector3_type& set(value_type x, value_type y, value_type z);
};

/// A 4x1 uniform
template<typename T>
struct vector4_type
{
    typedef T value_type;
    
    static const vector4_type kUnitX;
    static const vector4_type kUnitY;
    static const vector4_type kUnitZ;
    static const vector4_type kUnitW;
    static const vector4_type kZero;
    
    union
    {
        struct { value_type x, y, z, w; };
        struct { value_type r, g, b, a; };
        value_type comp[4];
    };
    operator value_type*() { return comp; }
    operator const value_type*() const { return comp; }
    
    vector4_type() {}
    vector4_type(value_type v) : comp { v,v,v,v }
    {
    }
    vector4_type(value_type x, value_type y, value_type z, value_type w) :
        comp { x, y, z, w }
    {
    }
    vector4_type(const vector3_type<T>& v, value_type w) :
        comp { v.x, v.y, v.z, w }
    {
    }
    vector4_type& set(value_type x, value_type y, value_type z, value_type w) {
        comp[0] = x;
        comp[1] = y;
        comp[2] = z;
        comp[3] = w;
        return *this;
    }
};

/// A 4x1 uniform
template<typename T>
struct quat_type
{
    typedef T value_type;
    
    static const quat_type kIdentity;
    
    union
    {
        struct { value_type x, y, z, w; };
        value_type comp[4];
    };
    operator value_type*() { return comp; }
    operator const value_type*() const { return comp; }
    
    quat_type() {}
    quat_type(value_type x, value_type y, value_type z, value_type w) :
        comp { x, y, z, w }
    {
    }
    quat_type& set(value_type x, value_type y, value_type z, value_type w) {
        comp[0] = x;
        comp[1] = y;
        comp[2] = z;
        comp[3] = w;
        return *this;
    }
    void makeIdentity() {
        set(value_type(0),value_type(0),value_type(0),value_type(1));
    }
};

template<typename T>
struct matrix3_type
{
    typedef T value_type;
    
    static const matrix3_type kIdentity;
    
    value_type comp[9];
    
    matrix3_type() {}
    matrix3_type(value_type v) :
        comp { v,0,0,0,v,0,0,0,v }
    {
    }
    operator value_type*() { return comp; }
    operator const value_type*() const { return comp; }
    void makeIdentity();
};

/// A 4x4 uniform
template<typename T>
struct matrix4_type
{
    typedef T value_type;
    
    static const matrix4_type kIdentity;
    
    value_type comp[16];
    
    matrix4_type() {}
    matrix4_type(value_type v) :
        comp { v,0,0,0,0,v,0,0,0,0,v,0,0,0,0,v }
    {
    }
    matrix4_type(const matrix3_type<T>& src);
    operator float*() { return comp; }
    operator const float*() const { return comp; }
    void makeIdentity();
};

//  Forward declarations of class types not defined in this file
template<typename _Point> struct AABB;
template<typename vec_type> struct plane;
template<typename vec_type> class frustrum;

//  Convenience
using vector2 = vector2_type<scalar>;
using vector3 = vector3_type<scalar>;
using vector4 = vector4_type<scalar>;
using matrix3 = matrix3_type<scalar>;
using matrix4 = matrix4_type<scalar>;
using plane3 = plane<vector3>;
using quat = quat_type<scalar>;

}

#include "mathtypes.inl"

#endif
