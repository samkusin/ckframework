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

include( CineKSDKBuild )

# Define project sources and includes for the parent (if any at this level)
#
set( PROJECT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/ckdefs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rendertypes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/allocator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/string.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/value.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/value_map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intrusive_list.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memorystack.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/objectstack.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/objectpool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/job.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jobqueue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jobscheduler.hpp
)

set( PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memorystack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jobqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jobscheduler.cpp
)

# Build Library
if( CINEK_EXT_PACKAGE_DIR )
    include_directories( "${CINEK_EXT_PACKAGE_DIR}/include" )
endif( )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/.." )
add_library( CKFramework STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )

# Build Tests
add_subdirectory( tests )


