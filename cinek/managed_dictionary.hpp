/**
 * @file    cinek/managed_dictionary.hpp
 * @author  Samir Sinha
 * @date    11/25/2015
 * @brief   Object allocation within a pooled heap
 * @copyright Cinekine
 */

#ifndef CINEK_MANAGED_DICTIONARY_HPP
#define CINEK_MANAGED_DICTIONARY_HPP

#include "objectpool.hpp"

#include <unordered_map>
#include <string>

namespace cinek {

template<typename Handle>
using ManagedDictionary =
    std::unordered_map<std::string, Handle,
                       std::hash<std::string>,
                       std::equal_to<std::string>,
                       std_allocator<std::pair<const std::string, Handle>>>;

template<typename Dictionary>
typename Dictionary::mapped_type registerResource
(
    typename Dictionary::mapped_type::Value&& value,
    typename Dictionary::mapped_type::Owner& pool,
    Dictionary& dictionary,
    const char* name
)
{
    typename Dictionary::mapped_type h;

    auto it = dictionary.end();

    if (name && *name) {
        std::string key = name;
        it = dictionary.find(key);
        if (it != dictionary.end()) {
            h = it->second;
        }
        else {
            it = dictionary.emplace(key, h).first;
        }
    }
    if (h) {
        //  existing handle, emplacing a new material into it
        h.setValue(std::move(value));
    }
    else {
        //  new handle - add it to the dictionary if applicable
        h = pool.add(std::move(value));
        if (it != dictionary.end()) {
            it->second = h;
        }
    }
    return h;
}


template<typename Dictionary>
void unregisterResource(Dictionary& dictionary, const char* name)
{
    std::string key = name;
    auto it = dictionary.find(key);
    if (it != dictionary.end()) {
        dictionary.erase(it);
    }
}

}

#endif