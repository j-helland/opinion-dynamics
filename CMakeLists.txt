cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(opinion-dynamics)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
# GLFW
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory( glfw )

# include FindOpenAL, defines OPENAL_INCLUDE_DIR and OPENAL_LIBRARY
# NOTE: uses ENV $OPENALDIR for HINT
include( FindOpenAL )
include_directories( ${OPENAL_INCLUDE_DIR} )

# include libsnd
# NOTE: uses $LIBSND_INCLUDE_DIR and $LIBSND_LIBRARY
include_directories( $ENV{LIBSND_INCLUDE_DIR} )
# message(STATUS "LIBSND_INCLUDE_DIR=$ENV{LIBSND_INCLUDE_DIR}")

# MSVC linker flags
if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

# Set Source
set( OPINION-DYNAMICS-SRC src/main.cpp)
# set( OPINION-DYNAMICS-SRC src/tests/graph_test.cpp )
# set( OPINION-DYNAMICS-SRC src/tests/voter_model_test.cpp )

# Add WIN32 Executable
# add_executable(opinion-dynamics WIN32 ${OPINION-DYNAMICS-SRC})
add_executable(opinion-dynamics ${OPINION-DYNAMICS-SRC})
#  link OpenGL, GLFW, and OpenAL
target_link_libraries(opinion-dynamics ${OPENGL_LIBRARIES} glfw ${OPENAL_LIBRARY} $ENV{LIBSND_LIBRARY})
# MSVC project
if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'opinion-dynamics' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT opinion-dynamics )
    endif()
endif()

# POST-BUILD
# Copy DLLs into Debug folders (will need to change when we have Debug vs. Release)
add_custom_command(TARGET opinion-dynamics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${PROJECT_SOURCE_DIR}/libs"
    $<TARGET_FILE_DIR:opinion-dynamics>)
