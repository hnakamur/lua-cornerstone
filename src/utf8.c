#include "cornerstone-priv.h"

typedef unsigned char uchar;

#define UTF8_IS_HEAD_BYTE(bptr) (((*(const uchar *)bptr) & 0xC0) != 0x80)
#define UTF8_IS_TAIL_BYTE(bptr) (((*(const uchar *)bptr) & 0xC0) == 0x80)

static int get_code_point_byte_len(const char *lead) {
  int byte = *(const uchar *)lead;
  if (byte <= 0x7F) {
    return 1;
  } else if (byte <= 0xDF) {
    return 2;
  } else if (byte <= 0xEF) {
    return 3;
  } else if (byte <= 0xF7) {
    return 4;
  } else if (byte <= 0xFB) {
    return 5;
  } else if (byte <= 0xFD) {
    return 6;
  } else {
    return -1;
  }
}

static int get_code_point(const char *s) {
  const uchar *u = (const uchar *)s;
  if (u[0] <= 0x7F) {
    return u[0];
  } else if (u[0] <= 0xDF) {
    return (u[0] & 0x1F) << 6 | (u[1] & 0x3F);
  } else if (u[0] <= 0xEF) {
    return (u[0] & 0x0F) << 12 | (u[1] & 0x3F) << 6 | (u[2] & 0x3F);
  } else if (u[0] <= 0xF7) {
    return (u[0] & 0x07) << 18 | (u[1] & 0x3F) << 12 | (u[2] & 0x3F) << 6 |
        (u[3] & 0x3F);
  } else if (u[0] <= 0xFB) {
    return (u[0] & 0x03) << 24 | (u[1] & 0x3F) << 18 | (u[2] & 0x3F) << 12 |
        (u[3] & 0x3F) << 6 | (u[4] & 0x3F);
  } else if (u[0] <= 0xFD) {
    return (u[0] & 0x01) << 30 | (u[1] & 0x3F) << 24 | (u[2] & 0x3F) << 18 |
        (u[3] & 0x3F) << 12 | (u[4] & 0x3F) << 6 | (u[5] & 0x3F);
  } else {
    return -1;
  }
}

static int count_code_point(const char *s, size_t slen) {
  const char *send = s + slen;
  int n = 0;
  for (; s < send; s += get_code_point_byte_len(s)) {
    n++;
  }
  return n;
}

static int to_byte_position(const char *s, unsigned index) {
  const char *p = s;
  unsigned i;
  for (i = 1; i < index; ++i) {
    p += get_code_point_byte_len(p);
  }
  return p - s + 1;
}

int utf8_char(lua_State *L) {
  int arg_n = lua_gettop(L);
  int arg_i;
  luaL_Buffer buf;
  for (arg_i = 1; arg_i <= arg_n; ++arg_i) {
    unsigned cp = (unsigned)luaL_checkint(L, arg_i);
    luaL_buffinit(L, &buf);
    if (cp <= 0x7F) {
      luaL_addchar(&buf, (char)cp);
    } else if (cp <= 0x7FF) {
      luaL_addchar(&buf, (char)(0xC0 | cp >> 6));
      luaL_addchar(&buf, (char)(0x80 | cp & 0x3F));
    } else if (cp <= 0xFFFF) {
      luaL_addchar(&buf, (char)(0xE0 | cp >> 12));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 6) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | cp & 0x3F));
    } else if (cp <= 0x1FFFFF) {
      luaL_addchar(&buf, (char)(0xF0 | cp >> 18));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 12) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 6) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | cp & 0x3F));
    } else if (cp <= 0x3FFFFFF) {
      luaL_addchar(&buf, (char)(0xF8 | cp >> 24));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 18) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 12) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 6) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | cp & 0x3F));
    } else if (cp <= 0x7FFFFFFF) {
      luaL_addchar(&buf, (char)(0xFC | cp >> 30));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 24) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 18) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 12) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | (cp >> 6) & 0x3F));
      luaL_addchar(&buf, (char)(0x80 | cp & 0x3F));
    }
    luaL_pushresult(&buf);
  }
  return arg_n;
}

int utf8_code_point(lua_State *L) {
  size_t str_len;
  const char *str = luaL_checklstring(L, 1, &str_len);
  int first = luaL_optint(L, 2, 1);
  int last = luaL_optint(L, 3, first);
  int cp_cnt;
  const char *str_end = str + str_len;
  const char *p = str;
  int cp_len;
  int i;
  int ret_cnt = 0;

  if (first < 0 || last < 0) {
    cp_cnt = count_code_point(str, str_len);
  }
  if (first < 0) {
    first = cp_cnt + first + 1;
  }
  if (last < 0) {
    last = cp_cnt + last + 1;
  }
  if (first > last) {
    return 0;
  }

  for (i = 1; i < first; ++i) {
    p += get_code_point_byte_len(p);
  }

  for (i = first; i <= last; ++i) {
    int cp_len = get_code_point_byte_len(p);
    if (p < str) {
      continue;
    }
    if (p + cp_len > str_end) {
      break;
    }
    lua_pushnumber(L, get_code_point(p));
    ret_cnt++;
    p += cp_len;
  }
  return ret_cnt;
}

