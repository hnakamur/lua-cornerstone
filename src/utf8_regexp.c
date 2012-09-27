#include <pcre.h>
#include "cornerstone-priv.h"

#define RE_MTBL_NAME "cornerstone.utf8.RegExp"
#define MR_MTBL_NAME "cornerstone.utf8.MatchResult"

typedef struct cs_regexp_s {
  pcre *re;
  pcre_extra *extra;
  size_t capture_cnt;
} cs_regexp_t;

typedef struct cs_matchres_s {
  int capture_cnt;
  size_t ovector_len;
  int *ovector;
  cs_regexp_t *regexp;
  int regexp_ref;
  const char *subject;
  int subject_ref;
} cs_matchres_t;

#define RE_NEW_OPTION_MAP(XX) \
  XX(ANCHORED) \
  XX(AUTO_CALLOUT) \
  XX(BSR_ANYCRLF) \
  XX(BSR_UNICODE) \
  XX(CASELESS) \
  XX(DOLLAR_ENDONLY) \
  XX(DOTALL) \
  XX(DUPNAMES) \
  XX(EXTENDED) \
  XX(EXTRA) \
  XX(FIRSTLINE) \
  XX(JAVASCRIPT_COMPAT) \
  XX(MULTILINE) \
  XX(NEWLINE_ANY) \
  XX(NEWLINE_ANYCRLF) \
  XX(NEWLINE_CR) \
  XX(NEWLINE_CRLF) \
  XX(NEWLINE_LF) \
  XX(NO_AUTO_CAPTURE) \
  XX(NO_UTF16_CHECK) \
  XX(NO_UTF8_CHECK) \
  XX(UCP) \
  XX(UNGREEDY)

