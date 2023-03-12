cmake_minimum_required(VERSION 3.0.0)
project(Alphabot VERSION 0.1.0)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Alphabot
    src/main.cpp
    src/Alphabot.cpp
    src/Alphabot.h
    src/Graffiti.cpp
    src/Graffiti.h
    src/ImageProcessing.cpp
    src/ImageProcessing.h
    src/Server.cpp
    src/Server.h
)

target_link_libraries(Alphabot ${OpenCV_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)