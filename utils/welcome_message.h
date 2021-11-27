#pragma once

#include <sstream>
#include "../security/string_obfuscation.h"

static const auto welcome_message = []() -> std::pair<std::string, char>
{
	std::stringstream str;
	str << _(
	"                                 __  _____ __   _________ \n"
	"                         _______|__|/ ____\\  | _\\______  \\\n"
	"                         \\_  __ \\  \\   __\\|  |/ /   /    /\n"
	"                          |  | \\/  ||  |  |    <   /    / \n"
	"                          |__|  |__||__|  |__|_ \\ /____/  \n"
	"                                          \\/         \n"
	);
	str << _("------------------------------------------------------------------------------------") << std::endl;
	str << _("                  Built on ") << _(__DATE__) << _(" at ") << _(__TIME__) << _(" - <rifk> <raxer> <leaked>") << std::endl;
	str << _("------------------------------------------------------------------------------------") << std::endl;

	constexpr auto key = random::_char<__COUNTER__>::value;
	auto txt = str.str();

	for (auto& c : txt)
		c ^= key;

	return std::make_pair(txt, key);
}();
