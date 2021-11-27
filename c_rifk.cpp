#include "c_rifk.h"
#include "hooks/c_hooks.h"
#include "hooks/idirect3ddevice9.h"
#include "menu/c_menu.h"
#include <thread>

using namespace std::chrono_literals;

__declspec(dllexport) rifk_loader_info loader_info {};

c_rifk::c_rifk()
{
	c_menu::instance();
	c_netvar::instance();
	c_hooks::run();
}

void runshit(void) {
	c_rifk::instance();
}

//ss
bool APIENTRY DllMain(const HINSTANCE module, const DWORD dw_reason_for_call, LPVOID lp_reserved)  // NOLINT(misc-misplaced-const)
{
	if (dw_reason_for_call == DLL_PROCESS_ATTACH)
	{
		/*
		HANDLE thread;
		
		syscall(NtCreateThreadEx)(&thread, THREAD_ALL_ACCESS, nullptr, current_process,
			nullptr, nullptr, THREAD_CREATE_FLAGS_CREATE_SUSPENDED | THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER, NULL, NULL, NULL, nullptr);

		CONTEXT context;
		context.ContextFlags = CONTEXT_FULL;
		syscall(NtGetContextThread)(thread, &context);

		context.Eax = reinterpret_cast<uint32_t>(&playback_loop);

		syscall(NtSetContextThread)(thread, &context);
		syscall(NtResumeThread)(thread, nullptr);

		if (strlen(loader_info.loader_path) < 2)
		{
			char szDllPath[MAX_PATH];
			GetModuleFileNameA(module, szDllPath, MAX_PATH);
			std::string::size_type pos = std::string(szDllPath).find_last_of("\\/");
			strcpy_s(loader_info.loader_path, std::string(szDllPath).substr(0, pos).c_str());
		}*/

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)runshit, 0, 0, 0);
	}

	return true;
}
