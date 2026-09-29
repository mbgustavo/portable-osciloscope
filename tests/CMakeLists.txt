add_executable(signal_monitor_smoke_test signal_monitor_smoke_test.cpp ../src/dsp/SignalMonitor.cpp)

target_include_directories(signal_monitor_smoke_test PRIVATE ../src)

osciloscope_enable_coverage(signal_monitor_smoke_test)

add_test(NAME signal_monitor_smoke_test COMMAND signal_monitor_smoke_test)

add_executable(audio_manager_test audio_manager_test.cpp ../src/audio/AudioManager.cpp)

target_include_directories(audio_manager_test PRIVATE ../src)

target_link_libraries(audio_manager_test PRIVATE ${OSCILLOSCOPE_PORTAUDIO_TARGET})

osciloscope_enable_coverage(audio_manager_test)

add_test(NAME audio_manager_test COMMAND audio_manager_test)

add_executable(
  main_window_test
  main_window_test.cpp
  ../src/MainWindow.cpp
  ../src/audio/AudioManager.cpp)

target_include_directories(main_window_test PRIVATE ../src)

target_link_libraries(main_window_test PRIVATE Qt6::Widgets ${OSCILLOSCOPE_PORTAUDIO_TARGET})

osciloscope_enable_coverage(main_window_test)

add_test(NAME main_window_test COMMAND main_window_test)
set_tests_properties(main_window_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(app_startup_smoke_test app_startup_smoke_test.cpp)

target_link_libraries(app_startup_smoke_test PRIVATE Qt6::Core)

osciloscope_enable_coverage(app_startup_smoke_test)

add_dependencies(app_startup_smoke_test portable-osciloscope)

add_test(NAME app_startup_smoke_test COMMAND app_startup_smoke_test $<TARGET_FILE:portable-osciloscope>)
set_tests_properties(
  app_startup_smoke_test
  PROPERTIES ENVIRONMENT
             "QT_QPA_PLATFORM=offscreen"
             TIMEOUT
             10)
