#include "c_flow_layout.h"
#include "../../hooks/idirect3ddevice9.h"

std::optional<c_drawable*> c_flow_layout::current_held = std::nullopt;
float c_flow_layout::held_propagation_delay = default_held_propagation_delay;

c_flow_layout::c_flow_layout(const c_vector2d size, const c_vector2d padding, const bool splitted)
	: c_drawable(size), padding(padding), splitted(splitted) { }

void c_flow_layout::draw(c_vector2d position)
{
	const auto draw_layout = [&] () -> void
	{
		if (children.empty())
			return;

		auto original = position;
		c_vector2d last_size(0, 0);

		for (auto& child : children)
		{
			if (position.x + child.first->get_size().x - original.x >= size.x)
			{
				original.y += last_size.y + padding.y;
				position = original;
			}

			const auto limit = renderer->get_limit();
			const auto from = position;

			child.first->draw(position);
			position += c_vector2d(child.first->get_size().x + padding.x, 0);

			const auto to = position + c_vector2d(0, child.first->get_size().y);

			auto clamped = rectangle(
				{
					std::clamp(from.x, limit.first.x, limit.second.x),
					std::clamp(from.y, limit.first.y, limit.second.y)
				},
			{
				std::clamp(to.x, limit.first.x, limit.second.x),
				std::clamp(to.y, limit.first.y, limit.second.y)
			});

			if (from.x < limit.first.x && to.x < limit.second.x)
				clamped.first.x = clamped.second.x = -1.f;

			if (from.y < limit.first.y && to.y < limit.second.y)
				clamped.first.y = clamped.second.y = -1.f;

			child.second = clamped;
			last_size = child.first->get_size();
		}
	};

	if (current_held.has_value())
		for (auto& child : children)
		{
			if (child.first->held)
			{
				if (current_held.value() == child.first.get()
					&& held_propagation_delay >= .0f)
					held_propagation_delay -= global_vars_base->frametime;

				if (held_propagation_delay < .0f)
					child.first->on_hold();

				break;
			}
		}

	if (!splitted)
		return draw_layout();

	const auto first = children[0];
	children.erase(children.begin());

	first.first->draw(position);

	auto total_width = .0f;
	for (auto& child : children)
		total_width += child.first->get_size().x + padding.x;

	position += c_vector2d(size.x - total_width, 0);
	draw_layout();

	children.insert(children.begin(), first);
}

std::optional<c_drawable*> c_flow_layout::get_foreground_element_by_position(const c_vector2d position)
{
	for (auto& child : children)
	{
		if (child.first && position.x >= child.second.first.x && position.y >= child.second.first.y
			&& position.x <= child.second.second.x && position.y <= child.second.second.y)
			return child.first->get_foreground_element_by_position(position);
	}

	return std::nullopt;
}

void c_flow_layout::set_current_held(const std::optional<c_drawable*> held)
{
	current_held = held;
	held_propagation_delay = default_held_propagation_delay;
}
