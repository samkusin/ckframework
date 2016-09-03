//
//  message.cpp
//  ckframework
//
//  Created by Samir Sinha on 6/23/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include "message.hpp"

namespace ckmsg {

    void Message::serialize(uint8_t *out)
    {
        *reinterpret_cast<uint32_t*>(out) = htobe32(_sender.id);
        out += sizeof(uint32_t);
        *reinterpret_cast<ClassId*>(out) = htobe32(_classId);
        out += sizeof(uint32_t);
        *reinterpret_cast<uint32_t*>(out) = htobe32(_seqId);
        out += sizeof(uint32_t);
        *reinterpret_cast<TagId*>(out) = htobe32(_tagId);
        out += sizeof(uint32_t);
        *reinterpret_cast<uint16_t*>(out) = htobe16(_customFlags);
        //  skip flags
    }
    
    void Message::unserialize(const uint8_t* in, uint16_t sz)
    {
        if (sz < sizeof(uint32_t))
            return;
        _sender.id = be32toh(*reinterpret_cast<const uint32_t*>(in));
        sz -= sizeof(uint32_t); in += sizeof(uint32_t);
        if (sz < sizeof(uint32_t))
            return;
        _classId = be32toh(*reinterpret_cast<const uint32_t*>(in));
        sz -= sizeof(uint32_t); in += sizeof(uint32_t);
        if (sz < sizeof(uint32_t))
            return;
        _seqId = be32toh(*reinterpret_cast<const uint32_t*>(in));
        sz -= sizeof(uint32_t); in += sizeof(uint32_t);
        if (sz < sizeof(uint32_t))
            return;
        _tagId = be32toh(*reinterpret_cast<const uint32_t*>(in));
        sz -= sizeof(uint32_t); in += sizeof(uint32_t);
        if (sz < sizeof(uint16_t))
            return;
        _customFlags = be16toh(*reinterpret_cast<const uint16_t*>(in));
        //  skip flags
    }
}
