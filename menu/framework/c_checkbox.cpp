
#include "c_checkbox.h"
#include "c_text.h"
#include "../../hooks/idirect3ddevice9.h"

c_checkbox::c_checkbox_inner::c_checkbox_inner(bool& value) : c_drawable(c_vector2d(11, 11)),
                                                              fade(0), enabled(0), value(value)
															  { }

void c_checkbox::c_checkbox_inner::draw(const c_vector2d position)
{
	renderer->rect_filled_linear_gradient(position, size + c_vector2d(1, 1),
										  c_color::background_alternate().fade_to(c_color::background(), fade / 255.f),
										  c_color::background().fade_to(c_color::background_alternate(), fade / 255.f)
											.fade_to(c_color::primary(), enabled / 255.f));

	renderer->rect(position, size, c_color::shadow(100));
	renderer->rect(position + c_vector2d(1, 1), size - c_vector2d(2, 2), c_color::border().fade_to(c_color::primary(), fade / 255.f));
	renderer->rect(position + c_vector2d(2, 2), size - c_vector2d(4, 4), c_color::shadow(100));
}

void c_checkbox::c_checkbox_inner::animate()
{
	linear_fade(fade, 0, 255, fade_frequency, is_hovered());
	linear_fade(enabled, 0, 255, enabled_frequency, value);
}

void c_checkbox::c_checkbox_inner::on_click()
{
	value = !value;
}

c_checkbox::c_checkbox(std::pair<std::string, char> title, bool& value)
	: c_flow_layout(c_vector2d(265, 14), c_vector2d(6, 0), true)
{
	add_child(std::make_shared<c_text>(title, c_color::foreground()));
	add_child(std::make_shared<c_checkbox_inner>(value));
}
