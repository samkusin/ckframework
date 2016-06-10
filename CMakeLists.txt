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
    add_subdirectory(${name})
    source_group(${name} FILES ${${name}_SOURCES} ${${name}_INCLUDES})
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${${name}_SOURCES})
    set(PROJECT_INCLUDES ${PROJECT_INCLUDES} ${${name}_INCLUDES})
endmacro(declare_subproject)

if(INCLUDE_CINEK_PROJECT)
    declare_subproject(cinek)
endif()

if(INCLUDE_CKMSG_PROJECT)
    declare_subproject(ckmsg)
endif()

if(INCLUDE_CKJSON_PROJECT)
    declare_subproject(ckjson)
endif()

if(INCLUDE_CKIO_PROJECT)
    declare_subproject(ckio)
endif()

if(INCLUDE_CKFILE_PROJECT)
    declare_subproject(ckfilestream)
endif()

if(INCLUDE_CKTASK_PROJECT)
    declare_subproject(cktask)
endif()

if(INCLUDE_CKM_PROJECT)
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
endif(NOT DISABLE_CKFRAMEWORK_TARGETS)
