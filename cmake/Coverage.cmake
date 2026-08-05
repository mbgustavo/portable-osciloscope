# Validate coverage prerequisites when coverage instrumentation is enabled.
function(osciloscope_configure_coverage)
  if(OSCILLOSCOPE_ENABLE_COVERAGE)
    if(NOT
       CMAKE_CXX_COMPILER_ID
       MATCHES
       "GNU|Clang|AppleClang")
      message(FATAL_ERROR "Coverage is currently supported only with GCC or Clang-compatible compilers.")
    endif()

    find_program(GCOVR_EXECUTABLE gcovr)
    if(NOT GCOVR_EXECUTABLE)
      message(FATAL_ERROR "gcovr is required when OSCILLOSCOPE_ENABLE_COVERAGE=ON.")
    endif()
  endif()
endfunction()

# Apply compiler and linker coverage flags to a target when coverage is enabled.
function(osciloscope_enable_coverage target_name)
  if(OSCILLOSCOPE_ENABLE_COVERAGE)
    target_compile_options(
      ${target_name}
      PRIVATE --coverage
              -O0
              -g)
    target_link_options(
      ${target_name}
      PRIVATE
      --coverage)
  endif()
endfunction()

# Add the coverage target that runs tests and writes text, HTML, and XML reports.
function(osciloscope_add_coverage_target)
  if(OSCILLOSCOPE_ENABLE_COVERAGE)
    add_custom_target(
      coverage
      COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
      COMMAND
        ${GCOVR_EXECUTABLE} --root ${CMAKE_SOURCE_DIR} --filter ${CMAKE_SOURCE_DIR}/src --filter
        ${CMAKE_SOURCE_DIR}/tests --exclude ${CMAKE_BINARY_DIR} --print-summary --txt
        ${CMAKE_BINARY_DIR}/coverage/coverage.txt --html-details ${CMAKE_BINARY_DIR}/coverage/index.html --xml-pretty
        --xml ${CMAKE_BINARY_DIR}/coverage/coverage.xml
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Running tests and generating coverage report"
      VERBATIM
      DEPENDS portable-osciloscope signal_monitor_smoke_test)
  endif()
endfunction()