#define RE_NEW_OPTION_GEN(name) \
  lua_pushnumber(L, PCRE_##name); \
  lua_setfield(L, -2, #name);

static int regexp_compile(lua_State *L) {
  const char *pattern = luaL_checkstring(L, 1);
  int options = luaL_optint(L, 2, 0);
  int err_code;
  const char *err_text;
  int err_offset;

  cs_regexp_t *regexp = lua_newuserdata(L, sizeof(cs_regexp_t));
  memset(regexp, 0, sizeof(cs_regexp_t));
  luaL_getmetatable(L, RE_MTBL_NAME);
  lua_setmetatable(L, -2);

  regexp->re = pcre_compile2(pattern, options, &err_code, &err_text,
      &err_offset, NULL);
  if (!regexp->re)
    return luaL_error(L, "%s (pattern offset: %d)", err_text, err_offset + 1);

  regexp->extra = pcre_study(regexp->re, 0, &err_text);
  if (err_text)
    return luaL_error(L, "%s", err_text);

  int utf8_supported = 0;
  pcre_config(PCRE_CONFIG_UTF8, &utf8_supported);
  if (utf8_supported == 0)
    return luaL_error(L, "PCRE_CONFIG_UTF8_DISABLED");

  pcre_fullinfo(regexp->re, regexp->extra, PCRE_INFO_CAPTURECOUNT,
      &regexp->capture_cnt);

  return 1;
}

static int regexp_gc(lua_State *L) {
  cs_regexp_t *regexp = (cs_regexp_t *)luaL_checkudata(L, 1, RE_MTBL_NAME);
  if (regexp->extra)
    pcre_free_study(regexp->extra);
  if (regexp->re)
    pcre_free(regexp->re);
  return 0;
}

#define RE_MATCH_ERRCODE_MAP(XX) \
  XX(NOMATCH)        \
  XX(NULL)           \
  XX(BADOPTION)      \
  XX(BADMAGIC)       \
  XX(UNKNOWN_OPCODE) \
  XX(NOMEMORY)       \
  XX(NOSUBSTRING)    \
  XX(MATCHLIMIT)     \
  XX(CALLOUT)        \
  XX(BADUTF8)        \
  XX(BADUTF8_OFFSET) \
  XX(PARTIAL)        \
  XX(BADPARTIAL)     \
  XX(INTERNAL)       \
  XX(BADCOUNT)       \
  XX(RECURSIONLIMIT) \
  XX(BADNEWLINE)     \
  XX(BADOFFSET)      \
  XX(SHORTUTF8)      \
  XX(RECURSELOOP)    \
  XX(JIT_STACKLIMIT) \
  XX(BADMODE)        \
  XX(BADENDIANNESS)

#define RE_MATCH_ERRGEN(name) \
  case PCRE_ERROR_##name: \
    return #name;

static const char *regexp_exec_errname(int errcode) {
  ERRNAME_FUNC_BODY(errcode, RE_MATCH_ERRCODE_MAP, RE_MATCH_ERRGEN,
      "UNKOWN_ERR_CODE")
}

static int regexp_match(lua_State *L) {
  cs_regexp_t *regexp = (cs_regexp_t *)luaL_checkudata(L, 1, RE_MTBL_NAME);
  size_t subject_len;
  const char *subject = luaL_checklstring(L, 2, &subject_len);
  int offset = luaL_optint(L, 3, 1);
  int options = 0;

  size_t ovector_len = (regexp->capture_cnt + 1) * 3;
  int *ovector = (int *)calloc(ovector_len, sizeof(int));
  if (!ovector)
    return luaL_error(L, "ENOMEM");

  int rc = pcre_exec(regexp->re, regexp->extra, subject, subject_len,
      offset - 1, options, ovector, ovector_len);
  if (rc < 0) {
    free(ovector);
    return luaL_error(L, regexp_exec_errname(rc));
  }

  cs_matchres_t *mr = lua_newuserdata(L, sizeof(cs_matchres_t));
  memset(mr, 0, sizeof(cs_matchres_t));
  luaL_getmetatable(L, MR_MTBL_NAME);
  lua_setmetatable(L, -2);

  mr->capture_cnt = rc - 1;
  mr->ovector_len = ovector_len;
  mr->ovector = ovector;

  mr->regexp = regexp;
  lua_pushvalue(L, 1);
  mr->regexp_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  mr->subject = subject;
  lua_pushvalue(L, 2);
  mr->subject_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  return 1;
}

static int matchres_gc(lua_State *L) {
  cs_matchres_t *mr = (cs_matchres_t *)luaL_checkudata(L, 1, MR_MTBL_NAME);
  free(mr->ovector);
  luaL_unref(L, LUA_REGISTRYINDEX, mr->regexp_ref);
  luaL_unref(L, LUA_REGISTRYINDEX, mr->subject_ref);
  return 0;
}

static int matchres_capture_count(lua_State *L) {
  cs_matchres_t *mr = (cs_matchres_t *)luaL_checkudata(L, 1, MR_MTBL_NAME);
  lua_pushnumber(L, mr->capture_cnt);
  return 1;
}

static int matchres_first(lua_State *L) {
  cs_matchres_t *mr = (cs_matchres_t *)luaL_checkudata(L, 1, MR_MTBL_NAME);
  int group = luaL_optint(L, 2, 0);
  lua_pushnumber(L, mr->ovector[group * 2] + 1);
  return 1;
}

static int matchres_last(lua_State *L) {
  cs_matchres_t *mr = (cs_matchres_t *)luaL_checkudata(L, 1, MR_MTBL_NAME);
  int group = luaL_optint(L, 2, 0);
  lua_pushnumber(L, mr->ovector[group * 2 + 1]);
  return 1;
}

static const struct luaL_Reg mr_methods[] = {
  { "__gc", matchres_gc },
  { "captureCount", matchres_capture_count },
  { "first", matchres_first },
  { "last", matchres_last },
  { NULL, NULL }
};

static const struct luaL_Reg mr_functions[] = {
  { NULL, NULL }
};

static const struct luaL_Reg re_methods[] = {
  { "__gc", regexp_gc },
  { "match", regexp_match },
  { NULL, NULL }
};

static const struct luaL_Reg re_functions[] = {
  { "compile", regexp_compile },
  { NULL, NULL }
};

int luaopen_cs_utf8_regexp(lua_State *L) {
  luaL_newmetatable(L, MR_MTBL_NAME);
  luaL_register(L, NULL, mr_methods);
  lua_setfield(L, -1, "__index");

  lua_createtable(L, 0, ARRAY_SIZE(mr_functions) - 1);
  luaL_register(L, NULL, mr_functions);
  lua_setfield(L, -2, "MatchResult");

  luaL_newmetatable(L, RE_MTBL_NAME);
  luaL_register(L, NULL, re_methods);
  lua_setfield(L, -1, "__index");

  lua_createtable(L, 0, ARRAY_SIZE(re_functions) - 1);
  luaL_register(L, NULL, re_functions);

  RE_NEW_OPTION_MAP(RE_NEW_OPTION_GEN);

  lua_setfield(L, -2, "regexp");

  return 1;
}
