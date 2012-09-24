#ifndef _CORNERSTONE_PRIV_H
#define _CORNERSTONE_PRIV_H

#define _GNU_SOURCE /* for memmem() on Linux */
#include <string.h>
#include <lauxlib.h>
#include "cornerstone.h"
 
int luaopen_cs_string(lua_State *L);
int luaopen_cs_utf8(lua_State *L);
int luaopen_cs_utf8_regexp(lua_State *L);

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define ERRNAME_FUNC_BODY(errcode, map, gen, def) \
  switch (errcode) { \
  map(gen) \
  default: return def; \
  }

#endif /* _CORNERSTONE_PRIV_H */
