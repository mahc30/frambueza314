# util/CMakeLists.txt

add_library(util log.c)
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})