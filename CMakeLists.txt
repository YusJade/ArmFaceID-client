cmake_minimum_required(VERSION 3.10)

project(armfaceid-client)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)

include_directories(include third_party third_party/treasure_chest)

include(path_config.cmake)

set(CMAKE_MODULE_PATH ${SeetaFace6_DIR}/cmake)
message(STATUS "CMAKE_MODULE_PATH: ${SeetaFace6_DIR}/cmake")

find_package(SeetaFaceDetector REQUIRED)
find_package(SeetaFaceLandmarker REQUIRED)
find_package(SeetaFaceAntiSpoofingX REQUIRED)
find_package(SeetaQualityAssessor REQUIRED)

find_package(OpenCV CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets NAMES Qt6 Qt5 CONFIG REQUIRED)

message(STATUS "Qt6_FOUND: ${Qt6_FOUND}")
message(STATUS "Qt5_FOUND: ${Qt5_FOUND}")

if(${Qt6_FOUND})
  set(Qt "Qt6")
elseif(${Qt5_FOUND})
  set(Qt "Qt5")
endif()

add_executable(armfaceid-client src/main.cpp src/camera.cpp 
                                src/qt_gui/qt_gui.cpp src/utils/base.cpp)

target_link_libraries(
  armfaceid-client PUBLIC absl::flags absl::flags_parse absl::flags_usage
                          spdlog::spdlog Qt6::Core Qt6::Widgets ${OpenCV_LIBS})

target_include_directories(
  armfaceid-client PUBLIC ${OpenCV_INCLUDE_DIRS} ${TREASURE_CHEST_INCLUDE_DIRS}
                          include)

add_subdirectory(third_party)
