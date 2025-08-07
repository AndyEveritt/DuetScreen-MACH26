FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG master
)
FetchContent_MakeAvailable(nlohmann_json)
set_target_properties(nlohmann_json PROPERTIES JSON_BuildTests OFF)