cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(cinek VERSION 0.2.0 LANGUAGES C CXX)

set(CINEK_ENTITY_TYPE "64"
    CACHE STRING "CKEntity value type width (32 or 64)")

set(CINEK_CORE_SOURCES
    "cinek/debug.c"
    "cinek/types.cpp"
    "cinek/file.cpp"
    "cinek/allocator.cpp"
    "cinek/memorystack.cpp"
    "cinek/cstringstack.cpp"
    "cinek/filestreambuf.cpp"
    "cinek/string.cpp"
    "cinek/task.cpp"
    "cinek/taskscheduler.cpp"
    )

set(CINEK_CORE_INCLUDES
    "cinek/ckdefs.h"
    "cinek/ckopts.h"
    "cinek/types.hpp"
    "cinek/debug.h"
    "cinek/buffer.hpp"
    "cinek/allocator.hpp"
    "cinek/memorystack.hpp"
    "cinek/cstringstack.hpp"
    "cinek/objectpool.hpp"
    "cinek/objectpool.inl"
    "cinek/circular_queue.hpp"
    "cinek/instrusive_list.hpp"
    "cinek/managed_dictionary.hpp"
    "cinek/managed_handle.inl"
    "cinek/string.hpp"
    "cinek/vector.hpp"
    "cinek/map.hpp"
    "cinek/file.hpp"
    "cinek/filestreambuf.hpp"
    "cinek/task.hpp"
    "cinek/taskscheduler.hpp"
    )

file(GLOB_RECURSE CINEK_RAPIDJSON_INCLUDES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ckjson/rapidjson/*.h
     )

set(CINEK_JSON_SOURCES
    "ckjson/json.cpp"
    "ckjson/jsoncollectionloader.cpp"
    )

set(CINEK_JSON_INCLUDES
    "ckjson/jsontypes.hpp"
    "ckjson/json.hpp"
    "ckjson/jsoncollectionloader.hpp"
    "ckjson/jsonstreambuf.hpp"
    ${CINEK_RAPIDJSON_INCLUDES}
    )

set(CINEK_ENTITY_SOURCES
    "ckentity/entitystore.cpp"
    )

set(CINEK_ENTITY_INCLUDES
    "ckentity/entity.h"
    "ckentity/entitystore.hpp"
    )
    
set(CINEK_MSG_SOURCES
    "ckmsg/messenger.cpp"
    )
    
set(CINEK_MSG_INCLUDES
    "ckmsg/buffer.hpp"
    "ckmsg/client.hpp"
    "ckmsg/client.inl"
    "ckmsg/message.hpp"
    "ckmsg/messenger.hpp"
    "ckmsg/server.hpp"
    "ckmsg/server.inl"
    )

add_library(ckcore STATIC
    ${CINEK_CORE_SOURCES}
    ${CINEK_JSON_SOURCES}
    ${CINEK_ENTITY_SOURCES}
    ${CINEK_CKMSG_SOURCES}
    )

target_include_directories(ckcore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

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


set(CINEK_LOCAL_COMPILE_OPTIONS )

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CINEK_LOCAL_COMPILE_OPTIONS "-fno-exceptions")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CINEK_LOCAL_COMPILE_OPTIONS "-fno-exceptions")
endif()

target_compile_options(ckcore
    PRIVATE ${CINEK_LOCAL_COMPILE_OPTIONS}
)

set(CINEK_GLOBAL_COMPILE_DEFINITIONS )

if("${CINEK_ENTITY_TYPE}" STREQUAL "32")
    set(CINEK_GLOBAL_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
        "CINEK_ENTITY_TYPE_32=1")
elseif("${CINEK_ENTITY_TYPE}" STREQUAL "64")
    set(CINEK_GLOBAL_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
        "CINEK_ENTITY_TYPE_32=0")
else()
    message(FATAL_ERROR "CONFIG - Specified CKEntity Type "
            "${CINEK_ENTITY_TYPE} bits. Must be either 32 or 64.")
endif()

message(STATUS "CONFIG - CKEntity Type is ${CINEK_ENTITY_TYPE} bits.")

target_compile_definitions(ckcore
    PUBLIC ${CINEK_GLOBAL_COMPILE_DEFINITIONS}
)

# Testing
enable_testing()

add_subdirectory(cinek/tests)


