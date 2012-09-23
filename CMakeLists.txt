project(cornerstone C)
cmake_minimum_required(VERSION 2.6)
include(dist.cmake)
include(checkit.cmake)

install_lua_module(cornerstone src/cornerstone.c)
add_checkit_test(test/*.lua)
