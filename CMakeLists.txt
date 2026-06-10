cmake_minimum_required(VERSION 3.20)

project(weatherflow_scheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

include(CTest)

add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(tests)

add_executable(weatherflow_scheduler
   main.cpp
   app/qt/main_window.cpp
   app/qt/main_window.hpp
)

target_include_directories(weatherflow_scheduler
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app/qt
)

target_link_libraries(weatherflow_scheduler
   PRIVATE Qt6::Widgets
   PRIVATE places_recommender
   PRIVATE task_scheduler
)
