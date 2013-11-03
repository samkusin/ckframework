###############################################################################
# Discover dependent packages.

# Gtest discovery - consider moving into a cmake module.
#   GTEST_FOUND     TRUE|FALSE
#   GTEST_INCLUDES  Include paths
#   GTEST_LIBS      GTest libraries

if( GTEST_INCLUDES AND GTEST_LIBS )
    # in cache already
    set( GTEST_FOUND TRUE )
else()
    if( DEFINED CINEK_GTEST_DIR )
        find_path( GTEST_INCLUDE_DIR
            gtest/gtest.h
            HINTS "${CINEK_GTEST_DIR}" "${CINEK_GTEST_DIR}/gtest"
            PATH_SUFFIXES "include"
        )
        find_library( GTEST_LIBRARY
            NAMES gtest libgtest 
            HINTS "${CINEK_GTEST_DIR}" "${CINEK_GTEST_DIR}/build" "${CINEK_GTEST_DIR}/gtest/build"
            PATH_SUFFIXES "" "lib"
        )
        find_library( GTEST_MAIN_LIBRARY
            NAMES gtest_main libgtest_main
            HINTS "${CINEK_GTEST_DIR}" "${CINEK_GTEST_DIR}/build" "${CINEK_GTEST_DIR}/gtest/build"
            PATH_SUFFIXES "" "lib"
        )
        set( GTEST_FOUND FALSE )
        if( NOT GTEST_INCLUDE_DIR STREQUAL "GTEST_INCLUDE_DIR-NOTFOUND" )
            if( NOT GTEST_LIBRARY STREQUAL "GTEST_LIBRARY-NOTFOUND" )
                if( NOT GTEST_MAIN_LIBRARY STREQUAL "GTEST_MAIN_LIBRARY-NOTFOUND" )
                    set( GTEST_FOUND TRUE )
                endif( )
            endif( )
        endif( )
        set( GTEST_INCLUDES 
            "${GTEST_INCLUDE_DIR}"
        )
        set( GTEST_LIBS 
            "${GTEST_LIBRARY}"
            "${GTEST_MAIN_LIBRARY}"
        )
    endif()
endif()
