#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class ConfigManager {
public:

	enum string_config_t {
		DUMMY_STR = 0,
		SERVERNAME,
		LAST_STRING_CONFIG /* this must be the last one */
	};
	
	enum integer_config_t {
		PORT,
		VERSION,
		LOGIN_DELAY,
		ATTACK_DELAY,
		EXHAUSTED,
		TARGET_SKULL,
		LAST_INTEGER_CONFIG /* this must be the last one */
	};
	
	bool loadFile(const std::string& _filename);
    const std::string& getString(uint32_t _what) const;
	int getNumber(uint32_t _what) const;
    	
private:
    std::string getGlobalString(lua_State* _L, const std::string& _identifier, const std::string& _default="");
	int getGlobalNumber(lua_State* _L, const std::string& _identifier, const int _default=0);
    
    std::string m_confString[LAST_STRING_CONFIG];
	int m_confInteger[LAST_INTEGER_CONFIG];
	
};

#endif
