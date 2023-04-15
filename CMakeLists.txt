cmake_minimum_required(VERSION 3.0.0)
project(Alphabot VERSION 0.1.0)

# переменная с путем к библиотеки чтобы каждый задал как ему удобно
# set(MOSQUITTO_LIB_PATH "C:/Program Files/mosquitto/include")
# set(MOSQUITTO_TARGET_PATH "C:/Program Files/mosquitto/build/src/Debug/mosquitto.lib")

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
    src/KeyParser.cpp
    src/KeyParser.h
)

include_directories(${MOSQUITTO_LIB_PATH})

target_link_libraries(Alphabot ${OpenCV_LIBS})
# target_link_libraries(Alphabot ${MOSQUITTO_TARGET_PATH})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)