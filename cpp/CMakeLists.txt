cmake_minimum_required(VERSION 2.8.8)

project( cpp )

# require CINEKSDK_DIR 
if( DEFINED CINEKSDK_DIR )
    set( CMAKE_MODULE_PATH ${CINEKSDK_DIR}/cmake ${CMAKE_MODULE_PATH} )
endif( )

include( CineKSDKBuild )

set( PROJECT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/ckalloc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ckdefs.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ckstring.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ckvector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ckmemorypool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ckerrorreporter.hpp
)
set( PROJECT_SOURCES

)

# Targets
CineKSDK_Module_Target( cpp "${PROJECT_INCLUDES}" "${PROJECT_SOURCES}" )

# Projects


