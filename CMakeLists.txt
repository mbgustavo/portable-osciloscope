cmake_minimum_required(VERSION 3.21)

project(
  portable-osciloscope
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(
  OSCILLOSCOPE_BUILD_TESTS
  "Build tests"
  ON)
option(
  OSCILLOSCOPE_ENABLE_COVERAGE
  "Enable test coverage instrumentation"
  OFF)

include(cmake/Coverage.cmake)
osciloscope_configure_coverage()

find_package(
  Qt6
  6.2
  REQUIRED
  COMPONENTS Widgets)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(
    PORTAUDIO
    QUIET
    IMPORTED_TARGET
    portaudio-2.0)
endif()

if(TARGET PkgConfig::PORTAUDIO)
  set(OSCILLOSCOPE_PORTAUDIO_TARGET PkgConfig::PORTAUDIO)
else()
  find_path(
    PORTAUDIO_INCLUDE_DIR
    NAMES portaudio.h
    DOC "Path to PortAudio headers")
  find_library(
    PORTAUDIO_LIBRARY
    NAMES portaudio portaudio_static
    DOC "Path to PortAudio library")

  if(NOT PORTAUDIO_INCLUDE_DIR OR NOT PORTAUDIO_LIBRARY)
    message(
      FATAL_ERROR
        "PortAudio not found. Install PortAudio development files or provide PORTAUDIO_INCLUDE_DIR and PORTAUDIO_LIBRARY."
    )
  endif()

  add_library(
    portaudio_external
    UNKNOWN
    IMPORTED)
  set_target_properties(portaudio_external PROPERTIES IMPORTED_LOCATION "${PORTAUDIO_LIBRARY}"
                                                      INTERFACE_INCLUDE_DIRECTORIES "${PORTAUDIO_INCLUDE_DIR}")
  set(OSCILLOSCOPE_PORTAUDIO_TARGET portaudio_external)
endif()

add_executable(
  portable-osciloscope
  src/main.cpp
  src/MainWindow.cpp
  src/audio/AudioManager.cpp
  src/audio/RingBuffer.cpp
  src/dsp/SignalMonitor.cpp
  src/widgets/WaveformWidget.cpp
  assets/icons/app.qrc)

if(WIN32)
  target_sources(portable-osciloscope PRIVATE assets/icons/signal.rc)
endif()

target_include_directories(portable-osciloscope PRIVATE src)

target_link_libraries(portable-osciloscope PRIVATE Qt6::Widgets ${OSCILLOSCOPE_PORTAUDIO_TARGET})

osciloscope_enable_coverage(portable-osciloscope)

if(OSCILLOSCOPE_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)

  osciloscope_add_coverage_target()
endif()
