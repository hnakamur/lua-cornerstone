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
 
static int index_of(lua_State *L) {
  size_t haystack_len;
  const char *haystack = luaL_checklstring(L, 1, &haystack_len);
  size_t needle_len;
  const char *needle = luaL_checklstring(L, 2, &needle_len);
  int position = luaL_optint(L, 3, 1);
  if (needle_len == 0) {
    lua_pushnumber(L, position);
    return 1;
  }

  if (position > haystack_len)
    goto error;
  const char *p = (const char *)memmem(&haystack[position - 1], haystack_len,
      needle, needle_len);
  if (!p)
    goto error;
  lua_pushnumber(L, p - haystack + 1);
  return 1;

error:
  lua_pushnil(L);
  return 1;
}
 
static int last_index_of(lua_State *L) {
  size_t haystack_len;
  const char *haystack = luaL_checklstring(L, 1, &haystack_len);
  size_t needle_len;
  const char *needle = luaL_checklstring(L, 2, &needle_len);
  int position = luaL_optint(L, 3, haystack_len);
  int last_char;
  const char *p;
  size_t n;

  if (needle_len == 0) {
    lua_pushnumber(L, position);
    return 1;
  }

  if (position > haystack_len)
    goto error;
  last_char = needle[needle_len - 1];
  for (p = &haystack[position - 1]; p >= &haystack[needle_len - 1]; --p) {
    if (*p != last_char)
      continue;
    const char *start = p - needle_len + 1;
    if (memcmp(start, needle, needle_len) == 0) {
      lua_pushnumber(L, start - haystack + 1);
      return 1;
    }
  }

error:
  lua_pushnil(L);
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
  { "indexOf", index_of },
  { "lastIndexOf", last_index_of },
  { "startsWith", starts_with },
  { NULL, NULL }
};

int luaopen_string(lua_State *L) {
  lua_newtable(L);
  luaL_register(L, NULL, functions);
  lua_setfield(L, -2, "string");
  return 1;
}
