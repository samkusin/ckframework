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
    template<typename T> vector2_type<T>::vector2_type() {}
    template<typename T> vector2_type<T>::vector2_type(value_type v) :
        comp { v, v }
    {
    }
    template<typename T> vector2_type<T>::vector2_type
    (
        value_type x,
        value_type y
    ) :
        comp { x, y }
    {
    }
    
    template<typename T> vector2_type<T>& vector2_type<T>::set
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
    template<typename T> const vector3_type<T> vector3_type<T>::kUnitX
        = { T(1.0), T(0.0), T(0.0) };
    template<typename T> const vector3_type<T> vector3_type<T>::kUnitY
        = { T(0.0), T(1.0), T(0.0) };
    template<typename T> const vector3_type<T> vector3_type<T>::kUnitZ
        = { T(0.0), T(0.0), T(1.0) };
    template<typename T> const vector3_type<T> vector3_type<T>::kZero
        = { T(0.0), T(0.0), T(0.0) };
    
    template<typename T> vector3_type<T>::vector3_type() {}
    template<typename T> vector3_type<T>::vector3_type(value_type v) :
        comp { v, v, v }
    {
    }
    template<typename T> vector3_type<T>::vector3_type
    (
        value_type x,
        value_type y,
        value_type z
    ) :
        comp { x, y, z }
    {
    }
    
    template<typename T> vector3_type<T>& vector3_type<T>::set
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
    template<typename T> const vector4_type<T> vector4_type<T>::kUnitX
        = { T(1.0), T(0.0), T(0.0), T(0.0) };
    template<typename T> const vector4_type<T> vector4_type<T>::kUnitY
        = { T(0.0), T(1.0), T(0.0), T(0.0) };
    template<typename T> const vector4_type<T> vector4_type<T>::kUnitZ
        = { T(0.0), T(0.0), T(1.0), T(0.0) };
    template<typename T> const vector4_type<T> vector4_type<T>::kUnitW
        = { T(0.0), T(0.0), T(0.0), T(1.0) };
    template<typename T> const vector4_type<T> vector4_type<T>::kZero
        = { T(0.0), T(0.0), T(0.0), T(0.0) };
    
    //
    // quat Inlined Implementation
    //
    template<typename T> const quat_type<T> quat_type<T>::kIdentity
        = { T(0.0), T(0.0), T(0.0), T(1.0) };
    
        
    template<typename T> const matrix3_type<T> matrix3_type<T>::kIdentity = matrix3_type<T>(T(1.0));
    
    //
    // Matrix4 Inlined Implementation
    //
    template<typename T> const matrix4_type<T> matrix4_type<T>::kIdentity = matrix4_type<T>(T(1.0));
    
    template<typename T> matrix4_type<T>::matrix4_type(const matrix3_type<T>& src)
    {
        comp[0] = src.comp[0];
        comp[1] = src.comp[1];
        comp[2] = src.comp[2];
        comp[3] = T(0);
        comp[4] = src.comp[3];
        comp[5] = src.comp[4];
        comp[6] = src.comp[5];
        comp[7] = T(0);
        comp[8] = src.comp[6];
        comp[9] = src.comp[7];
        comp[10] = src.comp[8];
        comp[11] = T(0);
        comp[12] = T(0);
        comp[13] = T(0);
        comp[14] = T(0);
        comp[15] = T(1);
    }
}
