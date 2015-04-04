/**
 * @file    Core/Message.hpp
 * @author  Samir Sinha
 * @date    10/31/2014
 * @brief   Common Event types
 * @copyright Copyright 2014 Samir Sinha.  All rights reserved.
 * @license ISC
 *          (http://www.isc.org/downloads/software-support-policy/isc-license/)
 */

#ifndef CK_Core_Message_hpp
#define CK_Core_Message_hpp

#include "cinek/sdo/sdo.hpp"

namespace cinek {

using MessageDelegateHandle = Handle;

struct Message
{
    enum
    {
        kEventMessage    = 0x01,
        kCommandMessage  = 0x02,
        kResultMessage   = 0x04,
        kErrorMessage    = 0x80,
    };

    using ClassId = uint32_t;
    using SequenceId = uint32_t;

    constexpr static ClassId createClassId
    (
        uint8_t flags,
        uint8_t source,
        uint16_t method
    )
    {
        return ((ClassId)(flags)<<24) + ((ClassId)(source)<<16) + method;
    }

    Message() = default;
    Message(ClassId cid, unique_ptr<SDO>&& dataPtr, SequenceId seqId);

    unique_ptr<SDO> data;
    ClassId classId;
    SequenceId sequenceId;
};

inline Message::Message(ClassId cid, unique_ptr<SDO>&& dataPtr, SequenceId seqId) :
    data(std::move(dataPtr)),
    classId(cid),
    sequenceId(seqId)
{
}

} /* namespace cinek */

#endif
