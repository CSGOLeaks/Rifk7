#include "c_text.h"
#include "../../hooks/idirect3ddevice9.h"

c_text::c_text(const std::pair<std::string, char> text, const c_color color, const uint32_t font, const uint8_t flags, const bool centered) :
	c_drawable(c_vector2d()), text(text), color(color), font(font), flags(flags), centered(centered)
{
	_rt(txt, text);
	size = renderer->get_text_size(txt, font);
}

void c_text::draw(const c_vector2d position)
{
	_rt(txt, text);

	if (centered)
		renderer->text(position + c_vector2d(size.x / 2.f, 0), txt, color, font, flags);
	else
		renderer->text(position, txt, color, font, flags);
}

void c_text::set_text(const std::pair<std::string, char> text)
{
	this->text = text;
}

void c_text::set_color(const c_color color)
{
	this->color = color;
}
