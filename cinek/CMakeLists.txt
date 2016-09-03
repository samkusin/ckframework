cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(cinek VERSION 0.1.0 LANGUAGES C CXX)

# All subprojects
set(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/uuid.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memorystack.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cstringstack.cpp"
   )

if(CMAKE_BUILD_TYPE EQUAL Debug OR CMAKE_BUILD_TYPE EQUAL RelWithDebInfo)
    set(PROJECT_SOURCES
        ${PROJECT_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/debug.c"
        )
endif()

set(PROJECT_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/ckdefs.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ckopts.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/uuid.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/allocator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/buffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memorystack.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cstringstack.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/managed_handle.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/managed_handle.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/objectpool.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/objectpool.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/managed_dictionary.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/intrusive_list.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/circular_queue.hpp"
   )

get_directory_property(HAS_PARENT_DIRECTORY PARENT_DIRECTORY)

if(HAS_PARENT_DIRECTORY)
    set(cinek_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)
    set(cinek_INCLUDES ${PROJECT_INCLUDES} PARENT_SCOPE)
endif(HAS_PARENT_DIRECTORY)


