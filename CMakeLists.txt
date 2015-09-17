cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(cinek VERSION 0.2.0 LANGUAGES C CXX)

set(CINEK_ENTITY_TYPE "64"
    CACHE STRING "CKEntity value type width (32 or 64)")

set(PROJECT_CORE_SOURCES
    "cinek/types.cpp"
    "cinek/debug.cpp"
    "cinek/file.cpp"
    "cinek/allocator.cpp"
    "cinek/memorystack.cpp"
    "cinek/cstringstack.cpp"
    "cinek/filestreambuf.cpp"
    "cinek/string.cpp"
    )

set(PROJECT_CORE_INCLUDES
    "cinek/ckdefs.h"
    "cinek/ckopts.h"
    "cinek/types.hpp"
    "cinek/circular_queue.hpp"
    "cinek/buffer.hpp"
    "cinek/debug.hpp"
    "cinek/file.hpp"
    "cinek/allocator.hpp"
    "cinek/memorystack.hpp"
    "cinek/cstringstack.hpp"
    "cinek/objectpool.hpp"
    "cinek/filestreambuf.hpp"
    "cinek/string.hpp"
    "cinek/vector.hpp"
    "cinek/map.hpp"
    )

file(GLOB_RECURSE PROJECT_RAPIDJSON_INCLUDES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ckjson/rapidjson/*.h
     )

set(PROJECT_JSON_SOURCES
    "ckjson/json.cpp"
    "ckjson/jsoncollectionloader.cpp"
    )

set(PROJECT_JSON_INCLUDES
    "ckjson/jsontypes.hpp"
    "ckjson/json.hpp"
    "ckjson/jsoncollectionloader.hpp"
    "ckjson/jsonstreambuf.hpp"
    ${PROJECT_RAPIDJSON_INCLUDES}
    )

set(PROJECT_ENTITY_SOURCES
    "ckentity/datarowset.cpp"
    "ckentity/entitydatatable.cpp"
    "ckentity/entitygroup.cpp"
    "ckentity/entitygroupmap.cpp"
    "ckentity/entitystore.cpp"
    )

set(PROJECT_ENTITY_INCLUDES
    "ckentity/entity.h"
    "ckentity/datarowset.hpp"
    "ckentity/vectordatarowset.hpp"
    "ckentity/entitydatatable.hpp"
    "ckentity/entitygroup.hpp"
    "ckentity/entitygroupmap.hpp"
    "ckentity/entitystore.hpp"
    )

add_library(ckcore STATIC
    ${PROJECT_CORE_SOURCES}
    ${PROJECT_JSON_SOURCES}
    ${PROJECT_ENTITY_SOURCES}
    )

target_include_directories(ckcore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

#
#if(CMAKE_COMPILER_IS_GNUCXX)
#    set(PROJECT_COMPILE_OPTIONS "-std=c++11")
#endif()
#
#if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#    set(PROJECT_COMPILE_OPTIONS "-std=c++11")
#endif()
#
#target_compile_options(ckcore
#    PUBLIC ${PROJECT_COMPILE_OPTIONS}
#)

target_compile_features(ckcore
    PUBLIC
        cxx_auto_type
        cxx_rvalue_references
        cxx_alias_templates
        cxx_constexpr
        cxx_decltype
        cxx_lambdas
        cxx_variadic_macros
        cxx_uniform_initialization
        cxx_defaulted_functions
        cxx_static_assert
        cxx_strong_enums
        cxx_right_angle_brackets
)

set(PROJECT_COMPILE_DEFINITIONS )

if("${CINEK_ENTITY_TYPE}" STREQUAL "32")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
        "CINEK_ENTITY_TYPE_32=1")
elseif("${CINEK_ENTITY_TYPE}" STREQUAL "64")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
        "CINEK_ENTITY_TYPE_32=0")
else()
    message(FATAL_ERROR "CONFIG - Specified CKEntity Type "
            "${CINEK_ENTITY_TYPE} bits. Must be either 32 or 64.")
endif()

message(STATUS "CONFIG - CKEntity Type is ${CINEK_ENTITY_TYPE} bits.")

target_compile_definitions(ckcore
    PUBLIC ${PROJECT_COMPILE_DEFINITIONS}
)

# Testing
enable_testing()

add_subdirectory(cinek/tests)

