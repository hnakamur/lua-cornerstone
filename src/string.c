#include "cornerstone.h"
 
static int ends_with(lua_State *L) {
  size_t str_len;
  const char *str = luaL_checklstring(L, 1, &str_len);
  size_t suffix_len;
  const char *suffix = luaL_checklstring(L, 2, &suffix_len);
  int ret = str_len >= suffix_len &&
    memcmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
  lua_pushboolean(L, ret);
  return 1;
}
 
static int starts_with(lua_State *L) {
  size_t str_len;
  const char *str = luaL_checklstring(L, 1, &str_len);
  size_t prefix_len;
  const char *prefix = luaL_checklstring(L, 2, &prefix_len);
  int ret = str_len >= prefix_len &&
    memcmp(str, prefix, prefix_len) == 0;
  lua_pushboolean(L, ret);
  return 1;
}

static const struct luaL_Reg functions[] = {
  { "endsWith", ends_with },
  { "startsWith", starts_with },
  { NULL, NULL }
};

int luaopen_string(lua_State *L) {
  lua_newtable(L);
  luaL_register(L, NULL, functions);
  lua_setfield(L, -2, "string");
  return 1;
}
