#pragma once
#include "Script.hpp"
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"
Script::Script()
{
	state = luaL_newstate();
}
bool Script::init(std::string name)
{
	luaL_openlibs(state);
	return luaL_dofile(state, name.c_str());
}
Script::~Script()
{
	lua_close(state);
}
lua_State * Script::getState()
{
	return state;
}

luabridge::LuaRef Script::call(std::string functionName, luabridge::LuaRef table)
{
	luabridge::LuaRef function = luabridge::getGlobal(getState(), functionName.c_str());
	luabridge::LuaRef ret = function(table);
	return ret;
}