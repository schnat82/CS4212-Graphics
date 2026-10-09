
cmake_minimum_required(VERSION 3.22)

project(
    cs4212StarterCode
    VERSION 1.0
    LANGUAGES CXX
)

list(
    APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake/Modules"
)

option(
    ENABLE_CPPCHECK
    "Enable static analysis with cppcheck"
    OFF
)

option(
    ENABLE_CLANG_TIDY
    "Enable static analysis with clang-tidy"
    OFF
)

if(ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck)

    if(CPPCHECK)
        set(
            CMAKE_CXX_CPPCHECK
            ${CPPCHECK}
            --suppress=missingInclude
            --enable=all
            --inline-suppr
            --inconclusive
            -i
            ${CMAKE_SOURCE_DIR}
        )
    else()
        message(
            SEND_ERROR
            "cppcheck requested but executable not found"
        )
    endif()
endif()

if(ENABLE_CLANG_TIDY)
    find_program(CLANGTIDY clang-tidy-13)

    if(CLANGTIDY)
        set(CMAKE_C_CLANG_TIDY ${CLANGTIDY})
        set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY})
    else()
        message(
            SEND_ERROR
            "clang-tidy could not be located."
        )
    endif()
endif()

# Use C++20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# PNG and ZLIB dependencies.
find_package(PNG REQUIRED)

include(FetchContent)

# Catch2 testing framework.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.0
)

FetchContent_MakeAvailable(Catch2)

# Boost program options.
if(WIN32)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(
    Boost REQUIRED
    COMPONENTS program_options
)

# OpenGL dependencies.
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# OpenMP is optional.
# If unavailable, the renderer runs serially.
find_package(OpenMP COMPONENTS CXX)

# Build the main utility library.
add_subdirectory(src)

# Build rendering examples.
add_subdirectory(examples)

# Build OpenGL examples.
add_subdirectory(OpenGL)

# Enable unit testing.
enable_testing()
add_subdirectory(utests)
