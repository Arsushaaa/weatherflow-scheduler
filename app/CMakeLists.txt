include(FetchContent)

FetchContent_Declare(
   cpr
   GIT_REPOSITORY https://github.com/libcpr/cpr.git
   GIT_TAG 1.12.0
)

FetchContent_Declare(
   nlohmann_json
   GIT_REPOSITORY https://github.com/nlohmann/json.git
   GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(nlohmann_json cpr)

add_library(places_recommender
   src/main_logic.cpp
   src/distance_calc.cpp
   src/parser.cpp
   src/response_to_api.cpp
)

target_link_libraries(places_recommender
   PUBLIC cpr::cpr
   PRIVATE nlohmann_json::nlohmann_json
   PUBLIC task_scheduler
)

target_include_directories(places_recommender PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)