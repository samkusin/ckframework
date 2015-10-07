//
//  EngineMath.hpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Engine_Math_hpp
#define Overview_Engine_Math_hpp

#include "mathtypes.hpp"

namespace ckm {

template<typename scalar> scalar epsilon();
template<typename scalar> scalar pi();
template<typename type> type zero();

template<typename vec> vec cross(vec const& x, vec const& y);
template<typename vec> typename vec::value_type dot(vec const& v0, vec const& v1);

template<typename scalar> scalar cos(scalar r);
template<typename scalar> scalar acos(scalar a);
template<typename scalar> scalar sin(scalar r);
template<typename scalar> scalar asin(scalar a);
template<typename scalar> scalar radians(scalar degrees);
template<typename scalar> scalar degrees(scalar radians);
template<typename mat> mat inverse(mat const& m);

template<typename vec> vec normalize(vec const& v);
template<typename vec> typename vec::value_type vectorLength(vec const& v);

//  quaternion math
/*
    inline quat inverse(quat const& q) {
        return glm::inverse(q);
    }
    
    inline mat4 mtx4x4FromQuat(quat const& q) {
        return glm::mat4_cast(q);
    }
    
    inline mat3 mtx3x3FromQuat(quat const& q) {
        return glm::mat3_cast(q);
    }
    
    inline quat quatFromMtx4x4(mat4 const& m) {
        return glm::quat_cast(m);
    }
    
    inline quat quatFromMtx3x3(mat3 const& m) {
        return glm::quat_cast(m);
    }
    
    inline quat quatFromAngleAndAxis(scalar angle, vec3 const& axis) {
        return glm::angleAxis(angle, axis);
    }
    
    inline vec3 axisFromQuat(quat const& q) {
        return glm::axis(q);
    }
    
    inline scalar angleFromQuat(quat const& q) {
        return glm::angle(q);
    }
    
    //  Referemce
    //  lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
    //  Inputs must be normalized (unit vectors)
    //  The returned quaternion is normalized.
    //
    quat quatFromUnitVectors(vec3 const& v0, vec3 const& v1);
*/
}

#endif
