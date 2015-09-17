//
//  json.hpp
//  Overview
//
//  Created by Samir Sinha on 4/22/14
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef CINEK_JSON_HPP
#define CINEK_JSON_HPP

#include "jsontypes.hpp"

#include "rapidjson/document.h"
#include <cstdlib>

namespace cinek {

    int32_t parseInt(const JsonValue& value, int32_t defaultVal=0);
    uint32_t parseUint(const JsonValue& value, uint32_t defaultVal=0);

    uint32_t parseFlagsToUint(const JsonValue& flagsDef, const char* flags);

} /* namespace cinek */

#endif
