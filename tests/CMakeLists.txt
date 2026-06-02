add_executable(signal_monitor_smoke_test
  signal_monitor_smoke_test.cpp
  ../src/dsp/SignalMonitor.cpp
)

target_include_directories(signal_monitor_smoke_test PRIVATE
  ../src
)

add_test(NAME signal_monitor_smoke_test COMMAND signal_monitor_smoke_test)
