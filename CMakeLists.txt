cmake_minimum_required(VERSION 3.10)
project(KungFuChess CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS)
endif()

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

# websocketpp (header-only) + standalone asio (header-only, no Boost) for the game server.
# Populated without add_subdirectory: websocketpp's own CMakeLists expects Boost and
# defines tests/install rules we don't need, so we only pull the source tree and wire
# the headers in manually via an INTERFACE target.
FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG 0.8.2
)
FetchContent_GetProperties(websocketpp)
if(NOT websocketpp_POPULATED)
    FetchContent_Populate(websocketpp)
endif()

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-18-2
)
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
endif()

add_library(websocketpp_asio INTERFACE)
target_include_directories(websocketpp_asio INTERFACE
    "${websocketpp_SOURCE_DIR}"
    "${asio_SOURCE_DIR}/asio/include")
target_compile_definitions(websocketpp_asio INTERFACE ASIO_STANDALONE _WEBSOCKETPP_CPP11_STL_)
if(WIN32)
    target_link_libraries(websocketpp_asio INTERFACE ws2_32 wsock32)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/server_main.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/client_main.cpp")

add_library(ChessLogic STATIC ${SOURCES})
target_link_libraries(ChessLogic PUBLIC ${OPENCV_LIB_NAME} nlohmann_json::nlohmann_json websocketpp_asio user32)

add_executable(ChessGame src/main.cpp)
target_link_libraries(ChessGame PRIVATE ChessLogic)
add_custom_command(TARGET ChessGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:ChessGame>")

add_executable(ChessServer src/server_main.cpp)
target_link_libraries(ChessServer PRIVATE ChessLogic)

add_executable(ChessClient src/client_main.cpp)
target_link_libraries(ChessClient PRIVATE ChessLogic)
add_custom_command(TARGET ChessClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:ChessClient>")

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

add_executable(smoke_network_client tests/manual/smoke_network_client.cpp)
target_link_libraries(smoke_network_client PRIVATE ChessLogic)
add_custom_command(TARGET smoke_network_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:smoke_network_client>")

add_executable(smoke_two_players tests/manual/smoke_two_players.cpp)
target_link_libraries(smoke_two_players PRIVATE ChessLogic)
add_custom_command(TARGET smoke_two_players POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:smoke_two_players>")

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
    test_event_bus
    test_message_codec
    test_connections_registry
    test_message_router
)

foreach(test_name ${UNIT_TESTS})
    add_executable(${test_name} tests/unit/${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE ChessLogic)
    add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()