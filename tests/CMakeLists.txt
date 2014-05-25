
find_package( GTest )

if( GTEST_FOUND )
    set( TEST_SOURCES
         ${CMAKE_CURRENT_SOURCE_DIR}/ckjobtest.cpp
    )
    include_directories( ${GTEST_INCLUDES} )
    add_executable( cktest ${TEST_SOURCES} )
    set_target_properties( cktest PROPERTIES
        COMPILE_FLAGS "-Wall"
        COMPILE_DEFINITIONS "GTEST_LINKED_AS_SHARED_LIBRARY=1;GTEST_LANG_CXX11=1"
    )
    target_link_libraries( cktest ${GTEST_LIBS} ck )
    add_test( test cktest )
endif( GTEST_FOUND )
