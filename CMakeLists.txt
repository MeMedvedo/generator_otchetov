CMAKE_MINIMUM_REQUIRED (VERSION 3.6)

set (CMAKE_CXX_STANDARD 17)
project(generator_otchetov LANGUAGES CXX)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_NAME} PUBLIC include/ )
set (SRCS main.cpp src/fish_go.cpp src/timer.cpp src/test.cpp)
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE -lpqxx)
