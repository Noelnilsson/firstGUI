cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(wx_cmake_fetchcontent_template LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build wxWidgets as a static library
set(wxBUILD_SHARED OFF)

message(STATUS "Fetching wxWidgets...")

FetchContent_Declare(
   wxWidgets
   GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
   GIT_SHALLOW ON
)
FetchContent_MakeAvailable(wxWidgets)

#-------------------------------------------------------------
# IMPORTANT: Include *all* of your .cpp files here
#-------------------------------------------------------------
set(SRCS
    main.cpp
    simpleParticle.cpp
)

# Build the executable named "main"
add_executable(main WIN32 ${SRCS})

# Link against the needed wxWidgets libraries
# You likely also want wxbase in addition to wxcore, wxnet, etc.
target_link_libraries(main PRIVATE 
    wx::base 
    wx::core 
    wx::adv  # For wxTimer and other advanced features
)
