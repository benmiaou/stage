project(stage CXX)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")



find_package(Qt4 REQUIRED QtCore QtOpenGL QtGui QtXml)
include(${QT_USE_FILE})
#find_package(Qt5Widgets)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(DGtal REQUIRED)
INCLUDE_DIRECTORIES(${DGTAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DGTAL_LIBRARY_DIRS})
find_package(OpenGL REQUIRED)



include_directories( ${PROJECT_SOURCE_DIR}/src )

# the list of source files
set(Stage_SRCS
    src/Main.cpp
src/DICOMManager.cpp
src/MyLabel.cpp
src/Controller.cpp
src/MyWindow.cpp
src/MyHistogram.cpp
src/MyScrollBar.cpp
src/DICOMManager.hpp
src/MyLabel.hpp
src/Controller.hpp
src/MyScrollBar.h

)
set(MYPROJECT_CURRENT_HEADERS
src/MyWindow.hpp
src/MyHistogram.hpp
)
qt4_wrap_cpp(mocFiles ${MYPROJECT_CURRENT_HEADERS})

add_executable(stage ${Stage_SRCS} ${mocFiles})

target_link_libraries(stage  ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${ITK_LIBRARIES} ${DGTAL_LIBRARIES})#Qt5::Widgets
