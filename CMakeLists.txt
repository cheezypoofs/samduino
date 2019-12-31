cmake_minimum_required (VERSION 3.6.2)
project (samduino-lib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

add_library (
    samduino STATIC

    "${PROJECT_SOURCE_DIR}/lib/Scheduler.cpp"
    "${PROJECT_SOURCE_DIR}/lib/SevenSegment.cpp"
)

add_executable (
    test_samduino

    "${PROJECT_SOURCE_DIR}/test/ArduinoTestState.cpp"

    "${PROJECT_SOURCE_DIR}/lib/SchedulerTest.cpp"
    "${PROJECT_SOURCE_DIR}/lib/SevenSegmentTest.cpp"

    "${PROJECT_SOURCE_DIR}/test/main.cpp"
)

target_link_libraries(
    test_samduino
        samduino
        "${CONAN_LIBS}"
)


include_directories(
    "${PROJECT_SOURCE_DIR}/lib"
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/test"
)
