project(stage CXX)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
#find_package(Qt5Widgets)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

include_directories( ${PROJECT_SOURCE_DIR}/src )

# the list of source files
set(Stage_SRCS
    src/Main.cpp
src/MyWindow.cpp
src/DICOMManager.cpp
src/MyLabel.cpp
src/Controller.cpp
src/MyHistogram.cpp
src/MyHistogramLabel.cpp
)
set(MYPROJECT_CURRENT_HEADERS
src/MyWindow.hpp
src/DICOMManager.hpp
src/MyLabel.hpp
src/Controller.hpp
src/MyHistogram.hpp
src/MyHistogramLabel.h

)
qt4_wrap_cpp(mocFiles ${MYPROJECT_CURRENT_HEADERS})

add_executable(stage ${Stage_SRCS} ${mocFiles})

target_link_libraries(stage ${QT_LIBRARIES} ${ITK_LIBRARIES})#Qt5::Widgets
