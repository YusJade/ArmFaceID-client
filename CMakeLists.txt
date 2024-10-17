cmake_minimum_required(VERSION 3.10)

project(armfaceid-client)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include third_party third_party/treasure_chest)

find_package(OpenCV CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(armfaceid-client src/main.cpp)

target_link_libraries(armfaceid-client PUBLIC absl::flags absl::flags_parse
                                              absl::flags_usage spdlog::spdlog ${OpenCV_LIBS})

target_include_directories(armfaceid-client PUBLIC ${OpenCV_INCLUDE_DIRS} 
                                                   ${TREASURE_CHEST_INCLUDE_DIRS}
                                                   include)

add_subdirectory(third_party)
