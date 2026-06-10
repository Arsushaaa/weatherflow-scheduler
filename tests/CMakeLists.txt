include(FetchContent)

FetchContent_Declare(
   googletest
   URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(task_scheduler_tests
   apply_tests.cpp
   compile_time_tests.cpp
   execution_tests.cpp
   future_tests.cpp
   invocation_tests.cpp
   result_semantics_tests.cpp
)

target_link_libraries(task_scheduler_tests
   PRIVATE task_scheduler
   PRIVATE GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(task_scheduler_tests)
