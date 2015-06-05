///
/// @file jsontypes.hpp
///
/// @author     Samir Sinha
/// @date       06/03/2015
/// @copyright  Copyright 2015 Cinekine Media
/// @license    The MIT License

#ifndef CINEK_JSON_TYPES_HPP
#define CINEK_JSON_TYPES_HPP

namespace rapidjson {
    class CrtAllocator;
    template<typename BaseAllocatorr> class MemoryPoolAllocator;
    template<typename CharType> struct UTF8;
    template<typename Encoding, typename Allocator> class GenericDocument;
    template<typename Encoding, typename Allocator> class GenericValue;
}

namespace cinek {
    typedef rapidjson::GenericDocument<rapidjson::UTF8<char>,
        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> JsonDocument;
    typedef rapidjson::GenericValue<rapidjson::UTF8<char>,
        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> JsonValue;
}

#endif
