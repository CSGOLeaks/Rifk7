#include "c_navbar.h"
#include "c_navbar_tab.h"

c_navbar::c_navbar(const std::shared_ptr<c_image> logo, std::vector<std::pair<std::string, char>> items, const c_vector2d size)
	: c_flow_layout(size), active_item(0)
{
	add_child(logo);

	for (uint32_t i = 0; i < items.size(); i++)
		add_child(std::make_shared<c_navbar_tab>(items[i], [&, i]() -> void { active_item = i; }, i == 0, i == items.size() - 1));

	reinterpret_cast<c_navbar_tab*>(children[active_item + 1].first.get())->set_active(true);
}

void c_navbar::draw(c_vector2d position)
{
	static const auto offset = c_vector2d(-70, 10);

	renderer->rect_filled_linear_gradient(position, size, c_color::background_alternate(), c_color::background());
	renderer->rect(position + c_vector2d(1, 1), size - c_vector2d(2, 2), c_color::border());

	for (uint32_t i = 1; i < children.size(); i++)
		reinterpret_cast<c_navbar_tab*>(children[i].first.get())->set_active(i - 1 == active_item);

	auto first = children[0];
	children.erase(children.begin());

	first.first->draw(position - offset);
	first.second = { position - offset, position - offset + first.first->size };
	position.x += first.first->size.x;

	c_flow_layout::draw(position + c_vector2d(35, 5));

	children.insert(children.begin(), first);
}

uint8_t& c_navbar::get_active()
{
	return active_item;
}
