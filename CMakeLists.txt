cmake_minimum_required(VERSION 3.20)

project(armfaceid-client)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

include_directories(include third_party third_party/treasure_chest)

if(EXISTS "${CMAKE_SOURCE_DIR}/path_config.cmake")
  message(STATUS "include path_config.cmake")
  include(path_config.cmake)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/utils.cmake")
  message(STATUS "include utils.cmake")
  include(utils.cmake)
endif()

set(CMAKE_MODULE_PATH 
  # $ENV{SeetaFace6_DIR}/cmake 
  ${SeetaFace6_DIR}/cmake)
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

set(SeetaFaceDetector_ROOT_DIR $ENV{SeetaFace6_DIR})
set(SeetaFaceLandmarker_ROOT_DIR $ENV{SeetaFace6_DIR})
set(SeetaQualityAssessor_ROOT_DIR $ENV{SeetaFace6_DIR})
set(SeetaFaceAntiSpoofingX_ROOT_DIR $ENV{SeetaFace6_DIR})
set(SeetaQualityAssessor_ROOT_DIR $ENV{SeetaFace6_DIR})

find_package(SeetaFaceDetector REQUIRED)
find_package(SeetaFaceLandmarker REQUIRED)
find_package(SeetaFaceAntiSpoofingX REQUIRED)
find_package(SeetaQualityAssessor REQUIRED)


find_package(OpenCV CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets CONFIG)

set(SOURCES 
  src/main.cpp 
  src/camera.cpp 
  src/utils/base.cpp
  src/face_analyzer.cpp
  src/qt_gui/client_ui.cpp 
  src/qt_gui/client.cpp 
  src/qt_gui/notification.cpp
  src/qt_gui/resizable_label.cpp
  src/qt_gui/notification.cpp)

set(QT_WRAP_HEADERS
  include/qt_gui/client_ui.h
  include/qt_gui/client.h
  include/qt_gui/notification.h
  include/qt_gui/resizable_label.h
)
 
if(${Qt6_FOUND})
  set(Qt_LIBRARIES Qt6::Core Qt6::Widgets)
  qt_wrap_cpp(SOURCES ${QT_WRAP_HEADERS})
  qt_add_resources(resources assets/resource.qrc)
else()
  find_package(Qt5 COMPONENTS Core Widgets CONFIG REQUIRED)
  if(${Qt5_FOUND})
    set(Qt_LIBRARIES Qt5::Core Qt5::Widgets)
    qt5_wrap_cpp(SOURCES ${QT_WRAP_HEADERS})
    qt5_add_resources(resources assets/resource.qrc)
  else()
    message(ERROR "找不到可用的 Qt !")
  endif()
endif()

message(STATUS "Qt6_DIR: ${Qt6_DIR}")
message(STATUS "Qt5_DIR: ${Qt5_DIR}")

message(STATUS "Qt6_FOUND: ${Qt6_FOUND}")
message(STATUS "Qt5_FOUND: ${Qt5_FOUND}")

add_executable(armfaceid-client ${SOURCES})

target_sources(armfaceid-client PRIVATE ${resources})

target_link_libraries(
  armfaceid-client PUBLIC absl::flags absl::flags_parse absl::flags_usage rpc
                          spdlog::spdlog ${Qt_LIBRARIES} ${OpenCV_LIBS}
                          ${SeetaFaceDetector_LIBRARY} ${SeetaFaceLandmarker_LIBRARY}
                          ${SeetaFaceAntiSpoofingX_LIBRARY} ${SeetaQualityAssessor_LIBRARY})

target_include_directories(
  armfaceid-client PUBLIC include 
                          ${OpenCV_INCLUDE_DIRS} ${TREASURE_CHEST_INCLUDE_DIRS}                                                 
                          ${SeetaFaceDetector_INCLUDE_DIR}/seeta ${SeetaFaceLandmarker_INCLUDE_DIR}/seeta
                          ${SeetaFaceAntiSpoofingX_INCLUDE_DIR}/seeta ${SeetaQualityAssessor_INCLUDE_DIR}/seeta
                          ${SeetaFaceDetector_INCLUDE_DIR} ${SeetaFaceLandmarker_INCLUDE_DIR}
                          ${SeetaFaceAntiSpoofingX_INCLUDE_DIR} ${SeetaQualityAssessor_INCLUDE_DIR})

add_subdirectory(third_party)
