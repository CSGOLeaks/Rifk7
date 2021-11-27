#include "../BASS/bass.h"
#include "c_indicators.h"
#include "../sdk/c_engine_client.h"
#include "../hooks/idirect3ddevice9.h"
#include "../hacks/c_fake_ping.h"
#include "../hacks/c_antiaim.h"
#include "../sdk/c_client_state.h"
#include "../utils/math.h"
#include "../sdk/c_input.h"

void c_indicators::draw()
{
	const auto local = c_cs_player::get_local_player();

	if (config.misc.arrows && engine_client()->is_ingame()
		&& local && local->is_alive() && !input->camera_in_third_person)
		draw_antiaim();

	if (!config.misc.indicators)
		return;

	auto position = c_vector2d(20, renderer->get_center().y - 80);

	draw_radio_info(position);	

	if (!engine_client()->is_ingame() || !local || !local->is_alive())
		return;

	draw_choke(position);

	draw_ping_acceptance(position);
}

void c_indicators::draw_radio_info(c_vector2d& position)
{
	if (strlen(BASS::bass_metadata) > 0 && config.misc.radio_channel)
	{
		static const auto radio_info = __("Now playing:");
		static const auto muted = __("MUTED");

		_rt(radio, radio_info);
		renderer->text(position, radio, c_color::primary(), fnv1a("pro13"), c_font::font_flags::drop_shadow);
		position.y += 16;

		renderer->text(position, BASS::bass_metadata, c_color::foreground(), fnv1a("pro13"), c_font::font_flags::drop_shadow);
		position.y += 16;

		if (radio_muted)
		{
			_rt(rad_muted, muted);
			renderer->text(position, rad_muted, c_color::gradient3(), fnv1a("pro13"), c_font::font_flags::drop_shadow);
		}
	}

	position.y += 32;
}

void c_indicators::draw_ping_acceptance(c_vector2d& position)
{
	const auto wanted_ping = fake_ping->calculate_wanted_ping(net_channel) * 1000.f;

	if (wanted_ping > 0.f)
	{
		static const auto ping_acceptance = __("Ping Acceptance - %d ms");

		const auto acceptance = 1.f - std::abs(net_channel->get_latency(flow_incoming) - wanted_ping / 1000.f);
		
		_rt(pa_format, ping_acceptance);
		char pa[40];
		sprintf_s(pa, pa_format, static_cast<int>(wanted_ping));
		draw_progressbar(position, pa, c_color::gradient1(), c_color::accent(), acceptance);
	}
}

void c_indicators::draw_choke(c_vector2d& position)
{
	if (!config.rage.enabled)
		return;

	static const auto choke = __("Choke Amount");
	_rt(ch, choke);
	draw_progressbar(position, ch, c_color::gradient3(), c_color::gradient4(), antiaim->get_visual_choke());
}

void c_indicators::draw_antiaim()
{
	if (!config.rage.enabled)
		return;

	const auto yaw = engine_client()->get_view_angles().y;
	const auto real = deg2rad(yaw - antiaim->get_last_real());
	const auto fake = deg2rad(yaw - antiaim->get_last_fake());

	draw_arrow(real, c_color::gradient3());
	draw_arrow(fake, c_color::primary());
}

void c_indicators::draw_progressbar(c_vector2d& position, const char* name, const c_color color1, const c_color color2, const float progress)
{
	renderer->text(position, name, c_color::foreground(), fnv1a("pro13"), c_font::font_flags::drop_shadow);
	position.y += 16;

	const auto size = c_vector2d(120, 12);
	renderer->rect_filled_linear_gradient(position + c_vector2d(2, 2), c_vector2d(size.x - 4, size.y - 4), color1, color2, true);
	renderer->rect_filled(position + c_vector2d(2 + size.x * std::clamp(progress, 0.f, 1.f), 2),
		c_vector2d(std::clamp(size.x * std::clamp(1.f - progress, 0.f, 1.f) - 4, 0.f, size.x - 4), size.y - 4), c_color::background());
	renderer->rect(position + c_vector2d(1, 1), size - c_vector2d(3, 3), c_color::shadow(100));
	renderer->rect(position, size - c_vector2d(1, 1), c_color::border());

	position.y += 20;
}

void c_indicators::draw_arrow(float angle, c_color color)
{
	c_vector2d pos[8] =
	{
		{ -7.f, -50.f },
		{ -7.f, -140.f },

		{ 7.f, -50.f },
		{ 7.f, -140.f },

		{ -20.f, -130.f },
		{ 0.f, -160.f },

		{ 20.f, -130.f },
		{ 0.f, -160.f }
	};
	
	for (auto& p : pos)
	{
		const auto s = sin(angle);
		const auto c = cos(angle);

		p = c_vector2d(p.x * c - p.y * s, p.x * s + p.y * c) + renderer->get_center();
	}

	renderer->line(pos[0], pos[1], color);
	renderer->line(pos[2], pos[3], color);
	renderer->line(pos[4], pos[5], color);
	renderer->line(pos[6], pos[7], color);
}
