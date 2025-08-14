FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
  CONFIGURE_COMMAND "" BUILD_COMMAND "")

FetchContent_MakeAvailable(nlohmann_json)

set_target_properties(
  nlohmann_json
  PROPERTIES JSON_BuildTests OFF
             JSON_MultipleHeaders OFF # Use single header mode
             JSON_Install OFF # Don't install
             JSON_BuildExamples OFF # Don't build examples
)
