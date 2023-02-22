#pragma once
#include "lua.hpp"
#include "LuaBridge\LuaBridge.h"
#include <string>
struct Script
{
private:
	lua_State* state;
public:
	Script();
	bool init(std::string name);

	luabridge::LuaRef call(std::string functionName, luabridge::LuaRef table);

	~Script();
	lua_State * getState();
};