#include "c_cl_camera_height_restriction_debug.h"
#include "../sdk/c_cvar.h"
#include "../sdk/c_cs_player.h"
#include "../utils/c_hook.h"

void c_cl_camera_height_restriction_debug::hook()
{
	static c_hook<convar> hook(cvar()->find_var(_("cl_camera_height_restriction_debug")));
	hook.apply<void(__thiscall*)()>(13, get_bool);
}

int __stdcall c_cl_camera_height_restriction_debug::proxy_get_bool(convar* var, void* return_address, void* eax)
{
	static auto return_to_modify_eye_pos = sig("client_panorama.dll", "85 C0 0F 84 ? ? ? ? 8B 45 08 8B 0D");

	const auto local = c_cs_player::get_local_player();

	if (return_address == return_to_modify_eye_pos)
		return !in_cm || (local && !local->get_anim_state()->in_hit_ground_animation);

    return 0;
}

// ReSharper disable once CppDeclaratorNeverUsed
static uint32_t get_bool_retn_address = 0;

__declspec(naked) bool get_bool(void* ecx, void* edx)
{
    _asm
    {
        push eax

        mov eax, [esp + 4]
        mov get_bool_retn_address, eax

        push get_bool_retn_address
        push ecx
        call c_cl_camera_height_restriction_debug::proxy_get_bool

        cmp eax, 0
        je _retn0

        cmp eax, 1
        je _retn1

        cmp eax, 2
        je _retn2
    	
    _retn0:
        mov al, 0
            retn

    _retn1:
        pop eax

        mov eax, get_bool_retn_address
        add eax, 0x462

        push eax

        retn

    _retn2:
        pop eax

        mov eax, get_bool_retn_address
        add eax, 0x462

        push eax

        retn
    }
}
