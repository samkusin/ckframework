/**
 * @file    Core/EventBase.hpp
 * @author  Samir Sinha
 * @date    11/15/2014
 * @brief   The base class for all Events handled by EventQueue
 * @copyright Copyright 2014 Samir Sinha.  All rights reserved.
 * @license ISC
 *          (http://www.isc.org/downloads/software-support-policy/isc-license/)
 */

#ifndef CK_Core_SDO_hpp
#define CK_Core_SDO_hpp

#include "cinek/allocator.hpp"
#include "cinek/types.hpp"

#define CINEK_SDO_CLASS(_TYPENAME_) \
    static const SDO::ClassId kClassId; \
    virtual bool sdoIsOfClass(const SDO::ClassId& classId) const { \
        return classId == kClassId; \
    } \
    virtual ClassId sdoClassId() const { return kClassId; } \
    virtual unique_ptr<SDO> clone(Allocator allocator) const { \
        return SDO::clone<_TYPENAME_>(*this, allocator); \
    }

#define CINEK_SDO_SUB_CLASS(_TYPENAME_, _BASE_) \
    static const SDO::ClassId kClassId; \
    virtual bool sdoIsOfClass(const SDO::ClassId& classId) const { \
        return classId == kClassId || _BASE_::sdoIsOfClass(classId); \
    } \
    virtual ClassId sdoClassId() const { return kClassId; } \
    virtual unique_ptr<SDO> clone(Allocator allocator) const { \
        return SDO::clone<_TYPENAME_>(*this, allocator); \
    }

#define CINEK_SDO_CLASSID(_TYPENAME_, ...) \
    const SDO::ClassId _TYPENAME_::kClassId = { __VA_ARGS__ };

namespace cinek {

class SDO
{
public:
    using ClassId = UUID;

public:
    virtual ~SDO() {}

    virtual bool sdoIsOfClass(const ClassId& classId) const = 0;
    virtual ClassId sdoClassId() const = 0;

    virtual unique_ptr<SDO> clone(Allocator allocator) const = 0;

    template<typename _Type>
    unique_ptr<SDO> clone(const _Type& data, Allocator allocator) const {
        SDO* p = ::new(allocator.alloc(sizeof(_Type))) _Type(data);
        return unique_ptr<SDO>(p, allocator);
    }
};

template<typename T, typename U> T sdo_cast(U data) {
    using sdo_type = typename std::remove_pointer<T>::type;
    return (data && data->sdoIsOfClass(sdo_type::kClassId)) ?
        static_cast<T>(data) : nullptr;
}

} /* namespace cinek */

#endif
