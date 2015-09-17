//
//  jsoncollectionloader.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "jsoncollectionloader.hpp"
#include "jsonstreambuf.hpp"
#include "json.hpp"

#include <cstring>

namespace cinek {

//
// The Unserializer object must implement the following methods:
// bool startCollection(const char* name);
// bool parseAttribute(const char* key, const JsonValue& value);
// bool parseModel(const char* key, JsonValue& object);
// bool endCollection();
bool unserializeFromJSON
(
    JsonCollectionLoader& handler,
    std::streambuf& instream,
    const char* collectionName,
    const char* fieldName /* ="collection" */
)
{
    RapidJsonStdStreamBuf jsonStream(instream);

    JsonDocument jsonDoc;
    jsonDoc.ParseStream<0>(jsonStream);

    if (jsonDoc.HasParseError() || !jsonDoc.IsObject())
    {
        RAPIDJSON_ASSERT(false);
        return false;
    }

    if (!jsonDoc.HasMember(fieldName))
    {
        RAPIDJSON_ASSERT(false);
        return false;
    }

    auto& items = jsonDoc[fieldName];

    int memberCount;
    if (items.IsArray())
    {
        memberCount = items.Size();
    }
    else if (items.IsObject())
    {
        memberCount = (int)(items.MemberEnd() - items.MemberBegin());
    }
    else
    {
        RAPIDJSON_ASSERT(false);
        return false;
    }

    //  parse the collection
    for (auto colIter = jsonDoc.MemberBegin(), colIterEnd = jsonDoc.MemberEnd();
         colIter != colIterEnd;
         ++colIter)
    {
        auto& attribute = *colIter;
        const char* attrKey = attribute.name.GetString();
        if (!strcmp(attrKey, fieldName)) // collections are later
            continue;
        if (!handler.parseAttribute(attrKey, attribute.value))
        {
            RAPIDJSON_ASSERT(false);
            return false;
        }
    }

    if (!handler.startCollection(collectionName, memberCount))
    {
        RAPIDJSON_ASSERT(false);
        return false;
    }

    //  iterate through all models
    if (items.IsObject())
    {
        for (auto modelIter = items.MemberBegin(),
                  modelIterEnd = items.MemberEnd();
             modelIter != modelIterEnd;
             ++modelIter)
        {
            auto& model = *modelIter;
            if (!handler.parseModel(model.name.GetString(), model.value))
            {
                RAPIDJSON_ASSERT(false);
            }
        }
    }
    else
    {
        for (auto modelIter = items.Begin(), modelIterEnd = items.End();
             modelIter != modelIterEnd;
             ++modelIter)
        {
            auto& model = *modelIter;
            if (!handler.parseModel("", model))
            {
                RAPIDJSON_ASSERT(false);
            }
        }
    }

    if (!handler.endCollection())
    {
        RAPIDJSON_ASSERT(false);
        return false;
    }

    return true;
}

} /* namespace cinek */
