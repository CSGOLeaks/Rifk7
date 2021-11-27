#include "c_smoke_grenade_projectile_.h"
#include "../sdk/recv_prop_hook.h"
#include "../sdk/macros.h"
#include "../utils/c_config.h"

void c_smoke_grenade_projectile_::hook()
{
	class c_smoke_grenade_projectile
	{
	public:
		netprop(get_did_smoke_effect_prop(), "CSmokeGrenadeProjectile", "m_bDidSmokeEffect")
	};

	static auto did_smoke_effect = recv_prop_hook(c_smoke_grenade_projectile::get_did_smoke_effect_prop());
	_m_bDidSmokeEffect = did_smoke_effect.hook_prop(m_bDidSmokeEffect);
}

void __cdecl c_smoke_grenade_projectile_::m_bDidSmokeEffect(c_recv_proxy_data* data, void* entity, void* output)
{
	_m_bDidSmokeEffect(data, entity, output);
	if (config.misc.no_smoke)
		*reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(output) + 0x1) = true;
}
