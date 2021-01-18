cmake_minimum_required(VERSION 3.8)

project(opinion-dynamics)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# GLFW
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory( glfw )

# include OpenAL, defines OPENAL_INCLUDE_DIR and OPENAL_LIBRARY
# uses ENV $OPENALDIR for HINT
include( FindOpenAL )
include_directories( ${OPENAL_INCLUDE_DIR} )

# MSVC linker flags
if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

# Set Source
# set( OPINION-DYNAMICS-SRC main.cpp)
set( OPINION-DYNAMICS-SRC graph_test.cpp)

# Add WIN32 Executable
# add_executable(opinion-dynamics WIN32 ${OPINION-DYNAMICS-SRC})
add_executable(opinion-dynamics ${OPINION-DYNAMICS-SRC})
#  link OpenGL, GLFW, and OpenAL
target_link_libraries(opinion-dynamics ${OPENGL_LIBRARIES} glfw ${OPENAL_LIBRARY})
# MSVC project
if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'opinion-dynamics' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT opinion-dynamics )
    endif()
endif()