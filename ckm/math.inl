//
//  math.inl
//  Overview
//
//  Created by Samir Sinha on 8/25/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

namespace ckm
{
#ifdef CKM_MATH_IMPLEMENTATION

template<> float epsilon<float>() {
    return 1e-6f;
}
template<> float pi<float>() {
    return bx::pi;
}
template<> float zero() {
    return 0.0f;
}
template<> float cos<float>(float r) {
    return cosf(r);
}
template<> float acos(float a) {
    return acosf(a);
}
template<> float sin<float>(float r) {
    return sinf(r);
}
template<> float asin<float>(float a) {
    return asinf(a);
}
template<> float radians<float>(float degrees) {
    return pi<float>() * degrees / 180.0f;
}
template<> float degrees<float>(float radians) {
    return 180.0f * radians / pi<float>();
}

/*
    quat quatFromUnitVectors(vec3 const& v0, vec3 const& v1)
    {
        vec3 w = cross(v0, v1);
        quat q = quat(1 + dot(v0, v1), w.x,w.y,w.z);
        return normalize(q);
    }

    mat4 mtx4x4RotateFromAngleAndAxis
    (
        mat4 const& m,
        scalar angle,
        vec3 const& axis
    )
    {
        return glm::rotate(m, angle, axis);
    }
*/

#endif
}
