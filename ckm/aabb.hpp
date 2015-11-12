///
/// @file
/// Utilities for an Axis Aligned Bounding Box (AABB)
///
/// @author     Samir Sinha
/// @date       07/16/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef CINEK_MATH_AABB_HPP
#define CINEK_MATH_AABB_HPP

#include <array>

namespace ckm {

    /// @struct AABB
    /// @brief  An axis-aligned box in 3D space
    /// @detail Default template definition assumes Point and Matrix types
    ///         follow a particular concept (i.e. GLM compliant types.)  For
    //          custom point/matrix types, specialization is required.
    ///
    template<class _Point>
    struct AABB
    {
        typedef _Point                       point_type;
        typedef AABB<_Point>                 this_type;
        typedef typename _Point::value_type  value_type;
        
        point_type min;          ///< The min coordinate
        point_type max;          ///< The max coordinate

        /// Default Constructor
        ///
        AABB();
        /// Constructor
        /// @param length   Sets min to -length and max to +length
        ///
        AABB(value_type length);
        /// Constructor
        /// @param halfDim  Radius of the box
        ///
        AABB(const point_type& halfDim);
        /// Constructor
        /// @param min  Min point of the box
        /// @param max  Max point of the box
        ///
        AABB(const point_type& min, const point_type& max);
        /// Checks if the box is valid.  The box could be flat (planar), or even
        /// a line if the min/max points share common coordinates.
        ///
        /// @return False if the box has a zero size
        ///
        operator bool() const;
        /// Calculates the size of the box as a Point
        /// @return The x,y,z sizes of the box
        ///
        point_type dimensions() const;
        /// Determines if this box lies within a box
        /// @param  box The box to test
        /// @return True if the this box lies within the supplied box
        ///
        bool inside(const AABB& box) const;
        /// Determines if this box contains a point
        /// @param  point The point to test
        /// @return True if the this box contains the supplied point
        ///
        bool contains(const point_type& pt) const;
        /// Determines if this box lies outside a box
        /// @param  box The box to test
        /// @return True if this box lies outside the supplied box
        ///
        bool outside(const AABB& box) const;
        /// Determines if a box intersects this box
        /// @param  box The box to test
        /// @return True if the box intersects this box
        ///
        bool intersects(const AABB& box) const;
        /// Test intersection with sphere
        /// @param  center  The sphere's center point
        /// @param  radius  The sphere's radius
        ///
        bool intersectsWithSphere(const point_type& center, value_type radius) const;
        /*
         * /// Calculates the intersection between a box and this box
         * /// @param  box The box to check
         * /// @return The intersection between this box and the passed in box
         * ///
         * AABB intersection(const AABB& box) const;
        */
        /// @param  box The box to test
        /// @return True if the box equals this box
        bool operator==(const AABB& box) const;
        /// @param  box The box to test
        /// @return True if the box does not equal this box
        bool operator!=(const AABB& box) const;
        /// @param  pt  Point to offset existing AABB
        /// @return Reference to the offsetted AABB
        ///
        AABB& operator+=(const point_type& pt);
        /// Zeros out the box
        ///
        void clear();
        /// Calculates the center point of our box
        /// @return The center point
        ///
        point_type center() const;
        /// Generate vertices from this AABB
        /// @param verts An array of vertices to be filled in
        ///
        void generateVertices(std::array<point_type, 8>& verts);
        /// Rotates, generating a new AABB bounding the rotated box
        /// @param mtxRot Rotation and Translation matrix
        ///
        template<class _Matrix>
        void rotate(const _Matrix& mtxRot);
        /// Merges another AABB with this AABB.  The result is stored in this
        /// @param  aabb  The AABB to merge
        /// @return A reference to this AABB
        ///
        AABB& merge(const AABB& aabb);
        
        /// @return The volume of the AABB based on the point type
        ///
        value_type volume() const;
        
        static value_type squared(value_type val) {
            return val*val;
        }
    };
   
    ////////////////////////////////////////////////////////////////////////////
    template<class _Point>
    AABB<_Point>::AABB()
    {
        clear();
    }

    template<class _Point>
    AABB<_Point>::AABB(const point_type& halfDim)
    {
        clear();
        min -= halfDim;
        max += halfDim;
    }

    template<class _Point>
    AABB<_Point>::AABB(const point_type& min, const point_type& max) : min(min), max(max)
    {
    }

    template<class _Point>
    AABB<_Point>::operator bool() const
    {
        return min != max;
    }

    template<class _Point>
    typename AABB<_Point>::point_type AABB<_Point>::dimensions() const
    {
        return max - min;
    }

    template<class _Point>
    bool AABB<_Point>::intersects(const AABB<_Point>& box) const
    {
        return !outside(box);
    }

    template<class _Point>
    bool AABB<_Point>::operator==(const AABB<_Point>& box) const
    {
        return min != box.min || max != box.max;
    }

    template<class _Point>
    bool AABB<_Point>::operator!=(const AABB<_Point>& box) const
    {
        return min != box.min || max != box.max;
    }

    template<class _Point>
    typename AABB<_Point>::point_type AABB<_Point>::center() const
    {
        return (min + max) * 0.5f;
    }
    
