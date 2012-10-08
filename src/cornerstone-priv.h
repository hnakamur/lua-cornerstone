#ifndef _CORNERSTONE_PRIV_H
#define _CORNERSTONE_PRIV_H

#define _GNU_SOURCE /* for memmem() on Linux */
#include <string.h>
#include <lauxlib.h>
#include "cornerstone.h"

typedef unsigned char uchar;
 
int luaopen_cs_bitvector(lua_State *L);
int luaopen_cs_string(lua_State *L);
int luaopen_cs_utf8(lua_State *L);
int luaopen_cs_utf8_regexp(lua_State *L);

int cs_utf8_count_code_point(const char *s, size_t slen);
void cs_utf8_sub(const char *str, size_t str_len, int first, int last,
    const char **sub, size_t *sub_len);
void cs_utf8_push_sub(lua_State *L, const char *str, size_t str_len, int first,
    int last);

int ends_with(lua_State *L);
int starts_with(lua_State *L);

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define ERRNAME_FUNC_BODY(errcode, map, gen, def) \
  switch (errcode) { \
  map(gen) \
  default: return def; \
  }

#endif /* _CORNERSTONE_PRIV_H */
