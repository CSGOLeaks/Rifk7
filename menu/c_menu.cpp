#include "c_menu.h"
#include "c_visuals.h"
#include "c_aiming.h"
#include "c_misc.h"
#include "c_welcome_message.h"
#include "framework/c_router.h"
#include "framework/c_navbar.h"
#include "framework/c_text.h"
#include "../hooks/idirect3ddevice9.h"
#include "../sdk/c_input_system.h"
#include "../images/logo.h"
#include "c_conf.h"
#include <memory>

c_menu::c_menu() : c_flow_layout(renderer->get_center() * 2.f), welcome_placeholder(0), open(false), alpha(0), cursor_pos(0, 0)
{
	hotkeys->register_callback([&](auto code) -> void {
		if (code != VK_INSERT || (alpha > 0 && alpha < 255))
			return;

		open = !open;
		// reset hold
		if (current_held.has_value())
		{
			current_held.value()->held = false;
			set_current_held(std::nullopt);
		}

		input_system()->enable_input(!open);
		input_system()->reset_input_state();
	});

	add_child(std::make_shared<c_navbar>(
		std::make_shared<c_image>(c_vector2d(42, 42), logo, sizeof(logo), .2f),
		std::initializer_list<std::pair<std::string, char>> {
			__("Visuals"),
			__("Aiming"),
			__("Misc"),
			__("Skins"),
			__("Config")
		}
	));

	add_child(std::make_shared<c_router>(std::initializer_list<std::shared_ptr<c_drawable>> {
		std::make_shared<c_welcome_message>()
	}, welcome_placeholder, false, c_vector2d(34, 8), c_vector2d(586, 112)));

	add_child(std::make_shared<c_router>(std::initializer_list<std::shared_ptr<c_drawable>> {
		make_child(c_visuals),
		make_child(c_aiming),
		make_child(c_misc),
		make_child(c_flow_layout, c_vector2d(600, 400)),
		make_child(c_conf)
	}, reinterpret_cast<c_navbar*>(children[0].first.get())->get_active(), true));
}

void c_menu::draw(const c_vector2d position)
{
	c_menu::animate();

	if (alpha <= 0)
		return;

	children[0].first->size.x = renderer->get_center().x * 2.f;

	c_color::set_alpha_override(alpha);

	children[0].first->draw(position);
	if (renderer->get_center().y * 2.f >= 760.f)
		children[1].first->draw(renderer->get_center()
			- c_vector2d(children[1].first->get_size().x / 2.f, children[2].first->get_size().y - 100));
	children[2].first->draw(renderer->get_center() - children[2].first->get_size() / 2.f + c_vector2d(0, 20));

	c_color::set_alpha_override(std::nullopt);
}

std::optional<c_drawable*> c_menu::get_foreground_element_by_position(const c_vector2d position)
{
	if (position.y <= children[0].first->get_size().y)
		return children[0].first->get_foreground_element_by_position(position);

	return children[2].first->get_foreground_element_by_position(position);
}

void c_menu::animate()
{
	// Scale to current resolution
	renderer->refresh_viewport();
	size = renderer->get_center() * 2.f;

	if (alpha > 0)
		c_flow_layout::animate();

	linear_fade(alpha, 0, 255, alpha_frequency, is_open());
}

void c_menu::update_cursor_pos(const uint32_t param)
{
	if (!open)
		return;

	cursor_pos.x = static_cast<float>(GET_X_LPARAM(param));
	cursor_pos.y = static_cast<float>(GET_Y_LPARAM(param));

	const auto hovered = get_foreground_element_by_position(cursor_pos);

	if (hovered.has_value())
	{
		if (!last_active.has_value() || last_active.value() != hovered.value())
		{
			if (last_active.has_value())
				last_active.value()->on_mouse_leave();

			last_active = std::optional<c_drawable*>(hovered);
			last_active.value()->on_mouse_enter();
		}
	}
	else if (last_active.has_value())
	{
		last_active.value()->on_mouse_leave();
		last_active = std::nullopt;
	}
}

void c_menu::register_click(const uint32_t param)
{
	if (!open)
		return;

	update_cursor_pos(param);

	if (last_active.has_value())
	{
		last_active.value()->on_click();
		set_current_held(last_clicked = last_active);
	}
}

void c_menu::register_unclick(const uint32_t param)
{
	if (!open)
		return;

	update_cursor_pos(param);

	if (last_clicked.has_value())
	{
		last_clicked.value()->on_unclick();
		set_current_held(last_clicked = std::nullopt);
	}
}

bool c_menu::is_open() const
{
	return open;
}
