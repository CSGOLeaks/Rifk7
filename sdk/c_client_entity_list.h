#pragma once

#include <functional>
#include "macros.h"
#include "c_client_entity.h"
#include "c_engine_client.h"
#include "c_cs_player.h"

class c_client_entity_list {
protected:
	~c_client_entity_list() = default;
public:
	virtual c_client_networkable* get_client_networkable(int ent) = 0;
	virtual c_client_networkable* get_client_networkable_from_handle(c_base_handle ent) = 0;
	virtual c_client_unknown* get_client_unknown_from_handle(c_base_handle ent) = 0;
	virtual c_client_entity* get_client_entity(int ent) = 0;
	virtual c_client_entity* get_client_entity_from_handle(c_base_handle ent) = 0;
	virtual int32_t number_of_entities(bool include_non_networkable) = 0;
	virtual int32_t get_highest_entity_index() = 0;
	virtual void set_max_entities(int max) = 0;
	virtual int32_t get_max_entities() = 0;

	void for_each(const std::function<void(c_client_entity*)> fn)
	{
		for (auto i = 0; i <= get_highest_entity_index(); ++i) {
			const auto entity = get_client_entity(i);
			if (entity)
				fn(entity);
		}
	}

	void for_each_player(const std::function<void(c_cs_player*)> fn)
	{
		for (auto i = 0; i <= static_cast<int32_t>(engine_client()->get_max_clients()); ++i) {
			const auto entity = get_client_entity(i);
			if (entity && entity->is_player())
				fn(reinterpret_cast<c_cs_player*>(entity));
		}
	}

	void for_each_player_fixed_z_order(const std::function<void(c_cs_player*)> fn)
	{
		std::vector<std::pair<c_cs_player*, float>> order;
		order.reserve(64);

		const auto local = c_cs_player::get_local_player();
		if (!local)
			return;

		const auto origin = local->get_origin();

		for_each_player([&](c_cs_player* player) -> void
		{
			if (player->is_dormant() || !player->is_alive() || player->is_local_player())
				return;

			// do not use shoot position here, the underlaying virtual does not lock the studiohdr
			// this breaks the entire animation state and is a pain in the ass to debug!
			order.push_back(std::make_pair(player,
				(player->get_origin() - origin).length2d()));
		});

		// closest players will be drawn last
		// so their esp is actually going to overlap players with bigger distances
		std::sort(order.begin(), order.end(),
			[](const std::pair<c_cs_player*, float>& a,
				const std::pair<c_cs_player*, float>& b) -> bool {
			return a.second > b.second;
		});

		for (auto const& entity : order)
			fn(entity.first);
	}
};

interface_var(c_client_entity_list, client_entity_list, "client_panorama.dll", "VClientEntityList")
