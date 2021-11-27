
#include "c_color_select.h"
#include "c_text.h"
#include "c_button.h"
#include "../../hooks/idirect3ddevice9.h"

c_color_select::c_color_select(std::pair<std::string, char> title, c_color& color)
	: c_flow_layout(c_vector2d(265, 14), c_vector2d(6, 0), true)
{
	add_child(std::initializer_list<std::shared_ptr<c_drawable>>{
		std::make_shared<c_text>(title, c_color::foreground()),
		std::make_shared<c_button>(__("<"), c_vector2d(6, 2), []() -> void {}, [&]() -> void
		{
			color = color.adjust_hue(-speed);
		}),
		std::make_shared<c_color_select_preview>(color, c_vector2d(90, 12)),
		std::make_shared<c_button>(__(">"), c_vector2d(6, 2), []() -> void {}, [&]() -> void
		{
			color = color.adjust_hue(speed);
		})
	});
}

c_color_select::c_color_select_preview::c_color_select_preview(c_color& color, const c_vector2d size)
	: c_drawable(size), color(color) { }

void c_color_select::c_color_select_preview::draw(const c_vector2d position)
{
	renderer->rect_filled_linear_gradient(position, size + c_vector2d(1, 1), c_color::background_alternate(), color);
	renderer->rect(position, size, c_color::shadow(100));
	renderer->rect(position + c_vector2d(1, 1), size - c_vector2d(2, 2), c_color::border());
	renderer->rect(position + c_vector2d(2, 2), size - c_vector2d(4, 4), c_color::shadow(100));
}
