
#include "c_hooks.h"
#include "c_wnd_proc.h"
#include "idirect3ddevice9.h"
#include "c_client_mode_shared.h"
#include "c_surface_.h"
#include "c_events.h"
#include "c_model_render_.h"
#include "c_client.h"
#include "c_game_event_manager_.h"
#include "c_engine_client.h"
#include "c_base_animating_.h"
#include "c_cl_csm_shadows.h"
#include "c_net_channel_.h"
#include "c_cs_player_.h"
#include "dbghelp.h"
#include <thread>
#include "c_cl_camera_height_restriction_debug.h"
#include "c_view_render_.h"
#include "c_prediction_.h"
#include "c_client_state_.h"
#include "c_mdl_cache.h"
#include "c_render_view_.h"
#include "c_engine_sound_.h"
#include "c_sv_cheats.h"
#include "c_panel_.h"
#include "c_smoke_grenade_projectile_.h"

using namespace std::chrono_literals;

void c_hooks::run()
{
#ifdef RELEASE
	HANDLE thread;
	syscall(NtCreateThreadEx)(&thread, THREAD_ALL_ACCESS, nullptr, current_process,
		nullptr, nullptr, THREAD_CREATE_FLAGS_CREATE_SUSPENDED | THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER, NULL, NULL, NULL, nullptr);

	CONTEXT context;
	context.ContextFlags = CONTEXT_FULL;
	syscall(NtGetContextThread)(thread, &context);

	context.Eax = reinterpret_cast<uint32_t>(&reflection_creation);

	syscall(NtSetContextThread)(thread, &context);
	syscall(NtResumeThread)(thread, nullptr);

	// wait for success of copy-on-write
	std::this_thread::sleep_for(2s);
#endif

	c_client_mode_shared::hook();
	c_wnd_proc::hook();
	c_net_channel_::hook();
	c_cs_player_::hook();
	c_surface_::hook();
	c_events::hook();
	c_render_view_::hook();
	c_model_render_::hook();
	c_client::hook();
	c_game_event_manager_::hook();
	c_base_animating_::hook();
	c_engine_client_::hook();
	c_cl_csm_shadows::hook();
	c_cl_camera_height_restriction_debug::hook();
	c_view_render_::hook();
	c_prediction_::hook();
	c_client_state_::hook();
	c_mdl_cache::hook();
	c_engine_sound_::hook();
	c_sv_cheats::hook();
	c_panel_::hook();
	c_smoke_grenade_projectile_::hook();

	logging->info(_("Ready, set, go!"));
}

#ifdef RELEASE
void reflection_creation()
{
	/*
		.text:4B32B37C E8 4F EF FB FF                                call    ZwClose
		.text:4B32B381 39 7D F4                                      cmp     [ebp+var_C], edi
		.text:4B32B384 74 12                                         jz      short loc_4B32B398
		.text:4B32B386 57                                            push    edi
		.text:4B32B387 57                                            push    edi
		.text:4B32B388 FF 75 F4                                      push    [ebp+var_C]
		.text:4B32B38B E8 90 EE FB FF                                call    ZwWaitForSingleObject
		.text:4B32B390 FF 75 F4                                      push    [ebp+var_C]
		.text:4B32B393 E8 38 EF FB FF                                call    ZwClose
	 */
	RTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION info = {};
	import("ntdll.dll", RtlCreateProcessReflection)(current_process, 0, nullptr, nullptr, nullptr, &info);
}
#endif
