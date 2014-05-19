cmake_minimum_required(VERSION 2.8.8)

project( rendermodel )

# require CINEKSDK_DIR
if( DEFINED CINEKSDK_DIR )
    set( CMAKE_MODULE_PATH ${CINEKSDK_DIR}/cmake ${CMAKE_MODULE_PATH} )
endif( )

include( CineKSDKBuild )

include_directories( ${CINEKSDK_INCLUDES} )

set( PROJECT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/modelinstance.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spriteinstance.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spriteanimation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sprite.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spritedatabase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tiledatabase.hpp
)

set( PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/spriteinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spriteanimation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sprite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spritedatabase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tiledatabase.cpp
)

# Targets
CineKSDK_Module_Target( rendermodel "${PROJECT_INCLUDES}" "${PROJECT_SOURCES}" )

# Projects
# add_subdirectory( test )

