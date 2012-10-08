#include <lauxlib.h>
#include <limits.h>

#define BITS_PER_WORD (CHAR_BIT * sizeof(unsigned int))
#define I_WORD(i) ((unsigned int)(i) / BITS_PER_WORD)
#define I_BIT(i) (1 << ((unsigned int)(i) % BITS_PER_WORD))

typedef struct cs_bitvector_s {
  int length;
  unsigned int values[1];
} cs_bitvector_t;

static int bitvector_new(lua_State *L) {
  int i, n;
  size_t nbytes;
  cs_bitvector_t *v;

  n = luaL_checkint(L, 1);
  luaL_argcheck(L, n >= 1, 1, "invalid length");
  nbytes = sizeof(cs_bitvector_t) + I_WORD(n - 1) * sizeof(unsigned int);
  v = (cs_bitvector_t *)lua_newuserdata(L, nbytes);

  v->length = n;
  for (i = 0; i < I_WORD(n - 1); i++)
    v->values[i] = 0;

  return 1;
}

static int bitvector_set(lua_State *L) {
  cs_bitvector_t *v = (cs_bitvector_t *)lua_touserdata(L, 1);
  int index = luaL_checkint(L, 2) - 1;
  luaL_checkany(L, 3);

  luaL_argcheck(L, v != NULL, 1, "'bitvector' expected");
  luaL_argcheck(L, 0 <= index && index < v->length, 2, "index out of range");

  if (lua_toboolean(L, 3))
    v->values[I_WORD(index)] |= I_BIT(index);
  else
    v->values[I_WORD(index)] &= ~I_BIT(index);
  return 0;
}

static int bitvector_get(lua_State *L) {
  cs_bitvector_t *v = (cs_bitvector_t *)lua_touserdata(L, 1);
  int index = luaL_checkint(L, 2) - 1;
  luaL_argcheck(L, v != NULL, 1, "'bitvector' expected");

  luaL_argcheck(L, 0 <= index && index < v->length, 2, "index out of range");
  lua_pushboolean(L, v->values[I_WORD(index)] & I_BIT(index));
  return 1;
}

static int bitvector_length(lua_State *L) {
  cs_bitvector_t *v = (cs_bitvector_t *)lua_touserdata(L, 1);
  luaL_argcheck(L, v != NULL, 1, "'bitvector' expected");
  lua_pushinteger(L, v->length);
  return 1;
}

static const struct luaL_Reg bitvector_functions[] = {
  { "get", bitvector_get },
  { "length", bitvector_length },
  { "new", bitvector_new },
  { "set", bitvector_set },
  { NULL, NULL }
};

int luaopen_cs_bitvector(lua_State *L) {
  luaL_register(L, "bitvector", bitvector_functions);
  lua_pop(L, 1);
  return 1;
}