static int utf8_index_of(lua_State *L) {
  size_t haystack_len;
  const char *haystack = luaL_checklstring(L, 1, &haystack_len);
  size_t needle_len;
  const char *needle = luaL_checklstring(L, 2, &needle_len);
  int position = luaL_optint(L, 3, 1);
  int cp_len;
  int cp_cnt = count_code_point(haystack, haystack_len);
  int byte_pos;

  if (position < 0) {
    position = cp_cnt + position + 1;
  }

  if (needle_len == 0) {
    lua_pushnumber(L, position);
    return 1;
  }

  if (position > cp_cnt)
    goto error;
  byte_pos = to_byte_position(haystack, position);
  const char *p = (const char *)memmem(&haystack[byte_pos - 1], haystack_len,
      needle, needle_len);
  if (!p)
    goto error;
  lua_pushnumber(L, count_code_point(haystack, p - haystack) + 1);

  return 1;

error:
  lua_pushnil(L);
  return 1;
}
 
static int utf8_last_index_of(lua_State *L) {
  size_t haystack_len;
  const char *haystack = luaL_checklstring(L, 1, &haystack_len);
  size_t needle_len;
  const char *needle = luaL_checklstring(L, 2, &needle_len);
  int position = luaL_optint(L, 3, -1);
  int last_char;
  const char *p;
  size_t n;
  int byte_pos;

  int cp_cnt = count_code_point(haystack, haystack_len);
  if (position < 0) {
    position = cp_cnt + position + 1;
  }

  if (needle_len == 0) {
    lua_pushnumber(L, position);
    return 1;
  }

  if (position > cp_cnt)
    goto error;
  last_char = needle[needle_len - 1];
  byte_pos = to_byte_position(haystack, position);
  for (p = &haystack[byte_pos - 1]; p >= &haystack[needle_len - 1]; --p) {
    if (*p != last_char)
      continue;
    const char *start = p - needle_len + 1;
    if (memcmp(start, needle, needle_len) == 0) {
      lua_pushnumber(L, count_code_point(haystack, start - haystack) + 1);
      return 1;
    }
  }

error:
  lua_pushnil(L);
  return 1;
}

int utf8_len(lua_State *L) {
  size_t str_len;
  const char *str = luaL_checklstring(L, 1, &str_len);
  int cnt = *str == '\0' ? 0 : count_code_point(str, str_len);
  lua_pushnumber(L, cnt);
  return 1;
}

int utf8_reverse(lua_State *L) {
  size_t str_len;
  const char *str = luaL_checklstring(L, 1, &str_len);
  const char *p;
  const char *q;
  const char *s;
  luaL_Buffer buf;

  luaL_buffinit(L, &buf);
  for (q = str + str_len; q > str; q = p) {
    for (p = q - 1; UTF8_IS_TAIL_BYTE(p) && p >= str; --p);
    for (s = p; s < q; ++s) {
      luaL_addchar(&buf, *s);
    }
  }
  luaL_pushresult(&buf);
  return 1;
}

int utf8_sub(lua_State *L) {
  size_t str_len;
  const char *str = luaL_checklstring(L, 1, &str_len);
  int first = luaL_optint(L, 2, 1);
  int last = luaL_optint(L, 3, -1);
  int cp_cnt;
  const char *str_end = str + str_len;
  const char *p = str;
  int cp_len;
  int i;
  int j;
  luaL_Buffer buf;

  if (first < 0 || last < 0) {
    cp_cnt = count_code_point(str, str_len);
  }
  if (first < 0) {
    first = cp_cnt + first + 1;
  }
  if (last < 0) {
    last = cp_cnt + last + 1;
  }

  for (i = 1; i < first; ++i) {
    p += get_code_point_byte_len(p);
  }

  luaL_buffinit(L, &buf);
  for (i = first; i <= last; ++i) {
    int cp_len = get_code_point_byte_len(p);
    if (p < str) {
      continue;
    }
    if (p + cp_len > str_end) {
      break;
    }
    for (j = 0; j < cp_len; ++j, ++p) {
      luaL_addchar(&buf, *p);
    }
  }
  luaL_pushresult(&buf);
  return 1;
}

static const struct luaL_Reg functions[] = {
  { "char", utf8_char },
  { "codePoint", utf8_code_point },
  { "endsWith", ends_with },
  { "indexOf", utf8_index_of },
  { "lastIndexOf", utf8_last_index_of },
  { "len", utf8_len },
  { "reverse", utf8_reverse },
  { "startsWith", starts_with },
  { "sub", utf8_sub },
  { NULL, NULL }
};

int luaopen_cs_utf8(lua_State *L) {
  lua_createtable(L, 0, ARRAY_SIZE(functions) - 1);
  luaL_register(L, NULL, functions);

  luaopen_cs_utf8_regexp(L);

  lua_setfield(L, -2, "utf8");
  return 1;
}
