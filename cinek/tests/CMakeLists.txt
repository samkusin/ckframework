

add_executable(ckcoretests
    "cstringstacktests.cpp"
    "ckcoretestmain.cpp"
)

target_compile_features(ckcoretests
    PUBLIC
        cxx_auto_type
        cxx_rvalue_references
        cxx_alias_templates
        cxx_constexpr
        cxx_decltype
        cxx_lambdas
        cxx_variadic_macros
        cxx_uniform_initialization
        cxx_defaulted_functions
        cxx_static_assert
        cxx_strong_enums
        cxx_right_angle_brackets
)

target_include_directories(ckcoretests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
)

target_link_libraries(ckcoretests ckcore)

enable_testing()

add_test(NAME RunCKCoreTests COMMAND ckcoretests)

