//
//  math.inl
//  SampleCommon
//
//  Created by Samir Sinha on 2/15/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

namespace ckm
{
template<> float abs<float>(float v) {
    return std::fabs(v);
}
template<> float cos<float>(float r) {
    return std::cosf(r);
}
template<> float acos(float a) {
    return std::acosf(a);
}
template<> float sin<float>(float r) {
    return std::sinf(r);
}
template<> float asin<float>(float a) {
    return std::asinf(a);
}
template<> float tan<float>(float r) {
    return std::tanf(r);
}
template<> float atan<float>(float a) {
    return std::atanf(a);
}
template<> float radians<float>(float degrees) {
    return kPi * degrees / 180.0f;
}
template<> float degrees<float>(float radians) {
    return 180.0f * radians / kPi;
}

template<typename vec_type> vec_type operator+
(
    vec_type const& v0,
    vec_type const& v1
)
{
    vec_type r;
    add(r, v0, v1);
    return r;
}

template<typename vec_type> vec_type operator-
(
    vec_type const& v0,
    vec_type const& v1
)
{
    vec_type r;
    sub(r, v0, v1);
    return r;
}

template<typename vec_type> vec_type operator*
(
    vec_type const& v,
    typename vec_type::value_type s
)
{
    vec_type r;
    scale(r, v, s);
    return r;
}

}