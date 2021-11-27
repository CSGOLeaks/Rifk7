#pragma once

#include "c_singleton.h"
#include <string>

class c_log : public c_singleton<c_log>
{
	enum class logtype { debug, info, warning, error };

public:
	c_log();

	static void debug(const std::string& message);
	static void info(const std::string& message);
	static void warning(const std::string& message);
	static void error(const std::string& message);

private:
	static void print(logtype type, const std::string& message);
};

#define logging c_log::instance()
