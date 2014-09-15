
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
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -stdlib=libc++" )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++" )
    if( NOT DEFINED CINEK_CXX_EXCEPTIONS OR NOT CINEK_CXX_EXCEPTIONS )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti" )
    endif( )
    set( CINEK_CXX_FLAGS_SET TRUE )
endif( )
