//
//  json.cpp
//  Overview
//
//  Created by Samir Sinha on 4/22/14
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "json.hpp"
#include <cstring>

namespace cinek {

    int32_t parseInt(const JsonValue& value, int32_t defaultVal)
    {
        if (value.IsString())
            return strtol(value.GetString(), NULL, 0);
        else if (value.IsInt())
            return value.GetInt();
        return defaultVal;
    }
    uint32_t parseUint(const JsonValue& value, uint32_t defaultVal)
    {
        if (value.IsString())
            return strtoul(value.GetString(), NULL, 0);
        else if (value.IsUint())
            return value.GetUint();
        return defaultVal;
    }

#if CINEK_GLM_ENABLED
    glm::vec3 parseVec3(const JsonValue& value, const glm::vec3& defaultVal)
    {
        if (!value.IsObject())
            return defaultVal;
        glm::vec3 v = defaultVal;
        auto& vx = value["x"];
        auto& vy = value["y"];
        auto& vz = value["z"];
        if (vx.IsDouble()) v.x = vx.GetDouble();
        if (vy.IsDouble()) v.y = vy.GetDouble();
        if (vz.IsDouble()) v.z = vz.GetDouble();
        return v;
    }
#endif

    uint32_t parseFlagsToUint(const JsonValue& flagsDef, const char* flags)
    {
        uint32_t result = 0;

        const char* end = flags + strlen(flags);
        const char* current = flags;
        while (current < end)
        {
            const char* next = strchr(current, ',');
            if (!next)
                next = end;
            size_t slen = next - current;
            if (slen)
            {
                for (auto memberIt = flagsDef.MemberBegin(),
                                     memberItEnd = flagsDef.MemberEnd();
                     memberIt != memberItEnd;
                     ++memberIt)
                {
                    const auto& member = *memberIt;
                    const char* flagStr = member.name.GetString();
                    if (!strncmp(current, flagStr, slen) && strlen(flagStr) == slen)
                    {
                        result |= parseUint(member.value);
                        break;
                    }
                }
            }

            current = next + 1;
        }

        return result;
    }

} /* namespace cinek */
