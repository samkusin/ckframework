//
//  geometry.inl
//
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

namespace ckm {

template<typename vec_type>
auto Plane3<vec_type>::testPoint(const value_type& testPt) const -> scalar
{
    const vec_type ptv = testPt - pt;
    return dot(ptv, normal);
}


template<typename vec_type>
Frustrum<vec_type>::Frustrum() :
    _nearZ(0),
    _farZ(0),
    _aspect(0)
{
}

template<typename vec_type>
Frustrum<vec_type>::Frustrum
(
    scalar nearZ,
    scalar farZ,
    scalar fov,
    scalar aspect
) :
    _nearZ(nearZ),
    _farZ(farZ),
    _aspect(aspect),
    _fovRadians(fov)
{
    const vec_type xLeft(1,0,0);
    const vec_type xRight(1,0,0);
    const vec_type yUp(0,1,0);
    const vec_type yDown(0,-1,0);
    const vec_type zFwd(0,0,-1);
    const vec_type zBack(0,0,1);

    auto fovTan2 = 2 * tan(_fovRadians/2);
    auto nearH = fovTan2 * nearZ;
    auto nearW = nearH * aspect;
    auto farH = fovTan2 * farZ;
    auto farW = farH * aspect;
    
    auto nearCenter = zFwd * nearZ;
    auto farCenter = zFwd * farZ;

    auto nearTL = nearCenter + (yUp * (nearH/2)) + (xLeft * (nearW/2));
    auto nearBL = nearCenter + (yDown * (nearH/2)) + (xLeft * (nearW/2));
    auto farTL = farCenter + (yUp * (farH/2)) + (xLeft * (farW/2));
    
    _shell[kLeftX].normal = ckm::normalize(ckm::cross(farTL - nearTL, nearBL - nearTL));
    _shell[kLeftX].pt = nearTL;
    
    auto nearTR = nearCenter + (yUp * (nearH/2)) + (xRight * (nearW/2));
    auto nearBR = nearCenter + (yDown * (nearH/2)) + (xRight * (nearW/2));
    auto farTR = farCenter + (yUp * (farH/2)) + (xRight * (farW/2));

    _shell[kRightX].normal = ckm::normalize(ckm::cross(nearBR - nearTR, farTR - nearTR));
    _shell[kRightX].pt = nearTR;
    
    _shell[kTopY].normal = ckm::normalize(ckm::cross(farTR - nearTR, nearTL - nearTR));
    _shell[kTopY].pt = farTR;
    
    auto farBL = farCenter + (yDown * (farH/2)) + (xLeft* (farW/2));
    auto farBR = farCenter + (yDown * (farH/2)) + (xRight * (farW/2));
    
    _shell[kBottomY].normal = ckm::normalize(ckm::cross(farBR - farBL, nearBL - farBL));
    _shell[kBottomY].pt = farBL;
    
    //  near and far planes have trivial normals
    _shell[kNearZ].normal = zFwd;
    _shell[kNearZ].pt = nearCenter;
    _shell[kFarZ].normal = zBack;
    _shell[kFarZ].pt = farCenter;
}

template<typename vec_type>
template<typename mat_type>
Frustrum<vec_type> Frustrum<vec_type>::transform
(
    const mat_type& basis,
    const vec_type& translate
)
const
{
    Frustrum frustrum;
    
    frustrum._aspect = _aspect;
    frustrum._fovRadians = _fovRadians;
    frustrum._nearZ = _nearZ;
    frustrum._farZ = _farZ;
    
    struct Fn
    {
        const mat_type* basis;
        const vec_type* translate;
        typedef Plane3<vec_type> Plane;
        
        Plane operator()(const Plane& pl) const {
            Plane ret = { *basis * pl.normal, *basis * pl.pt };
            ret.pt += *translate;
            return ret;
        }
    };
    Fn fn = { &basis, &translate };
    
    //  rotate our normals and points.  translate our points
    std::transform(_shell.begin(), _shell.end(), frustrum._shell.begin(), fn);
    
    return frustrum;
}

template<typename vec_type>
bool Frustrum<vec_type>::testAABB(const AABB<vec_type>& aabb) const
{
    if (_nearZ == _farZ)
        return false;
    
    // http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
    bool intersect = testAABBWithPlane(aabb, kNearZ) &&
                testAABBWithPlane(aabb,kFarZ) &&
                testAABBWithPlane(aabb,kLeftX) &&
                testAABBWithPlane(aabb,kRightX) &&
                testAABBWithPlane(aabb,kTopY) &&
                testAABBWithPlane(aabb,kBottomY);
   
    return intersect;
}

template<typename vec_type>
bool Frustrum<vec_type>::testAABBWithPlane
(
    const AABB<vec_type>& aabb,
    Plane planeType
)
const
{
    bool intersect = true;
    auto& plane = _shell[planeType];
    auto& normal = plane.normal;
        
    auto posV = aabb.min;
    if (normal.x >= 0)
        posV.x = aabb.max.x;
    if (normal.y >= 0)
        posV.y = aabb.max.y;
    if (normal.z >= 0)
        posV.z = aabb.max.z;
    
    if (plane.testPoint(posV) < 0)
        return false;   // outside
    /*
    //  tests if the box intersects - if we need to differentiate between
    //  intersect and insde, then uncomment this code and return the
    //  appropriate result.
    //
    Plane3::value_type negV = aabb.max;
    if (normal.x >= 0)
        negV.x = aabb.min.x;
    if (normal.y >= 0)
        negV.y = aabb.min.y;
    if (normal.z >= 0)
        negV.z = aabb.min.z;
    
    if (plane.testPoint(negV) < 0)
        intersect = true;
    */
    
    return intersect;
}

    
}