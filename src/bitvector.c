#include <lauxlib.h>
#include <limits.h>

#define BITVECTOR_MTBL_NAME "cs.BitVector"
#define cs_checkbitvector(L, index) \
  ((cs_bitvector_t *)luaL_checkudata(L, index, BITVECTOR_MTBL_NAME))

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
  luaL_getmetatable(L, BITVECTOR_MTBL_NAME);
  lua_setmetatable(L, -2);

  v->length = n;
  for (i = 0; i < I_WORD(n - 1); i++)
    v->values[i] = 0;

  return 1;
}

static unsigned int *get_entry_and_mask(lua_State *L, unsigned int *mask) {
  cs_bitvector_t *v = cs_checkbitvector(L, 1);
  int index = luaL_checkint(L, 2) - 1;
  luaL_argcheck(L, 0 <= index && index < v->length, 2, "index out of range");
  *mask = I_BIT(index);
  return &v->values[I_WORD(index)];
}

static int bitvector_set(lua_State *L) {
  unsigned int mask;
  unsigned int *entry = get_entry_and_mask(L, &mask);
  luaL_checkany(L, 3);
  if (lua_toboolean(L, 3))
    *entry |= mask;
  else
    *entry &= ~mask;
  return 0;
}

static int bitvector_get(lua_State *L) {
  unsigned int mask;
  unsigned int *entry = get_entry_and_mask(L, &mask);
  lua_pushboolean(L, *entry & mask);
  return 1;
}

static int bitvector_index(lua_State *L) {
  const char *key = luaL_checkstring(L, 2);
  lua_getmetatable(L, 1);
  lua_getfield(L, -1, key);
  if (!lua_isnil(L, -1))
    return 1;
  lua_pop(L, 1);
  return bitvector_get(L);
}

static int bitvector_length(lua_State *L) {
  cs_bitvector_t *v = cs_checkbitvector(L, 1);
  luaL_argcheck(L, v != NULL, 1, "'bitvector' expected");
  lua_pushinteger(L, v->length);
  return 1;
}

static int bitvector_tostring(lua_State *L) {
  cs_bitvector_t *v = cs_checkbitvector(L, 1);
  lua_pushfstring(L, "bitvector(%d)", v->length);
  return 1;
}

static const struct luaL_Reg bitvector_methods[] = {
  { "__index", bitvector_index },
  { "__len", bitvector_length },
  { "__newindex", bitvector_set },
  { "__tostring", bitvector_tostring },
  { "get", bitvector_get },
  { "length", bitvector_length },
  { "set", bitvector_set },
  { NULL, NULL }
};

static const struct luaL_Reg bitvector_functions[] = {
  { "new", bitvector_new },
  { NULL, NULL }
};

int luaopen_cs_bitvector(lua_State *L) {
  luaL_newmetatable(L, BITVECTOR_MTBL_NAME);
  luaL_register(L, NULL, bitvector_methods);
  luaL_register(L, "bitvector", bitvector_functions);
  lua_pop(L, 2);
  return 1;
}
