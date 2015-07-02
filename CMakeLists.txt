project(stage CXX C)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")


include_directories( ${PROJECT_SOURCE_DIR}/src )
include_directories( ${PROJECT_SOURCE_DIR}/src/DICOMManager )
include_directories( ${PROJECT_SOURCE_DIR}/src/ui )
include_directories( ${PROJECT_SOURCE_DIR}/src/Controller )


find_package(Qt4 REQUIRED QtCore QtOpenGL QtGui QtXml)
include(${QT_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(DGtal REQUIRED)
INCLUDE_DIRECTORIES(${DGTAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DGTAL_LIBRARY_DIRS})
find_package(OpenGL REQUIRED)

#add readme in buildDir
configure_file(README.md README.md)

ADD_DEFINITIONS(-std=c++11 -fpermissive)


include_directories( ${PROJECT_SOURCE_DIR}/src )

# the list of source files
set(Stage_SRCS
    src/Main.cpp
src/Controller/Controller.cpp

src/DICOMManager/DICOMManager.cpp

src/ui/MyLabel.cpp
src/ui/MyWindow.cpp
src/ui/MyHistogram.cpp
src/ui/MyHistogramLabel.cpp
src/ui/MyScrollBar.cpp
src/ui/My3Dviewer.cpp
src/Controller/Controller.hpp
src/DICOMManager/DICOMManager.hpp
src/ui/MyLabel.hpp
src/ui/MyScrollBar.h
src/ui/MyHistogramLabel.hpp
)
set(MYPROJECT_CURRENT_HEADERS
src/ui/MyWindow.hpp
src/ui/MyHistogram.hpp
src/ui/My3Dviewer.h
)
qt4_wrap_cpp(mocFiles ${MYPROJECT_CURRENT_HEADERS})

add_executable(stage ${Stage_SRCS} ${mocFiles})

target_link_libraries(stage  ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${ITK_LIBRARIES} ${DGTAL_LIBRARIES})#Qt5::Widgets
