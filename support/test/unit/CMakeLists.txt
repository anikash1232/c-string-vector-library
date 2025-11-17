cmake_minimum_required(VERSION 3.14)  # FetchContent needs 3.14+
project(demo-project LANGUAGES C CXX)

# ---- Languages / standards ----
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Options for your project warnings (kept OFF for third-party) ----
option(PROJECT_WERROR "Treat warnings as errors for project code" ON)

# ---- Sources / includes ----
include_directories("${PROJECT_SOURCE_DIR}/include")
file(GLOB PROJECT_C_SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")

# Main binary (your program)
add_executable(${PROJECT_NAME} ${PROJECT_C_SOURCES})

if (PROJECT_WERROR AND NOT MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()

# ---- GoogleTest (pinned) ----
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Silence the GCC false-positive only on gtest/gmock (not your code)
foreach(tgt gtest gtest_main gmock gmock_main)
  if (TARGET ${tgt} AND NOT MSVC)
    target_compile_options(${tgt} PRIVATE
      -Wno-maybe-uninitialized
      -Wno-error=maybe-uninitialized
    )
  endif()
endforeach()

# ---- Tests ----
enable_testing()

# test sources (C++)
file(GLOB TEST_CPP_SOURCES "${PROJECT_SOURCE_DIR}/test/unit/*.cpp")

# For the test target, reuse your project C sources but drop main.c if present
set(PROJECT_C_SOURCES_NO_MAIN ${PROJECT_C_SOURCES})
list(FILTER PROJECT_C_SOURCES_NO_MAIN EXCLUDE REGEX ".*/main\\.c$")

add_executable(all_tests
  ${PROJECT_C_SOURCES_NO_MAIN}
  ${TEST_CPP_SOURCES}
)

# Link gtest_main; gtest/gmock headers as SYSTEM to keep them quiet in includes
target_link_libraries(all_tests PRIVATE gtest_main)

# Your headers
target_include_directories(all_tests PRIVATE "${PROJECT_SOURCE_DIR}/include")

# Mark gtest headers as system to reduce noise from third-party warnings
if (TARGET gtest)
  get_target_property(_GTEST_INC gtest INTERFACE_INCLUDE_DIRECTORIES)
  if (_GTEST_INC)
    target_include_directories(all_tests SYSTEM PRIVATE ${_GTEST_INC})
  endif()
endif()
if (TARGET gmock)
  get_target_property(_GMOCK_INC gmock INTERFACE_INCLUDE_DIRECTORIES)
  if (_GMOCK_INC)
    target_include_directories(all_tests SYSTEM PRIVATE ${_GMOCK_INC})
  endif()
endif()

# Keep your tests strict if you want
if (PROJECT_WERROR AND NOT MSVC)
  target_compile_options(all_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif (MSVC)
  target_compile_options(all_tests PRIVATE /W4 /WX)
endif()

add_test(NAME all_tests COMMAND all_tests)