

add_executable(ckcoretests
    "cstringstacktests.cpp"
    "ckcoretestmain.cpp"
)

target_include_directories(ckcoretests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
)

target_link_libraries(ckcoretests ckcore)

enable_testing()

add_test(NAME RunCKCoreTests COMMAND ckcoretests)

