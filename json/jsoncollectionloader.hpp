///
/// @file jsoncollectionloader.hpp
/// The JsonCollectionLoader common class header
///
/// @author     Samir Sinha
/// @date       07/17/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef CINEK_JSON_COLLECTIONLOADER_HPP
#define CINEK_JSON_COLLECTIONLOADER_HPP

#include <cinek/json/json.hpp>
#include <streambuf>

namespace cinek {

/// @class   JsonCollectionLoader
/// @brief   Applications implement JsonCollectionLoader to unserializer
///          a group of ModelCollection objects
///
/// When any of the methods return false, parsing aborts.
///
class JsonCollectionLoader
{
public:
    virtual ~JsonCollectionLoader() {}
    virtual bool startCollection(const char* name, uint32_t modelCount) = 0;
    virtual bool parseAttribute(const char* key, const JsonValue& value) = 0;
    virtual bool parseModel(const char* key, JsonValue& object) = 0;
    virtual bool endCollection() = 0;
};

/// Unserializes an Object Collection from source JSON.
/// The input JSON must be an object containing one or more collections.
/// Applications supply an Unserializer specialized for a particular
/// ModelCollection type.
///
/// @ingroup TileModel
///
/// @param  handler  The Unserializer object that implements the
///                  JsonCollectionLoader interface.
/// @param  instream The input stream
/// @return False if parsing fails
///
bool unserializeFromJSON(JsonCollectionLoader& handler,
                         std::streambuf& instream,
                         const char* collectionName,
                         const char* fieldName="collection");

} /* namespace cinek */

#endif