    template<class _Point>
    AABB<_Point>& AABB<_Point>::operator+=(const point_type& pt)
    {
        min += pt;
        max += pt;
        return *this;
    }

    template<class _Point>
    AABB<_Point> operator+(const AABB<_Point>& box, const typename AABB<_Point>::point_type& off)
    {
        AABB<_Point> result;
        result.min = box.min + off;
        result.max = box.max + off;
        return result;
    }
    
    template<class _Point>
    AABB<_Point>::AABB(value_type length)
    {
        min = point_type(-length, -length, -length);
        max = point_type(length, length, length);
    }

    template<class _Point>
    void AABB<_Point>::clear()
    {
        min.x = min.y = min.z = 0;
        max.x = max.y = max.z = 0;
    }

    template<class _Point>
    bool AABB<_Point>::inside(const AABB<_Point>& box) const
    {
        return (min.x >= box.min.x && max.x <= box.max.x &&
                min.y >= box.min.y && max.y <= box.max.y &&
                min.z >= box.min.z && max.z <= box.max.z);
    }

    template<class _Point>
    bool AABB<_Point>::contains(const point_type& pt) const
    {
        return (pt.x >= min.x && pt.x <= max.x &&
                pt.y >= min.y && pt.y <= max.y &&
                pt.z >= min.z && pt.z <= max.z);
    }

    template<class _Point>
    bool AABB<_Point>::outside(const AABB<_Point>& box) const
    {
        return (min.x > box.max.x || box.min.x > max.x ||
                min.y > box.max.y || box.min.y > max.y ||
                min.z > box.max.z || box.min.z > max.z);
    }
    
    template<class _Point>
    void AABB<_Point>::generateVertices(std::array<point_type, 8>& verts)
    {
        //  -y vertices
        verts[0].x = min.x;     // v0 (-x,-y,-z)
        verts[0].y = min.y;
        verts[0].z = min.z;
        verts[1].x = min.x;     // v1 (-x,-y,+z)
        verts[1].y = min.y;
        verts[1].z = max.z;
        verts[2].x = max.x;     // v2 (+x,-y,+z)
        verts[2].y = min.y;
        verts[2].z = max.z;
        verts[3].x = max.x;     // v3 (+x,-y,-z)
        verts[3].y = min.y;
        verts[3].z = min.z;
        
        //  +y vertices
        verts[4].x = min.x;     // v4 (-x,+y,-z)
        verts[4].y = max.y;
        verts[4].z = min.z;
        verts[5].x = min.x;     // v5 (-x,+y,+z)
        verts[5].y = max.y;
        verts[5].z = max.z;
        verts[6].x = max.x;     // v6 (+x,+y,+z)
        verts[6].y = max.y;
        verts[6].z = max.z;
        verts[7].x = max.x;     // v7 (+x,+y,-z)
        verts[7].y = max.y;
        verts[7].z = min.z;
    }

    template<class _Point>
    template<class _Matrix>
    void AABB<_Point>::rotate(const _Matrix& mtxRot)
    {
        std::array<point_type, 8> box;
        generateVertices(box);
        
        typedef typename _Matrix::col_type column;
        
        for (auto& pt : box)
        {
            column col = column(pt, (typename column::value_type)1);
            col *= mtxRot;
            
            if (pt.x < min.x)
                min.x = pt.x;
            if (pt.x > max.x)
                max.x = pt.x;

            if (pt.y < min.y)
                min.y = pt.y;
            if (pt.y > max.y)
                max.y = pt.y;

            if (pt.z < min.z)
                min.z = pt.z;
            if (pt.z > max.z)
                max.z = pt.z;
        }
    }
    
    template<class _Point>
    AABB<_Point>& AABB<_Point>::merge(const AABB<_Point>& aabb)
    {
        if (aabb.min.x < min.x)
            min.x = aabb.min.x;
        if (aabb.min.y < min.y)
            min.y = aabb.min.y;
        if (aabb.min.z < min.z)
            min.z = aabb.min.z;
        if (aabb.max.x > max.x)
            max.x = aabb.max.x;
        if (aabb.max.y > max.y)
            max.y = aabb.max.y;
        if (aabb.max.z > max.z)
            max.z = aabb.max.z;
        
        return *this;
    }
    
    template<class _Point>
    typename AABB<_Point>::value_type AABB<_Point>::volume() const
    {
        return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
    }
    
    template<class _Point>
    bool AABB<_Point>::intersectsWithSphere
    (
        const point_type& center,
        value_type radius
    )
    const
    {
        auto dist2 = squared(radius);
        
        if (center.x < min.x)
            dist2 -= squared(center.x-min.x);
        else if (center.x > max.x)
            dist2 -= squared(center.x-max.x);
        if (center.y < min.y)
            dist2 -= squared(center.y-min.y);
        else if (center.y > max.y)
            dist2 -= squared(center.y-max.y);
        if (center.z < min.z)
            dist2 -= squared(center.z-min.z);
        else if (center.z > max.z)
            dist2 -= squared(center.z-max.z);
        
        return dist2 > 0;
    }

}   // namespace ckm

#endif

