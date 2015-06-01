project(stage)

cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

include_directories( ${PROJECT_SOURCE_DIR}/src )

# the list of source files
set(Stage_SRCS
    src/Main.cpp
)

add_executable(stage ${Stage_SRCS})

target_link_libraries(stage ${QT_LIBRARIES} ${ITK_LIBRARIES})
