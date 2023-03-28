cmake_minimum_required(VERSION 3.22)

# Set the default build type
set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_C_STANDARD 99)
    set(FETCHCONTENT_QUIET FALSE)
    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    if(K2_BUILD_DOCS)
        find_package(Doxygen)
        message(STATUS "Building k2 docs.")
        if(Doxygen_FOUND)
            add_subdirectory(docs)
        endif()
    endif()

    # Set cmake module path
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
endif()
Include(FetchContent)



# Setup glfw
function(glfw_build)
    set(GLFW_BUILD_EXAMPLES  OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS     OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS      OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL         OFF CACHE INTERNAL "Generate installation target")
    set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE INTERNAL "Use MSVC Runtime")
    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw
            GIT_TAG        3.3.2
            GIT_SHALLOW 1
            GIT_PROGRESS 1
    )

    FetchContent_MakeAvailable(glfw)
endfunction()
glfw_build()

# Setup glad
function(glad_build)
    set(GLAD_PROFILE    "core"  CACHE INTERNAL "")
    set(GLAD_API        "gl="   CACHE INTERNAL "")
    set(GLAD_GENERATOR  "c"     CACHE INTERNAL "")
    FetchContent_Declare(
            glad
            GIT_REPOSITORY https://github.com/Dav1dde/glad
            GIT_TAG        v0.1.34
            GIT_SHALLOW 1
            GIT_PROGRESS 1
    )

    FetchContent_MakeAvailable(glad)
endfunction()
glad_build()

find_package(OpenGL REQUIRED)

project(snake C)
add_executable(snake WIN32 main.c)

target_link_libraries(snake OpenGL::GL glfw glad User32.lib)
target_include_directories(snake PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_options(snake PRIVATE /MT)


target_link_options(
        snake PRIVATE
        /SUBSYSTEM:Windows
)
