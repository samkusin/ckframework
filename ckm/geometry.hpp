//
//  EngineGeometry.hpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CINEK_MATH_GEOMETRY_HPP
#define CINEK_MATH_GEOMETRY_HPP

#include "math.hpp"

#include <array>

namespace ckm {

    template<typename vec_type>
    struct plane
    {
        using value_type = vec_type;
        using scalar = typename vec_type::value_type;

        value_type normal;
        value_type pt;

        scalar testPoint(const value_type& testPt) const;
    };

    template<typename vec_type>
    class frustrum
    {
    public:
        enum side
        {
            kNearZ,
            kFarZ,
            kLeftX,
            kRightX,
            kTopY,
            kBottomY,
            kPlaneCount
        };

        using shell = std::array<plane<vec_type>, kPlaneCount>;
        using scalar = typename vec_type::value_type;

        frustrum();
        frustrum(scalar nearZ, scalar farZ, scalar fov, scalar aspect);

        scalar nearZ() const { return _nearZ; }
        scalar farZ() const { return _farZ; }
        scalar fovRadians() const { return _fovRadians; }
        scalar aspect() const { return _aspect; }

        //const shell& shell() const { return _shell; }

        template<typename mat_type>
        frustrum transform(const mat_type& basis, const vec_type& translate) const;

        bool testAABB(const AABB<vec_type>& aabb) const;
        bool testAABBWithPlane(const AABB<vec_type>& aabb, side plane) const;

    private:
        shell _shell;
        scalar _nearZ, _farZ;
        scalar _aspect;
        scalar _fovRadians;
    };

    template<typename scalar_type>
    struct raytest
    {
        enum class result
        {
            kNone,
            kIntersect,
            kCoplanar
        };

        static result planeIntersection
        (
            vector3_type<scalar_type>* intersectPt,
            const vector3_type<scalar_type>& rayOrigin,
            const vector3_type<scalar_type>& rayDir,
            const plane<vector3_type<scalar_type>>& plane
        );
    };

}

#include "geometry.inl"

#endif
