#include "c_log.h"
#include "../c_rifk.h"
#include "../sdk/c_cvar.h"
#include "welcome_message.h"
#include <sstream>

c_log::c_log()
{
	cvar()->find_var(_("developer"))->set_value(1);
	cvar()->find_var(_("con_enable"))->set_value(2);
	cvar()->find_var(_("con_filter_enable"))->set_value(1);
	cvar()->find_var(_("con_filter_text"))->set_value(_("[rifk7]"));
	cvar()->find_var(_("con_filter_text_out"))->set_value(_(" "));
	cvar()->find_var(_("contimes"))->set_value(10);
	cvar()->find_command(_("clear"))->dispatch();

	_rt(msg, welcome_message);
	cvar()->console_color_printf(false, { 0, 255, 0, 255 }, msg);
}

void c_log::debug(const std::string& message)
{
#ifndef RELEASE
	print(logtype::debug, message);
#endif
}

void c_log::info(const std::string& message)
{
	print(logtype::info, message);
}

void c_log::warning(const std::string& message)
{
	print(logtype::warning, message);
}

void c_log::error(const std::string& message)
{
	print(logtype::error, message);
}

void c_log::print(const logtype type, const std::string& message)
{
	std::stringstream stream;
	c_color color = { 255, 255, 255, 255 };
	stream << _("[rifk7] - [");

	switch (type) {
	case logtype::debug:
		stream << _("debug");
		break;
	case logtype::info:
		stream << _("info");
		color = { 0, 255, 0 };
		break;
	case logtype::warning:
		stream << _("warning");
		color = { 255, 70, 0 };
		break;
	case logtype::error:
		stream << _("error");
		color = { 255, 0, 0 };
		break;
	}

	stream << _("] - ") << message.c_str() << std::endl;
	cvar()->console_color_printf(true, color, stream.str().c_str());
}
