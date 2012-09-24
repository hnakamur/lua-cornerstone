#include "cornerstone-priv.h"

static const struct luaL_Reg functions[] = {
  { NULL, NULL }
};

int luaopen_utf8(lua_State *L) {
  lua_createtable(L, 0, ARRAY_SIZE(functions) - 1);
  luaL_register(L, NULL, functions);

  luaopen_utf8_regexp(L);

  lua_setfield(L, -2, "utf8");
  return 1;
}
