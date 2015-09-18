

add_executable(ckentitytests
    "testcomponent.cpp"
    "ckentitytestmain.cpp"
)


target_include_directories(ckentitytests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
)

target_link_libraries(ckentitytests ckcore)

enable_testing()

add_test(NAME RunCKEntityTests COMMAND ckentitytests)

