#include "c_base_animating_.h"
#include "../sdk/recv_prop_hook.h"
#include "../sdk/c_base_animating.h"
#include "../sdk/c_cs_player.h"

void c_base_animating_::hook()
{
	static auto client_side_animation = recv_prop_hook(c_base_animating::get_client_side_animation_prop());
	_m_bClientSideAnimation = client_side_animation.hook_prop(m_bClientSideAnimation);
}

// ReSharper disable once CppInconsistentNaming
void c_base_animating_::m_bClientSideAnimation(c_recv_proxy_data* data, void* entity, void* output)
{
	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return _m_bClientSideAnimation(data, entity, output);
	
	const auto player = reinterpret_cast<c_cs_player*>(entity);

	if(player->is_player() && player->is_enemy())
		data->value.m_Int = 0;

	_m_bClientSideAnimation(data, entity, output);
}
