#pragma once

#include "string_obfuscation.h"
#include "../macros.h"
#include "../utils/c_memory.h"

__forceinline void erase_function(uint8_t* function)
{
    const auto end = memory::find_sig(reinterpret_cast<uint32_t>(function), _("90 90 90 90 90"), 0x4000);
	const uint32_t bytes = reinterpret_cast<uint32_t>(end) - reinterpret_cast<uint32_t>(function) + 6;

    void* fn = function;
	ULONG size = bytes, old;
    syscall(NtProtectVirtualMemory)(current_process, &fn, &size, PAGE_EXECUTE_READWRITE, &old);
    fn = function;
    size = bytes;
    memset(fn, 0, size);
    syscall(NtProtectVirtualMemory)(current_process, &fn, &size, old, &old);
}

// NOLINTNEXTLINE
#define erase_fn(a) constexpr auto concat(w, __LINE__) = &a;\
    erase_function(reinterpret_cast<uint8_t*>((void*&)concat(w, __LINE__)))
#define erase_end __asm _emit 0x90; 0x90, 0x90, 0x90, 0x90;
