# (c) 2025 Daniele Rapetti
#
# This code is licensed under MIT license (see LICENSE for details)
cmake_minimum_required(VERSION 3.15...3.25)

project(
  UFSCampaignTracker
  VERSION 0.1
  DESCRIPTION "A simple campaign tracker fo Under Falling Sky"
  LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  Djson
  GIT_REPOSITORY https://github.com/Iximiel/djson.git
  GIT_TAG v0.0.2-alpha)
FetchContent_MakeAvailable(Djson)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v5.0.0)
FetchContent_MakeAvailable(ftxui)

add_executable(UFSCampaignTracker main.cpp utils.cpp campaign.cpp save.cpp)

# this is a 3.24 thing set_property(TARGET UFSCampaignTracker PROPERTY
# COMPILE_WARNING_AS_ERROR ON)
if(MSVC)
  target_compile_options(UFSCampaignTracker PRIVATE /W4 /WX)
else()
  target_compile_options(
    UFSCampaignTracker
    PRIVATE -Wall
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wuseless-cast
            -Werror
            # -Wlifetime
  )
endif()

target_link_libraries(
  UFSCampaignTracker
  PRIVATE Djson::Djson
          #
          ftxui::screen ftxui::dom ftxui::component)

add_executable(test test.cpp campaign.cpp save.cpp)
target_link_libraries(test PRIVATE Djson::Djson ftxui::screen ftxui::dom
                                   ftxui::component)
