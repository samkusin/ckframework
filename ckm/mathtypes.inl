//
//  mathtypes.inl
//  Overview
//
//  Created by Samir Sinha on 8/25/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

namespace ckm
{
    //
    // Vector2 Inlined Implementation
    //
    template<typename T> vector2<T>::vector2() {}
    template<typename T> vector2<T>::vector2(value_type v) :
        comp { v, v }
    {
    }
    template<typename T> vector2<T>::vector2
    (
        value_type x,
        value_type y
    ) :
        comp { x, y }
    {
    }
    
    template<typename T> vector2<T>& vector2<T>::set
    (
        value_type x,
        value_type y
    )
    {
        comp[0] = x;
        comp[1] = y;
        return *this;
    }


    //
    // Vector3 Inlined Implementation
    //
    template<typename T> const vector3<T> vector3<T>::kUnitX
        = { T(1.0), T(0.0), T(0.0) };
    template<typename T> const vector3<T> vector3<T>::kUnitY
        = { T(0.0), T(1.0), T(0.0) };
    template<typename T> const vector3<T> vector3<T>::kUnitZ
        = { T(0.0), T(0.0), T(1.0) };
    template<typename T> const vector3<T> vector3<T>::kZero
        = { T(0.0), T(0.0), T(0.0) };
    
    template<typename T> vector3<T>::vector3() {}
    template<typename T> vector3<T>::vector3(value_type v) :
        comp { v, v, v }
    {
    }
    template<typename T> vector3<T>::vector3
    (
        value_type x,
        value_type y,
        value_type z
    ) :
        comp { x, y, z }
    {
    }
    
    template<typename T> vector3<T>& vector3<T>::set
    (
        value_type x,
        value_type y,
        value_type z
    )
    {
        comp[0] = x;
        comp[1] = y;
        comp[2] = z;
        return *this;
    }
    
    //
    // Vector4 Inlined Implementation
    //
    template<typename T> const vector4<T> vector4<T>::kUnitX
        = { T(1.0), T(0.0), T(0.0), T(0.0) };
    template<typename T> const vector4<T> vector4<T>::kUnitY
        = { T(0.0), T(1.0), T(0.0), T(0.0) };
    template<typename T> const vector4<T> vector4<T>::kUnitZ
        = { T(0.0), T(0.0), T(1.0), T(0.0) };
    template<typename T> const vector4<T> vector4<T>::kUnitW
        = { T(0.0), T(0.0), T(0.0), T(1.0) };
    template<typename T> const vector4<T> vector4<T>::kZero
        = { T(0.0), T(0.0), T(0.0), T(0.0) };
    
        
    template<typename T> const matrix3<T> matrix3<T>::kIdentity = matrix3<T>(T(1.0));
    
    template<typename T> const matrix4<T> matrix4<T>::kIdentity = matrix4<T>(T(1.0));

}
