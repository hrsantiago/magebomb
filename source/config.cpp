#include "Config.h"

bool ConfigManager::loadFile(const std::string& _filename)
{
	lua_State* L = lua_open();

	if(!L) return false;

	if(luaL_dofile(L, _filename.c_str()))
	{
		lua_close(L);
		return false;
	}
	
	m_confString[SERVERNAME] = getGlobalString(L, "server", "127.0.0.1");
	m_confInteger[PORT] = getGlobalNumber(L, "port", 7171);
	m_confInteger[LOGIN_DELAY] = getGlobalNumber(L, "logindelay", 50);
	m_confInteger[ATTACK_DELAY] = getGlobalNumber(L, "attackdelay", 10000);
	m_confInteger[EXHAUSTED] = getGlobalNumber(L, "exhausted", 1000);
	m_confInteger[VERSION] = getGlobalNumber(L, "version", 800);
	m_confInteger[TARGET_SKULL] = getGlobalNumber(L, "targetskull", 0);
	
	lua_close(L);
	return true;
}

const std::string& ConfigManager::getString(uint32_t _what) const
{
	if(_what < LAST_STRING_CONFIG)
		return m_confString[_what];
	else
		return m_confString[DUMMY_STR];
}

int ConfigManager::getNumber(uint32_t _what) const
{
	if(_what < LAST_INTEGER_CONFIG)
		return m_confInteger[_what];
	else
		return 0;
}

std::string ConfigManager::getGlobalString(lua_State* _L, const std::string& _identifier, const std::string& _default)
{
	lua_getglobal(_L, _identifier.c_str());

	if(!lua_isstring(_L, -1))
		return _default;

	int len = (int)lua_strlen(_L, -1);
	std::string ret(lua_tostring(_L, -1), len);
	lua_pop(_L,1);

	return ret;
}

int ConfigManager::getGlobalNumber(lua_State* _L, const std::string& _identifier, const int _default)
{
	lua_getglobal(_L, _identifier.c_str());

	if(!lua_isnumber(_L, -1))
		return _default;

	int val = (int)lua_tonumber(_L, -1);
	lua_pop(_L,1);

	return val;
}
