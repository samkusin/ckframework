cmake_minimum_required(VERSION 2.8.8)

project( CKFramework )

###############################################################################
# CMake custom variables used to set up the build process.  All subtargets
# will use these variables to discover packages, set compile settings, or other
# modify other build related settings.

# Setup the external package directory used for compiling the SDK.
if ( NOT DEFINED CINEK_EXT_PACKAGE_DIR )
    set( CINEK_EXT_PACKAGE_DIR ""
         CACHE FILEPATH "The path to external packages used by the SDK for compilation." )
endif( )

# Setup whether to enable or disable exception handling in CineK SDK systems.
option( CINEK_CXX_EXCEPTIONS
    "Enable C++ exception handling for the SDK." OFF
)

# Used for gtest support for module testing.
option( CINEK_GTEST "Enable testing suite." OFF )

set( CINEK_GTEST_DIR ""
    CACHE FILEPATH "The path to the gtest package used for testing."
)

if( CINEK_GTEST AND CINEK_GTEST_DIR )
    enable_testing()
endif( )

# Used to specify location of local modules.
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

# include once all options and environment variables are defined
include( CineKSDKBuild )

set( PROJECT_DEFINITIONS  )

# Define project sources and includes for the parent (if any at this level)
#
set( MAIN_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/ckdefs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/allocator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/string.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/value.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/value_convert.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intrusive_list.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memorystack.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cstringstack.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/objectstack.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/objectpool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/taskscheduler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filestreambuf.hpp
)
set( MAIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memorystack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cstringstack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/taskscheduler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filestreambuf.cpp
)
source_group( source FILES ${MAIN_SOURCES} )
source_group( include FILES ${MAIN_INCLUDES} )

set( JSON_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/json/json.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json/jsonstreambuf.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json/jsoncollectionloader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/allocators.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/document.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/encodedstream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/encodings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/filereadstream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/filestream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/filewritestream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/prettywriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/rapidjson.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/reader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/stringbuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/writer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/error/en.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/error/error.h
)

set( JSON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/json/json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json/jsoncollectionloader.cpp
)
source_group( source\\json FILES ${JSON_SOURCES} )
source_group( include\\json FILES ${JSON_INCLUDES} )


set( PROJECT_INCLUDES
    ${MAIN_INCLUDES}
    ${JSON_INCLUDES}
)

set( PROJECT_SOURCES
    ${MAIN_SOURCES}
    ${JSON_SOURCES}
)

# Build Library
if( CINEK_EXT_PACKAGE_DIR )
    include_directories( "${CINEK_EXT_PACKAGE_DIR}/include" )
endif( )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/.." )

add_library( CKFramework STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )

set_target_properties( CKFramework PROPERTIES
    COMPILE_DEFINITIONS "${PROJECT_DEFINITIONS}")

# Build Tests
add_subdirectory( tests )
