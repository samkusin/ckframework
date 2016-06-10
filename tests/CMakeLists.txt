
macro(create_test name target files)
  add_executable(${target}
        ${${name}_SOURCES}
        ${files}
        "cstringstacktests.cpp"
        "ckcoretestmain.cpp"
  )

  target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")

  set_property(TARGET ${target} PROPERTY C_STANDARD 99)
  target_compile_features(${target} PUBLIC
        cxx_alias_templates
        cxx_auto_type
        cxx_decltype
        cxx_defaulted_functions
        cxx_deleted_functions
        cxx_explicit_conversions
        cxx_extended_friend_declarations
        cxx_lambdas
        cxx_nullptr
        cxx_range_for
        cxx_override
        cxx_static_assert
        cxx_variadic_macros
        cxx_variadic_templates
        cxx_strong_enums
  )
  # Use exceptions for CPP testing
  if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID EQUAL GNU)
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic)
        if(NOT ENABLE_CXX_RTTI)
            target_compile_options(${target} PRIVATE -fno-rtti)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID EQUAL MSVC)
        if(NOT ENABLE_CXX_RTTI)
            target_compile_options(${target} PRIVATE /GR-)
        endif()
    endif()
endmacro(create_test)

if(INCLUDE_CINEK_PROJECT)
    create_test(cinek cinektests "cstringstacktests.cpp;ckcoretestmain.cpp")
    add_test(NAME RunCineKTests COMMAND cinektests)
endif()


