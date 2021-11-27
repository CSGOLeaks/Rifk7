#include "c_group_box.h"
#include "c_text.h"
#include "../../hooks/idirect3ddevice9.h"

c_group_box::c_group_box(const std::pair<std::string, char> title, const std::initializer_list<std::shared_ptr<c_drawable>> children,
						 const c_vector2d size)	: c_flow_layout(size, c_vector2d(10, 4))
{
	add_child(std::make_shared<c_text>(title, c_color::primary(), fnv1a("pro13"), title_flags));

	for (auto& child : children)
		add_child(child);
}

void c_group_box::draw(c_vector2d position)
{
	position += c_vector2d(1, 6);

	const auto origin = position;
	
	auto first = children[0];
	children.erase(children.begin());

	c_flow_layout::draw(position + padding + c_vector2d(0, 4));

	for (auto& child : children)
		position += child.second.second - child.second.first + padding;

	children.insert(children.begin(), first);

	const auto adjusted_size = c_vector2d(size.x, (position - origin).y) + c_vector2d(0, padding.y + 4);

	first.first->draw(origin + c_vector2d(adjusted_size.x, 0) / 2.f);
	position.y += first.first->size.y;

	// Outer line
	renderer->line(origin - c_vector2d(1, 1), origin + c_vector2d(-1, adjusted_size.y + 1), c_color::shadow(100));
	renderer->line(origin + c_vector2d(0, adjusted_size.y + 1), origin + adjusted_size + c_vector2d(0, 1), c_color::shadow(100));
	renderer->line(origin + adjusted_size + c_vector2d(1, 1), origin + c_vector2d(adjusted_size.x + 1, -1), c_color::shadow(100));

	renderer->line(origin - c_vector2d(0, 1), origin
				   + c_vector2d(adjusted_size.x - children[0].first->get_size().x - padding.x, -2) / 2.f, c_color::shadow(100));
	renderer->line(origin + c_vector2d(adjusted_size.x + children[0].first->get_size().x + padding.x, -2) / 2.f,
				   origin + c_vector2d(adjusted_size.x, -1), c_color::shadow(100));

	// Inner line
	renderer->line(origin + c_vector2d(1, 1), origin + c_vector2d(1, adjusted_size.y - 1), c_color::shadow(100));
	renderer->line(origin + c_vector2d(1, adjusted_size.y - 1), origin + adjusted_size - c_vector2d(1, 1), c_color::shadow(100));
	renderer->line(origin + adjusted_size - c_vector2d(1, 1), origin + c_vector2d(adjusted_size.x - 1, -1), c_color::shadow(100));

	renderer->line(origin + c_vector2d(1, 1), origin
				   + c_vector2d(adjusted_size.x - children[0].first->get_size().x - padding.x, 2) / 2.f, c_color::shadow(100));
	renderer->line(origin + c_vector2d(adjusted_size.x + children[0].first->get_size().x + padding.x, 2) / 2.f,
				   origin + c_vector2d(adjusted_size.x, 1), c_color::shadow(100));

	// Connect line
	renderer->line(origin + c_vector2d(adjusted_size.x - children[0].first->get_size().x - padding.x + 2, -2) / 2.f,
				   origin + c_vector2d(adjusted_size.x - children[0].first->get_size().x - padding.x + 2, 2) / 2.f, c_color::shadow(100));
	renderer->line(origin + c_vector2d(adjusted_size.x + children[0].first->get_size().x + padding.x - 2, -2) / 2.f,
				   origin + c_vector2d(adjusted_size.x + children[0].first->get_size().x + padding.x - 2, 2) / 2.f, c_color::shadow(100));

	// Main line
	renderer->line(origin, origin + c_vector2d(0, adjusted_size.y), c_color::border());
	renderer->line(origin + c_vector2d(0, adjusted_size.y), origin + adjusted_size, c_color::border());
	renderer->line(origin + adjusted_size, origin + c_vector2d(adjusted_size.x, 0), c_color::border());

	renderer->line(origin, origin + c_vector2d(adjusted_size.x - children[0].first->get_size().x - padding.x, 0) / 2.f, c_color::border());
	renderer->line(origin + c_vector2d(adjusted_size.x + children[0].first->get_size().x + padding.x, 0) / 2.f,
				   origin + c_vector2d(adjusted_size.x, 0), c_color::border());
}

c_vector2d c_group_box::get_size()
{
	return size + c_vector2d(16, 16);
}
