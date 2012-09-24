#include "cornerstone-priv.h"

static const struct luaL_Reg functions[] = {
  { NULL, NULL }
};

int luaopen_cornerstone(lua_State *L) {
  luaL_register(L, "cornerstone", functions);
  luaopen_string(L);
  luaopen_utf8(L);
  return 1;
}
