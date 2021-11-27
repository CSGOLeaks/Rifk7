
#include "c_welcome_message.h"
#include "../hooks/idirect3ddevice9.h"
#include "../utils/welcome_message.h"

c_welcome_message::c_welcome_message()
	: c_text(welcome_message, c_color::primary(), fnv1a("pro13"), c_font::font_flags::drop_shadow),
	  fade_direction(false), fade(0) { }

void c_welcome_message::draw(c_vector2d position)
{
	const auto primary = c_color::primary().fade_to(c_color::shadow(100), fade / 255.f * .5f, true);
	const auto accent = c_color::accent().fade_to(c_color::shadow(100), fade / 255.f * .5f, true);

	uint8_t line = 0;
	_rt(txt, text);

	char* pos;
    auto prev = txt;
	while ((pos = strchr(prev, '\n')))
    {
		// Crazy inline magic for really fast rendering...

        const auto divider = 50 - 2 * (line < 3) + (line == 2);

		const auto backup_target = *pos;
		*pos = '\0';

		const auto backup_divider = prev[divider];
		prev[divider] = '\0';
		const auto partial_size = renderer->get_text_size(prev, font);

		renderer->text(position, prev, primary, font, flags);
		prev[divider] = backup_divider;
		renderer->text(position + c_vector2d(partial_size.x, 0), &prev[divider], accent, font, flags);

		*pos = backup_target;

		position.y += partial_size.y;
		prev = &pos[1];

		if (++line > 4)
			break;
    }

	renderer->text(position, prev, primary, font, flags);
}

void c_welcome_message::animate()
{
	linear_fade(fade, 0, 255, fade_frequency, fade_direction);

	if (fade == 0 || fade == 255)
		fade_direction = !fade_direction;
}

c_vector2d c_welcome_message::get_size()
{
	return c_vector2d(340, 115);
}
