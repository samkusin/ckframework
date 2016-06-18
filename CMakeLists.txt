cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(ckframework VERSION 0.1.0 LANGUAGES C CXX)

option(INCLUDE_CINEK_PROJECT    "CineK Common Utilities" ON)
option(INCLUDE_CKMSG_PROJECT    "CineK Messaging" ON)
option(INCLUDE_CKJSON_PROJECT   "CineK JSON (rapidjson)" ON)
option(INCLUDE_CKM_PROJECT      "CineK Math" ON)
option(INCLUDE_CKIO_PROJECT     "CineK File IO" ON)
option(INCLUDE_CKFILE_PROJECT   "CineK File Streaming" ON)
option(INCLUDE_CKTASK_PROJECT   "CineK Task Scheduler" ON)

option(DISABLE_CKFRAMEWORK_TESTS    "Do not build tests" ON)
option(DISABLE_CKFRAMEWORK_TARGETS  "Do not build targets" ON)
option(ENABLE_CXX_EXCEPTIONS    "Enable C++ Exceptions" OFF)
option(ENABLE_CXX_RTTI          "Enable C++ RTTI" OFF)

# Testing
if(NOT DISABLE_TESTS)
    enable_testing()
endif()

# All subprojects
set(PROJECT_SOURCES  )
set(PROJECT_INCLUDES  )

macro(declare_subproject name)
    #add_subdirectory(${name})
    source_group(${name} FILES ${${name}_SOURCES} ${${name}_INCLUDES})
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${${name}_SOURCES})
    set(PROJECT_INCLUDES ${PROJECT_INCLUDES} ${${name}_INCLUDES})
endmacro(declare_subproject)

if(INCLUDE_CINEK_PROJECT)
    set(cinek_SOURCES
        "cinek/memory.c"
        "cinek/debug.c"
        "cinek/uuid.cpp"
       )
    set(cinek_INCLUDES
        "cinek/ckdefs.h"
        "cinek/ckopts.h"
        "cinek/debug.h"
        "cinek/memory.h"
        "cinek/uuid.hpp"
        "cinek/allocator.hpp"
        "cinek/buffer.hpp"
        "cinek/memorystack.hpp"
        "cinek/cstringstack.hpp"
        "cinek/managed_handle.hpp"
        "cinek/managed_handle.inl"
        "cinek/objectpool.hpp"
        "cinek/objectpool.inl"
        "cinek/managed_dictionary.hpp"
        "cinek/intrusive_list.hpp"
        "cinek/circular_queue.hpp"
       )
    declare_subproject(cinek)
endif()

if(INCLUDE_CKMSG_PROJECT)
    set(ckmsg_SOURCES
        "ckmsg/endpoint.cpp"
        )
    set(ckmsg_INCLUDES
        "ckmsg/buffer.hpp"
        "ckmsg/message.hpp"
        "ckmsg/endpoint.hpp"
        "ckmsg/endpoint.inl"
        "ckmsg/messenger.hpp"
        "ckmsg/messenger.inl"
        "ckmsg/client.hpp"
        "ckmsg/client.inl"
        "ckmsg/server.hpp"
        "ckmsg/server.inl"
       )
    declare_subproject(ckmsg)
endif()

if(INCLUDE_CKJSON_PROJECT)
    file(GLOB_RECURSE RAPIDJSON_INCLUDES
         RELATIVE "ckjson"
         rapidjson/*.h
         )
    set(ckjson_SOURCES
        "ckjson/json.cpp"
        )
    set(ckjson_INCLUDES
        "ckjson/jsontypes.hpp"
        "ckjson/json.hpp"
        "ckjson/jsonstreambuf.hpp"
        ${RAPIDJSON_INCLUDES}
        )
    declare_subproject(ckjson)
endif()

if(INCLUDE_CKIO_PROJECT)
    set(ckio_SOURCES
        "ckio/file.c"
       )
    set(ckio_INCLUDES
        "ckio/file.h"
       )
    declare_subproject(ckio)
endif()

if(INCLUDE_CKFILE_PROJECT)
    set(ckfilestream_SOURCES
        "ckfilestream/file.cpp"
       )
    set(ckfilestream_INCLUDES
        "ckfilestream/file.hpp"
        "ckfilestream/filestreambuf.hpp"
       )
    declare_subproject(ckfilestream)
endif()

if(INCLUDE_CKTASK_PROJECT)
    set(cktask_SOURCES )
    set(cktask_INCLUDES
        "cktask/task.hpp"
        "cktask/taskscheduler.hpp"
        "cktask/task.inl"
        "cktask/taskscheduler.inl"
       )
    declare_subproject(cktask)
endif()

if(INCLUDE_CKM_PROJECT)
    set(ckm_SOURCES )
    set(ckm_INCLUDES
        "ckm/aabb.hpp"
        "ckm/mathtypes.hpp"
        "ckm/mathtypes.inl"
        "ckm/math.inl"
        "ckm/equation.hpp"
        "ckm/equation.inl"
        "ckm/keyframe.hpp"
        "ckm/keyframe.inl"
        "ckm/sequence.hpp"
        "ckm/sequence.inl"
       )
    declare_subproject(ckm)
endif()

# Testing
if(NOT DISABLE_CKFRAMEWORK_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Returns the files accumulated for the entire cinek project
get_directory_property(HAS_PARENT_DIRECTORY PARENT_DIRECTORY)

if(HAS_PARENT_DIRECTORY)
    set(CKFRAMEWORK_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)
    set(CKFRAMEWORK_INCLUDES ${PROJECT_INCLUDES} PARENT_SCOPE)
endif()

# Setup target properties for building the static library
if(NOT DISABLE_CKFRAMEWORK_TARGETS)
    add_library(ckframework STATIC "${PROJECT_SOURCES}")

    target_include_directories(ckframework PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    set_property(TARGET ckframework PROPERTY C_STANDARD 99)
    target_compile_features(ckframework PUBLIC
        cxx_alias_templates
        cxx_auto_type
        cxx_decltype
        cxx_defaulted_functions
        cxx_deleted_functions
        cxx_explicit_conversions
        cxx_extended_friend_declarations
        cxx_lambdas
        cxx_nullptr
        cxx_range_for
        cxx_override
        cxx_static_assert
        cxx_variadic_macros
        cxx_variadic_templates
        cxx_strong_enums
        )

    if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID EQUAL GNU)
        target_compile_options(ckframework PRIVATE -Wall -Wextra -pedantic)

        if(NOT ENABLE_CXX_EXCEPTIONS)
            target_compile_options(ckframework PUBLIC -fno-exceptions)
        endif()
        if(NOT ENABLE_CXX_RTTI)
            target_compile_options(ckframework PRIVATE -fno-rtti)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID EQUAL MSVC)
        if(NOT ENABLE_CXX_EXCEPTIONS)
            target_compile_options(ckframework PUBLIC /EHs-c-)
        endif()
        if(NOT ENABLE_CXX_RTTI)
            target_compile_options(ckframework PRIVATE /GR-)
        endif()
    endif()

    install(TARGETS ckframework
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
    install(FILES ${PROJECT_INCLUDES}
        DESTINATION include
        )

endif(NOT DISABLE_CKFRAMEWORK_TARGETS)
