#include "c_button.h"
#include "../../hooks/idirect3ddevice9.h"

c_button::c_button(const std::pair<std::string, char> text, const c_vector2d padding,
				   const std::function<void()> on_click, const std::function<void()> on_hold)
	: c_text(text, c_color::foreground(), fnv1a("pro12"), button_flags), fade(0),
		padding(padding), click(on_click), hold(on_hold) { }

void c_button::draw(const c_vector2d position)
{
	renderer->rect_filled_linear_gradient(position, get_size() + c_vector2d(1, 1),
										  c_color::background_alternate().fade_to(c_color::background(), fade / 255.f),
										  c_color::background().fade_to(c_color::background_alternate(), fade / 255.f));

	renderer->rect(position, get_size(), c_color::shadow(100));
	
	if (held)
	{
		renderer->rect(position + c_vector2d(1, 1), get_size() - c_vector2d(2, 2), c_color::accent());
		set_color(c_color::accent());
	}		
	else
	{
		renderer->rect(position + c_vector2d(1, 1), get_size() - c_vector2d(2, 2), c_color::border().fade_to(c_color::primary(), fade / 255.f));
		set_color(c_color::foreground().fade_to(c_color::primary(), fade / 255.f));
	}

	renderer->rect(position + c_vector2d(2, 2), get_size() - c_vector2d(4, 4), c_color::shadow(100));

	c_text::draw(position + size / 2.f + c_vector2d(padding.x, 0) / 4.f);
}

void c_button::animate()
{
	linear_fade(fade, 0, 255, fade_frequency, is_hovered());
}

void c_button::on_click()
{
	c_drawable::on_click();
	click();
}

void c_button::on_hold()
{
	c_drawable::on_hold();
	hold();
}

c_vector2d c_button::get_size()
{
	return c_text::get_size() + padding;
}
