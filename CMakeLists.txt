cmake_minimum_required(VERSION 3.10)
project(SenGL VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)

add_library(SenGL SHARED src/SenGL.cpp)
target_include_directories(SenGL PUBLIC include)

set_target_properties(SenGL PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)

add_executable(test_sengl examples/test.cpp)
target_link_libraries(test_sengl SenGL)