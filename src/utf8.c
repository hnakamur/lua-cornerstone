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

int utf8_code_point(lua_State *L) {
  size_t str_len;
  const char *str = luaL_checklstring(L, 1, &str_len);
  int first = luaL_optint(L, 2, 1);
  int last = luaL_optint(L, 3, first);
  int cp_cnt = count_code_point(str, str_len);
  const char *str_end = str + str_len;
  const char *p = str;
  int cp_len;
  int i;
  int ret_cnt = 0;

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

static const struct luaL_Reg functions[] = {
  { "codePoint", utf8_code_point },
  { NULL, NULL }
};

int luaopen_cs_utf8(lua_State *L) {
  lua_createtable(L, 0, ARRAY_SIZE(functions) - 1);
  luaL_register(L, NULL, functions);

  luaopen_cs_utf8_regexp(L);

  lua_setfield(L, -2, "utf8");
  return 1;
}
