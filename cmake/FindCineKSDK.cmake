###############################################################################
# Discover dependent packages.

# CineK SDK discovery
#   CINEKSDK_FOUND     TRUE|FALSE
#   CINEKSDK_INCLUDES  Include paths
#   CINEKSDK_LIBS      SDK libraries

set( _CINEK_COMPONENTS )
foreach( _CINEK_MODULE ${CineKSDK_FIND_COMPONENTS} )
    list( APPEND _CINEK_COMPONENTS ${_CINEK_MODULE} )
endforeach( )
list( APPEND _CINEK_COMPONENTS "core" )
list( REMOVE_DUPLICATES _CINEK_COMPONENTS )

set( CINEKSDK_FOUND FALSE )
if( NOT DEFINED CINEKSDK_INCLUDES )
    set( CINEKSDK_INCLUDES )
endif( )
if( NOT DEFINED CINEKSDK_LIBS )
    set( CINEKSDK_LIBS )
endif( )

# If any components are specified, find the global cinek include paths
# testing against headers specific to component.
#
foreach( _CINEK_MODULE ${_CINEK_COMPONENTS} )  
    if( ${_CINEK_MODULE} STREQUAL "core" )
        find_path( CINEKSDK_${_CINEK_MODULE}_INCLUDE_DIR 
            cinek/${_CINEK_MODULE}/cktypes.h
            HINTS "${CINEKSDK_ROOT}"
            PATH_SUFFIXES "include"
        )
    elseif( ${_CINEK_MODULE} STREQUAL "cpp" )
        find_path( CINEKSDK_${_CINEK_MODULE}_INCLUDE_DIR 
            cinek/${_CINEK_MODULE}/ckdefs.hpp
            HINTS "${CINEKSDK_ROOT}"
            PATH_SUFFIXES "include"
        )
    else( )
        find_path( CINEKSDK_${_CINEK_MODULE}_INCLUDE_DIR
            cinek/${_CINEK_MODULE}/types.h cinek/${_CINEK_MODULE}/types.hpp
            HINTS "${CINEKSDK_ROOT}"
            PATH_SUFFIXES "include"
        )
    endif( )
   
    # append found files to our includes and libs variables.
    if( NOT CINEKSDK_${_CINEK_MODULE}_INCLUDE_DIR STREQUAL "CINEKSDK_${_CINEK_MODULE}_INCLUDE_DIR-NOTFOUND" )
        #message( STATUS "${CINEKSDK_${_CINEK_MODULE}_INCLUDE_DIR}")
        list( APPEND CINEKSDK_INCLUDES "${CINEKSDK_${_CINEK_MODULE}_INCLUDE_DIR}" )
    endif( )
endforeach( )

list( APPEND CINEKSDK_INCLUDES "${CINEKSDK_DIR}/ext" )

set( _CINEK_INCLUDE_COUNT 0 )
set( _CINEK_LIB_COUNT 0 )
list( LENGTH CINEKSDK_INCLUDES _CINEK_INCLUDE_COUNT )
list( LENGTH CINEK_LIB_COUNT _CINEK_LIB_COUNT )

if( _CINEK_INCLUDE_COUNT OR _CINEK_LIB_COUNT )
    set( CINEKSDK_FOUND TRUE )
else( )
    unset( CINEKSDK_INCLUDES )
    unset( CINEKSDK_LIBS )
endif( )
