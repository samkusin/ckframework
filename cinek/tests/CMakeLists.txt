

add_executable(ckcoretests
    ${PROJECT_SOURCES}
    "cstringstacktests.cpp"
    "ckcoretestmain.cpp"
)

target_include_directories(cinektests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
)

add_test(NAME RunCineKTests COMMAND cinektests)

