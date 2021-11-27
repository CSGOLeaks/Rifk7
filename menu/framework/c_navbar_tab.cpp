#include "c_navbar_tab.h"
#include "c_text.h"
#include "../../hooks/idirect3ddevice9.h"

c_navbar_tab::c_navbar_tab(const std::pair<std::string, char> text, const std::function<void()> activate,
						   const bool first, const bool last, const c_vector2d size) :
	c_text(text, c_color::foreground(), fnv1a("pro17"), flags), activate(activate), highlight(0), first(first), last(last), is_active(false)
{
	this->size = size;
}

void c_navbar_tab::draw(const c_vector2d position)
{
	const auto corrected_size = size + c_vector2d(8, 0);
	const auto side = first ? 1 : last ? 2 : 0;

	renderer->parallelogram_filled_linear_gradient(position, corrected_size, c_color::background_alternate(), c_color::background(), side);
	renderer->parallelogram(position, corrected_size, c_color::border(), side);
	renderer->parallelogram(position + c_vector2d(2, 2), corrected_size - c_vector2d(4, 4), c_color::border(), side);

	if (is_active)
		set_color(c_color::primary());
	else
		set_color(c_color::foreground().fade_to(c_color::primary(), highlight / 255.f));

	c_text::draw(position + corrected_size / 2.f);
}

void c_navbar_tab::animate()
{
	linear_fade(highlight, 0, 255, highlight_frequency, is_hovered());
}

void c_navbar_tab::on_click()
{
	activate();
}

void c_navbar_tab::set_active(const bool is_active)
{
	// Fade out if other tab becomes active
	if (this->is_active && !is_active)
		highlight = 255;

	this->is_active = is_active;
}
