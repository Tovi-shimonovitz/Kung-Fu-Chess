cmake_minimum_required(VERSION 3.10)
project(KungFuChess CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# vendored OpenCV (headers + prebuilt Debug/Release .lib+.dll under third_party/opencv)
set(OPENCV_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencv")
include_directories("${OPENCV_ROOT}/include")
link_directories("${OPENCV_ROOT}/bin")
set(OPENCV_LIB_NAME "$<IF:$<CONFIG:Debug>,opencv_world451d,opencv_world451>")
set(OPENCV_DLL "${OPENCV_ROOT}/bin/$<IF:$<CONFIG:Debug>,opencv_world451d.dll,opencv_world451.dll>")

# nlohmann/json (header-only), fetched from its official repo at configure time
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(ChessLogic STATIC ${SOURCES})
target_link_libraries(ChessLogic PUBLIC ${OPENCV_LIB_NAME} nlohmann_json::nlohmann_json)

add_executable(ChessGame src/main.cpp)
target_link_libraries(ChessGame PRIVATE ChessLogic)
add_custom_command(TARGET ChessGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:ChessGame>")

# temporary manual visual smoke check - not part of the automated test suite
add_executable(smoke_board_renderer tests/manual/smoke_board_renderer.cpp)
target_link_libraries(smoke_board_renderer PRIVATE ChessLogic)
add_custom_command(TARGET smoke_board_renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:smoke_board_renderer>")

add_executable(smoke_graphics_runner tests/manual/smoke_graphics_runner.cpp)
target_link_libraries(smoke_graphics_runner PRIVATE ChessLogic)
add_custom_command(TARGET smoke_graphics_runner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:smoke_graphics_runner>")

add_executable(bbox_check tests/manual/bbox_check.cpp)
target_link_libraries(bbox_check PRIVATE ChessLogic)
add_custom_command(TARGET bbox_check POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:bbox_check>")

enable_testing()

set(UNIT_TESTS
    test_position
    test_board_mapper
    test_board
    test_piece_rules
    test_rule_engine
    test_motion
    test_motion_lock
    test_game_engine
    test_real_time_arbiter
    test_collisions
    test_board_parser
    test_controller
    test_piece_code
    test_animation_config
    test_animation_clip
    test_piece_animation_set
    test_sprite_repository
    test_piece_animator
)

foreach(test_name ${UNIT_TESTS})
    add_executable(${test_name} tests/unit/${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE ChessLogic)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()