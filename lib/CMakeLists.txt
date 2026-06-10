add_library(task_scheduler INTERFACE)

target_include_directories(task_scheduler INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../manual_libraries
)