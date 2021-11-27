#pragma once

#include "../includes.h"
#include "../hooks/idirect3ddevice9.h"
#include <mutex>

class c_esp
{
	struct player_info
	{
		c_base_handle handle{};
		c_cs_player* player{};
		int32_t index{};
		char name[16]{};
		c_vector3d origin;
		c_qangle angles;
		std::optional<c_vector3d> head_position;
		float duck_amount{};
		int health{}, last_health{}, health_fade{}, ammo{}, ammo_max{};
		bool is_enemy{}, is_on_ground{}, dormant{}, reload{};
		c_base_animating::animation_layers layers{};
		matrix3x4 bones[128]{};
		bool good_bones{};
	};

	struct nade_info
	{
		int32_t type;
		c_vector3d origin;
	};

	struct esp_info
	{
		esp_info() { }  // NOLINT(hicpp-use-equals-default, modernize-use-equals-default)

		std::vector<player_info> players;
		std::vector<nade_info> nades;
		c_vector3d shoot_position{};
		bool is_scoped{};
		viewmatrix world_to_screen_matrix{};
		std::mutex mutex;
	};

public:
	static void draw();
	static void draw_local_impact(c_vector3d start, c_vector3d end);
	static void draw_enemy_impact(c_game_event* event);

	static void store_data();

	inline static esp_info info;
private:
	static constexpr auto esp_flags = c_font::font_flags::centered_x | c_font::font_flags::centered_y | c_font::font_flags::drop_shadow;
	static constexpr auto collision_box_top = 72.f;
	static constexpr auto collision_box_mod = 18.f;
	static constexpr auto head_radius = 6.5f;
	static constexpr auto fade_frequency =  255 / 1.f;

	static void esp_name(player_info& player, const c_vector2d& from, float width, const c_color& color);
	static void esp_box(const c_vector2d& from, float width, float height, const c_color& color);
	static void esp_health(player_info& player, const c_vector2d& from, float height, const c_color& color);
	static void esp_ammo(player_info& player, const c_vector2d& from, float width, float height, const c_color& color);
	static void esp_radar(std::optional<c_vector3d> position, const c_color& color);
    static void esp_skeleton(player_info& player, const c_color& color, bool retro = false);
	static void draw_player(player_info& player);
	static void draw_players();

	static void draw_nade(nade_info& nade);
	static void draw_nades();

	static void draw_scope();
	static void draw_impact(c_vector3d start, c_vector3d end, c_color color);
};
