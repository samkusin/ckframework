cmake_minimum_required(VERSION 2.8.8)

project( cinek )

###############################################################################
# CMake custom variables used to set up the build process.  All subtargets
# will use these variables to discover packages, set compile settings, or other
# modify other build related settings.

# Setup where the CINEK build module is located.  All CineK modules use this value to
# setup the CMAKE_MODULE_PATH
#
set( CINEKSDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    CACHE FILEPATH "The path to the cinek build module where build scripts are located."
)

# Setup the external package directory used for compiling the SDK.
if ( NOT DEFINED CINEK_EXT_PACKAGE_DIR )
    set( CINEK_EXT_PACKAGE_DIR ""
         CACHE FILEPATH "The path to external packages used by the SDK for compilation." )
endif( )

# Setup whether to enable or disable exception handling in CineK SDK systems.
option( CINEK_CXX_EXCEPTIONS 
    "Enabled C++ exception handling for the SDK." OFF 
)

# Used for gtest support for module testing.
set( CINEK_GTEST_DIR ""
    CACHE FILEPATH "The path to the gtest package used for testing."
)

if( CINEK_GTEST_DIR )
    enable_testing()
endif( )

# Used to specify location of local modules.  
if( DEFINED CINEKSDK_DIR )
    set( CMAKE_MODULE_PATH ${CINEKSDK_DIR}/cmake )
endif( )

include( CineKSDKBuild )

# Define project sources and includes for the parent (if any at this level)
#
set( PROJECT_INCLUDES )
set( PROJECT_SOURCES )
set( TEST_SOURCES )

# Enumerate which projects to build based on the existence and contents of modules.txt
#
message( STATUS "Enumerating CineK modules for building:" )

if( NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/modules.txt" )
    message( WARNING "The modules.txt file was not found.  Building no additional modules." )
else()
    file( STRINGS modules.txt CINEK_MODULES )

    foreach( CINEK_MODULE ${CINEK_MODULES} )
        if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${CINEK_MODULE}" )
            message( STATUS "\tModule directory found '${CINEK_MODULE}'" )
            add_subdirectory( ${CINEK_MODULE} )
            set( PROJECT_INCLUDES ${PROJECT_INCLUDES} ${${CINEK_MODULE}_INCLUDES} )
            set( PROJECT_SOURCES ${PROJECT_SOURCES} ${${CINEK_MODULE}_SOURCES} )
            set( TEST_SOURCES ${TEST_SOURCES} ${${CINEK_MODULE}_TEST_SOURCES} )
        else( )
            message( STATUS "\tModule directory not found '${CINEK_MODULE}' (ignoring)" )
        endif( )
    endforeach( CINEK_MODULE )
endif( )

# Build Library
if( CINEK_EXT_PACKAGE_DIR )
    include_directories( "${CINEK_EXT_PACKAGE_DIR}" )
endif( )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/.." )
add_library( ck STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )

# Build Tests
find_package( GTest )

if( GTEST_FOUND )
    include_directories( ${GTEST_INCLUDES} )
    add_executable( cktest ${TEST_SOURCES} )
    set_target_properties( cktest PROPERTIES
        COMPILE_FLAGS "-Wall"
        COMPILE_DEFINITIONS "GTEST_LINKED_AS_SHARED_LIBRARY=1;GTEST_LANG_CXX11=1"
    )
    target_link_libraries( cktest ${GTEST_LIBS} ck )
    add_test( test cktest )
endif( GTEST_FOUND )

