#include "c_router.h"
#include "../../hooks/idirect3ddevice9.h"

c_router::c_router(const std::initializer_list<std::shared_ptr<c_drawable>> children,
				   uint8_t& active_item, const bool primary, const c_vector2d padding, const c_vector2d size)
	: c_flow_layout(size + padding), active_item(active_item), padding(padding), primary(primary)
{
	for (auto& child : children)
		add_child(child);
}

void c_router::draw(c_vector2d position)
{
	position.y -= 2;

	renderer->rect_filled(position, size, c_color::background());
	renderer->rect(position + c_vector2d(1, 1), size - c_vector2d(3, 3), c_color::shadow(100));
	renderer->rect(position + c_vector2d(3, 3), size - c_vector2d(7, 7), c_color::shadow(100));

	if (primary)
	{
		const auto stage1_fade = std::clamp(fade / 1000.f, 0.f, .25f) * 4.f;
		const auto stage2_fade = std::clamp(fade / 1000.f - .25f, 0.f, .25f) * 4.f;
		const auto stage3_fade = std::clamp(fade / 1000.f - .5f, 0.f, .25f) * 4.f;
		const auto stage4_fade = std::clamp(fade / 1000.f - .75f, 0.f, .25f) * 4.f;
		
		const auto color1 = c_color::gradient2().fade_to(c_color::primary(), stage1_fade).fade_to(c_color::gradient1(), stage2_fade)
			.fade_to(c_color::accent(), stage3_fade).fade_to(c_color::gradient2(), stage4_fade);
		const auto color2 = c_color::accent().fade_to(c_color::gradient2(), stage1_fade).fade_to(c_color::primary(), stage2_fade)
			.fade_to(c_color::gradient1(), stage3_fade).fade_to(c_color::accent(), stage4_fade);
		const auto color3 = c_color::gradient1().fade_to(c_color::accent(), stage1_fade).fade_to(c_color::gradient2(), stage2_fade)
			.fade_to(c_color::primary(), stage3_fade).fade_to(c_color::gradient1(), stage4_fade);
		const auto color4 = c_color::primary().fade_to(c_color::gradient1(), stage1_fade).fade_to(c_color::accent(), stage2_fade)
			.fade_to(c_color::gradient2(), stage3_fade).fade_to(c_color::primary(), stage4_fade);

		renderer->rect_full_linear_gradient(position, size - c_vector2d(1, 1),
			color1,	color2,	color3,	color4);
		renderer->rect_full_linear_gradient(position + c_vector2d(2, 2), size - c_vector2d(5, 5),
			color1,	color2,	color3,	color4);
	}
	else
	{
		renderer->rect(position, size - c_vector2d(1, 1), c_color::border());
		renderer->rect(position + c_vector2d(2, 2), size - c_vector2d(5, 5), c_color::border());
	}

	const rectangle inner = {
		c_vector2d(position + padding / 2.f),
		c_vector2d(position + size - padding / 2.f)
	};

	renderer->limit(inner);

	if (children.size() > active_item)
		children[active_item].first->draw(inner.first);

	renderer->reset_limit();
}

void c_router::animate()
{
	c_flow_layout::animate();

	linear_fade(fade, 0, 1000, fade_frequency, true);

	if (fade == 1000)
		fade = 0;
}

std::optional<c_drawable*> c_router::get_foreground_element_by_position(const c_vector2d position)
{
	if (children.size() > active_item)
		return children[active_item].first->get_foreground_element_by_position(position);

	return std::nullopt;
}
