project(cornerstone C)
cmake_minimum_required(VERSION 2.6)
include(dist.cmake)
include(checkit.cmake)

install_lua_module(cornerstone
  src/cornerstone.c src/string.c src/utf8.c src/utf8_regexp.c
)
add_checkit_test(test/*.lua)
target_link_libraries(cornerstone pcre)
