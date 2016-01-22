#include <stdint.h>
#include <string.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

void set_prg_bank_size(int s);
void set_chr_bank_size(int s);
void set_mapper(int s);
void cpu_write(uint32_t addr, uint8_t data);
void cpu_read(uint32_t addr, uint32_t len);
void ppu_read(uint32_t addr, uint32_t len);

lua_State *luastate = 0;

static int l_set_prg_bank_size(lua_State *L) {
	uint32_t d = (uint32_t)luaL_checkinteger(L, 1);

	set_prg_bank_size(d);
	return(0);
}

static int l_set_chr_bank_size(lua_State *L) {
	uint32_t d = (uint32_t)luaL_checkinteger(L, 1);

	set_chr_bank_size(d);
	return(0);
}

static int l_set_mapper(lua_State *L) {
	uint32_t d = (uint32_t)luaL_checkinteger(L, 1);

	set_mapper(d);
	return(0);
}

static int l_cpu_write(lua_State *L) {
	uint32_t addr = (uint32_t)luaL_checkinteger(L, 1);
	uint32_t data = (uint32_t)luaL_checkinteger(L, 2);

	cpu_write(addr, data);
	return(0);
}

static int l_cpu_read(lua_State *L) {
	uint32_t addr = (uint32_t)luaL_checkinteger(L, 1);
	uint32_t len = (uint32_t)luaL_checkinteger(L, 2);

	cpu_read(addr, len);
	return(0);
}

static int l_ppu_read(lua_State *L) {
	uint32_t addr = (uint32_t)luaL_checkinteger(L, 1);
	uint32_t len = (uint32_t)luaL_checkinteger(L, 2);

	ppu_read(addr, len);
	return(0);
}

bool script_init()
{
	luastate = luaL_newstate();
	luaL_openlibs(luastate);

	lua_pushcfunction(luastate, l_set_prg_bank_size);
	lua_setglobal(luastate, "set_prg_bank_size");

	lua_pushcfunction(luastate, l_set_chr_bank_size);
	lua_setglobal(luastate, "set_chr_bank_size");

	lua_pushcfunction(luastate, l_set_mapper);
	lua_setglobal(luastate, "set_mapper");

	lua_pushcfunction(luastate, l_cpu_write);
	lua_setglobal(luastate, "cpu_write");

	lua_pushcfunction(luastate, l_cpu_read);
	lua_setglobal(luastate, "cpu_read");

	lua_pushcfunction(luastate, l_ppu_read);
	lua_setglobal(luastate, "ppu_read");

	return(true);
}

void script_kill()
{
	if (luastate) {
		lua_close(luastate);
		luastate = 0;
	}
}

bool script_load(char *filename)
{
	if (luaL_dofile(luastate, filename)) {
		fprintf(stderr, "lua error: %s\n", lua_tostring(luastate, -1));
		lua_pop(luastate, 1);
		return(false);
	}
	return(true);
}

bool script_setup()
{
	lua_getglobal(luastate, "dump_setup");
	if (lua_pcall(luastate, 0, 0, 0) != 0) {
		printf("Error running function f:%s`", lua_tostring(luastate, -1));
		return(false);
	}
	return(true);
}
bool script_dump()
{
	lua_getglobal(luastate, "dump_cpu");
	if (lua_pcall(luastate, 0, 0, 0) != 0) {
		printf("Error running function f:%s`", lua_tostring(luastate, -1));
		return(false);
	}
	lua_getglobal(luastate, "dump_ppu");
	if (lua_pcall(luastate, 0, 0, 0) != 0) {
		printf("Error running function f:%s`", lua_tostring(luastate, -1));
		return(false);
	}
	return(true);
}
