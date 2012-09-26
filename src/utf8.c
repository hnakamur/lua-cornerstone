#include "cornerstone-priv.h"

static int get_code_point_byte_len(const char *lead) {
  int byte = *(const unsigned char *)lead;
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
  const unsigned char *u = (const unsigned char *)s;
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

int utf8_len(lua_State *L) {
  size_t str_len;
  const char *str = luaL_checklstring(L, 1, &str_len);
  int cnt = *str == '\0' ? 0 : count_code_point(str, str_len);
  lua_pushnumber(L, cnt);
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
  { "len", utf8_len },
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
