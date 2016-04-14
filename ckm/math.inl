//
//  math.inl
//  SampleCommon
//
//  Created by Samir Sinha on 2/15/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

namespace ckm
{
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

template<typename T> matrix4_type<T>& quatToMatrix
(
    matrix4_type<T>& mtx,
    quat_type<T> const& q
)
{
    const T q2x = q.x + q.x;
    const T q2y = q.y + q.y;
    const T q2z = q.z + q.z;
    const T q2w = q.w + q.w;
    const T q2x2 = q2x * q.x;
    const T q2y2 = q2y * q.y;
    const T q2z2 = q2z * q.z;
    const T q2xy = q2x * q.y;
    const T q2wz = q2w * q.z;
    const T q2xz = q2x * q.z;
    const T q2wy = q2w * q.y;
    const T q2yz = q2y * q.z;
    const T q2wx = q2w * q.x;
    
    mtx.comp[0] = T(1) - (q2y2 + q2z2);
    mtx.comp[1] = q2xy - q2wz;
    mtx.comp[2] = q2xz + q2wy;
    mtx.comp[3] = T(0);
    
    mtx.comp[4] = q2xy + q2wz;
    mtx.comp[5] = T(1) - (q2x2 + q2z2);
    mtx.comp[6] = q2yz - q2wx;
    mtx.comp[7] = T(0);
    
    mtx.comp[8] = q2xz - q2wy;
    mtx.comp[9] = q2yz + q2wx;
    mtx.comp[10] = T(1) - (q2x2 + q2y2);
    mtx.comp[11] = T(0);
    
    mtx.comp[12] = T(0);
    mtx.comp[13] = T(0);
    mtx.comp[14] = T(0);
    mtx.comp[15] = T(1);
    
    return mtx;
}

template<typename T> matrix4_type<T>& matrixFromQuatAndTranslate
(
    matrix4_type<T>& mtx,
    quat_type<T> const& q,
    vector3_type<T> const& v
)
{
    quatToMatrix(mtx, q);
    mtx[12] = v[0];
    mtx[13] = v[1];
    mtx[14] = v[2];
    return mtx;
}


template<typename T> vector3_type<T>& forwardFromQuat
(
    vector3_type<T>& v,
    quat_type<T> const& q
)
{
    const T q2x = q.x + q.x;
    const T q2y = q.y + q.y;
    const T q2w = q.w + q.w;
    const T q2x2 = q2x * q.x;
    const T q2y2 = q2y * q.y;
    const T q2xz = q2x * q.z;
    const T q2wy = q2w * q.y;
    const T q2yz = q2y * q.z;
    const T q2wx = q2w * q.x;
    
    v.comp[0] = q2xz - q2wy;
    v.comp[1] = q2yz + q2wx;
    v.comp[2] = T(1) - (q2x2 + q2y2);
    
    return v;
}

template<typename T> vector3_type<T>& sideFromQuat
(
    vector3_type<T>& v,
    quat_type<T> const& q
)
{
    const T q2x = q.x + q.x;
    const T q2y = q.y + q.y;
    const T q2z = q.z + q.z;
    const T q2w = q.w + q.w;
    const T q2y2 = q2y * q.y;
    const T q2z2 = q2z * q.z;
    const T q2xy = q2x * q.y;
    const T q2wz = q2w * q.z;
    const T q2xz = q2x * q.z;
    const T q2wy = q2w * q.y;
    
    v.comp[0] = T(1) - (q2y2 + q2z2);
    v.comp[1] = q2xy - q2wz;
    v.comp[2] = q2xz + q2wy;
    
    return v;
}

template<typename T> vector3_type<T>& upFromQuat
(
    vector3_type<T>& v,
    quat_type<T> const& q
)
{
    const T q2x = q.x + q.x;
    const T q2y = q.y + q.y;
    const T q2z = q.z + q.z;
    const T q2w = q.w + q.w;
    const T q2x2 = q2x * q.x;
    const T q2z2 = q2z * q.z;
    const T q2xy = q2x * q.y;
    const T q2wz = q2w * q.z;
    const T q2yz = q2y * q.z;
    const T q2wx = q2w * q.x;
    
    v.comp[0] = q2xy + q2wz;
    v.comp[1] = T(1) - (q2x2 + q2z2);
    v.comp[2] = q2yz - q2wx;
    
    return v;
}

template<typename T> quat_type<T>& matrixToQuat
(
    quat_type<T>& q,
    matrix4_type<T> const& mtx
)
{
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    const T tr = mtx[0] + mtx[5] + mtx[10];
    if (tr > 0) {
        //  classic case
        const T qw4 = sqrt(tr+T(1)) * T(2);
        q.w = qw4 * T(0.25);
        q.x = (mtx[9]-mtx[6])/qw4;
        q.y = (mtx[2]-mtx[8])/qw4;
        q.z = (mtx[4]-mtx[1])/qw4;
    }
    else if ((mtx[0] > mtx[5]) && (mtx[0] > mtx[10])) {
        const T qx4 = sqrt(T(1) + mtx[0] - mtx[5] - mtx[10]) * T(2);
        q.w = (mtx[9]-mtx[6])/qx4;
        q.x = qx4 * T(0.25);
        q.y = (mtx[1]+mtx[4])/qx4;
        q.z = (mtx[2]+mtx[8])/qx4;
    }
    else if (mtx[5] > mtx[10]) {
        const T qy4 = sqrt(T(1) + mtx[5] - mtx[0] - mtx[10]) * T(2);
        q.w = (mtx[2]-mtx[8])/qy4;
        q.x = (mtx[1]+mtx[4])/qy4;
        q.y = qy4 * T(0.25);
        q.z = (mtx[6]+mtx[9])/qy4;
    }
    else {
        const T qz4 = sqrt(T(1) + mtx[10] - mtx[0] - mtx[5]) * T(2);
        q.w = (mtx[4]-mtx[1])/qz4;
        q.x = (mtx[2]+mtx[8])/qz4;
        q.y = (mtx[6]+mtx[9])/qz4;
        q.z = qz4 * T(0.25);
    }
    return q;
}

template<typename T> quat_type<T>& eulerToQuat
(
    quat_type<T>& q,
    T ax,
    T ay,
    T az
)
{
    const T ex_2 = ax*0.5f;
    const T ey_2 = ay*0.5f;
    const T ez_2 = az*0.5f;
    const T cx = cos(ex_2);
    const T cy = cos(ey_2);
    const T cz = cos(ez_2);
    const T sx = sin(ex_2);
    const T sy = sin(ey_2);
    const T sz = sin(ez_2);
    q.comp[0] = cy*cz*sx - sy*sz*cx;
    q.comp[1] = cx*cz*sy + sx*sz*cy;
    q.comp[2] = cx*cy*sz - sx*sy*cz;
    q.comp[3] = cx*cy*cz + sx*sy*sz;
    
    return q;
}

template<typename T> vector3_type<T>& translateFromMatrix
(
    vector3_type<T>& v,
    matrix4_type<T> const& mtx
)
{
    v[0] = mtx[12];
    v[1] = mtx[13];
    v[2] = mtx[14];
    return v;
}


}