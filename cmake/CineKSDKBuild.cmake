#
#   CINEKSDK_BUILD     - was this project made from within the CineK SDK
#                         project? (i.e. all modules made within the CineK
#                         project directory.)  For standalone builds, this
#                         is FALSE.
#

# Are we building from the CineK SDK root system?  Only set this if not
# explicitly set by a parent.
#
if( NOT DEFINED CINEKSDK_DIR )
    message( FATAL_ERROR "CINEKSDK_DIR is not defined!  This variable must point to the cinek build project." )
endif( )

if( NOT DEFINED CINEKSDK_ROOT )
    get_filename_component( CINEKSDK_ROOT ${CINEKSDK_DIR} PATH )
endif( )

# Verify Specified Generator
#
if( NOT CINEK_GENERATOR_CHECK )
    if( ${CMAKE_GENERATOR} STREQUAL "Xcode" )
        if( ${CMAKE_VERSION} VERSION_LESS "2.8.10" )
            message( WARNING "CMake is known to have issues with Xcode project generation prior to version 2.8.10.  Please update your CMake." )
        endif( )
    endif( )

    set( CINEK_GENERATOR_CHECK TRUE )
endif( )

# Set up compile flags for a project.
#
if( NOT DEFINED CINEK_C_FLAGS_SET OR NOT CINEK_C_FLAGS_SET )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99" )
    set( CINEK_C_FLAGS_SET TRUE )
endif( )

if( NOT DEFINED CINEK_CXX_FLAGS_SET OR NOT CINEK_CXX_FLAGS_SET )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -stdlib=libc++" )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++" )
    if( NOT DEFINED CINEK_CXX_EXCEPTIONS OR NOT CINEK_CXX_EXCEPTIONS )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti" )
    endif( )
    set( CINEK_CXX_FLAGS_SET TRUE )
endif( )

# Useful macros for builds.
#
macro( CineKSDK_Test_Target LONGNAME SOURCES )
    set( ${LONGNAME}_TEST_INCLUDES ${INCLUDES} PARENT_SCOPE )
    set( ${LONGNAME}_TEST_SOURCES ${SOURCES} PARENT_SCOPE )
endmacro( )

macro( CineKSDK_Module_Target LONGNAME INCLUDES SOURCES )
    install( FILES ${INCLUDES} DESTINATION include/cinek/${LONGNAME} )

    get_property(MODULE_PARENT_DIRECTORY DIRECTORY PROPERTY PARENT_DIRECTORY )
    if( MODULE_PARENT_DIRECTORY )
        set( ${LONGNAME}_INCLUDES ${INCLUDES} PARENT_SCOPE )
        set( ${LONGNAME}_SOURCES ${SOURCES} PARENT_SCOPE )
    endif( MODULE_PARENT_DIRECTORY )
endmacro( )