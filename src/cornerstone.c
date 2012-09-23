#include "cornerstone.h"

static const struct luaL_Reg functions[] = {
  { NULL, NULL }
};

int luaopen_cornerstone(lua_State *L) {
  luaL_register(L, "cornerstone", functions);
  return 1;
}
