//
//  math.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 2/15/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#ifndef CINEK_MATH_HPP
#define CINEK_MATH_HPP

#include "mathtypes.hpp"

namespace ckm {

//  functions
template<typename T=scalar> T abs(T v);
template<typename T=scalar> T cos(T r);
template<typename T=scalar> T acos(T a);
template<typename T=scalar> T sin(T r);
template<typename T=scalar> T asin(T a);
template<typename T=scalar> T tan(T r);
template<typename T=scalar> T atan(T a);
template<typename T=scalar> T radians(T degrees);
template<typename T=scalar> T degrees(T radians);

//  operations
template<typename vec_type> vec_type& add(vec_type& r, vec_type const& a, vec_type const& b);
template<typename vec_type> vec_type& sub(vec_type& r, vec_type const& a, vec_type const& b);
template<typename vec_type> vec_type& scale(vec_type& r, vec_type const& v, typename vec_type::value_type s);

template<typename vec_type> vec_type& cross(vec_type& r, vec_type const& x, vec_type const& y);
template<typename vec_type> typename vec_type::value_type dot(vec_type const& v0,vec_type const& v1);
template<typename vec_type> typename vec_type::value_type vectorLength(vec_type const& v);

//  val_type can either be vector or matrix
template<typename val_type> val_type& inverse(val_type& o, val_type const& m);
template<typename val_type> val_type& normalize(val_type& o, val_type const& v);
template<typename val_type, typename val_type2> val_type& mul(val_type& r, val_type const& v, val_type2 const& v2);

//  operator overloads
template<typename vec_type> vec_type operator+(vec_type const& v0, vec_type const& v1);
template<typename vec_type> vec_type operator-(vec_type const& v0, vec_type const& v1);
template<typename vec_type> vec_type operator*(vec_type const& v, typename vec_type::value_type s);

}

#endif /* CINEK_MATH_HPP */
