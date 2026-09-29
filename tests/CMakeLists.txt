add_executable(signal_monitor_smoke_test signal_monitor_smoke_test.cpp ../src/dsp/SignalMonitor.cpp)

target_include_directories(signal_monitor_smoke_test PRIVATE ../src)

osciloscope_enable_coverage(signal_monitor_smoke_test)

add_test(NAME signal_monitor_smoke_test COMMAND signal_monitor_smoke_test)

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
