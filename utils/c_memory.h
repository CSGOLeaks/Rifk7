#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <wintrust.h>
#include <softpub.h>
#include "defs.h"
#include "../security/fnv1a.h"
#include "../security/string_obfuscation.h"

#pragma comment(lib, "Wintrust.lib")

namespace memory
{
	__forceinline uintptr_t get_module_handle(const uint32_t module, const uint32_t process = 0)
	{
		MODULEENTRY32 entry;
		entry.dwSize = sizeof(MODULEENTRY32);

		const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process);

		if (Module32First(snapshot, &entry))
			while (Module32Next(snapshot, &entry))
			{
				std::string name = entry.szModule;
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);

				if (fnv1a_rt(name.c_str()) == module)
				{
					CloseHandle(snapshot);
					return reinterpret_cast<uintptr_t>(entry.hModule);
				}
			}

		CloseHandle(snapshot);
		return 0;
	}

	__forceinline int32_t rva_2_offset(const uint32_t rva, PIMAGE_NT_HEADERS nt_headers, const bool in_memory = false) {
		if (rva == 0 || !in_memory)
			return rva;

		auto sec = IMAGE_FIRST_SECTION(nt_headers);
		for (size_t i = 0; i < nt_headers->FileHeader.NumberOfSections; i++) {
			if (rva >= sec->VirtualAddress && rva < sec->VirtualAddress + sec->Misc.VirtualSize)
				break;
			sec++;
		}

		return rva - sec->VirtualAddress + sec->PointerToRawData;
	}

	__forceinline uintptr_t get_proc_address(const uintptr_t module, const uint32_t function, const bool in_memory = false)
	{
		const auto dos_headers = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
		if (dos_headers->e_magic != IMAGE_DOS_SIGNATURE)
			return 0;

		const auto nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(dos_headers->e_lfanew + module);
		if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
			return 0;

		const auto exports = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(
			rva_2_offset(nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress,
				nt_headers, in_memory) + module);

		const auto names = reinterpret_cast<uint32_t*>(rva_2_offset(exports->AddressOfNames, nt_headers, in_memory) + module);

		auto ordinal_index = static_cast<uint32_t>(-1);
		for (uint32_t i = 0; i < exports->NumberOfFunctions; i++)
		{
			const auto function_name = reinterpret_cast<const char*>(rva_2_offset(names[i], nt_headers, in_memory) + module);

			if (fnv1a_rt(function_name) == function)
			{
				ordinal_index = i;
				break;
			}
		}

		if (ordinal_index > exports->NumberOfFunctions)
			return 0;

		const auto ordinals = reinterpret_cast<uint16_t*>(rva_2_offset(exports->AddressOfNameOrdinals, nt_headers, in_memory) + module);
		const auto addresses = reinterpret_cast<uint32_t*>(rva_2_offset(exports->AddressOfFunctions, nt_headers, in_memory) + module);
		return rva_2_offset(addresses[ordinals[ordinal_index]], nt_headers, in_memory) + module;
	}

	__forceinline uint8_t* find_sig(const uint32_t offset, const char* signature, const uint32_t range = 0u)
    {
        static auto pattern_to_bytes = [](const char* pattern) -> std::vector<int> {
            auto bytes = std::vector<int32_t>{ };
	        const auto start = const_cast<char*>(pattern);
	        const auto end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto current = start; current < end; ++current) {
                if (*current == '?') {
                    current++;

                    if (*current == '?')
                        current++;

                    bytes.push_back(-1);
                }
                else
                    bytes.push_back(static_cast<int32_t>(strtoul(current, &current, 0x10)));
            }

            return bytes;
        };

        const auto scan_bytes = reinterpret_cast<std::uint8_t*>(offset);
        auto pattern_bytes = pattern_to_bytes(signature);
        const auto s = pattern_bytes.size();
        const auto d = pattern_bytes.data();

        for (auto i = 0ul; i < range - s; ++i) {
            auto found = true;

            for (auto j = 0ul; j < s; ++j)
                if (scan_bytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }

            if (found)
                return &scan_bytes[i];
        }

        return nullptr;
    }

    __forceinline uint8_t* find_module_sig(const uint32_t name, const char* signature)
    {
        const auto module = get_module_handle(name);

        if (module) {
            const auto dos_header = PIMAGE_DOS_HEADER(module);
            const auto nt_headers = PIMAGE_NT_HEADERS(reinterpret_cast<std::uint8_t*>(module) + dos_header->e_lfanew);

            return find_sig(module, signature, nt_headers->OptionalHeader.SizeOfImage);
        }

        return nullptr;
    }

	template <uint32_t module, uint32_t function>
	__forceinline static void* import()
	{
		static void* fn;

		if (!fn)
			fn = reinterpret_cast<void*>(get_proc_address(get_module_handle(module), function));

		return fn;
	}
}

#define imp(module, name) (reinterpret_cast<decltype(&(name))>(memory::import<fnv1a(module), fnv1a(#name)>()))
#define import(module, name) (reinterpret_cast<name>(memory::import<fnv1a(module), fnv1a(#name)>()))
#define sig(name, signature) memory::find_module_sig(fnv1a(name), _ot(signature))

namespace memory
{
	struct syscall_fn {
		uint32_t index;
		uint16_t retn;
	};

	__forceinline bool verify_integrity(const char* path) {
		std::string source(path);
		std::wstring file(source.begin(), source.end());

		WINTRUST_FILE_INFO file_data;
		memset(&file_data, 0, sizeof(file_data));
		file_data.cbStruct = sizeof(WINTRUST_FILE_INFO);
		file_data.pcwszFilePath = file.c_str();
		file_data.hFile = nullptr;
		file_data.pgKnownSubject = nullptr;

		GUID guid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		WINTRUST_DATA wintrust;
		memset(&wintrust, 0, sizeof(wintrust));

		wintrust.cbStruct = sizeof(wintrust);
		wintrust.pPolicyCallbackData = nullptr;
		wintrust.pSIPClientData = nullptr;
		wintrust.dwUIChoice = WTD_UI_NONE;
		wintrust.fdwRevocationChecks = WTD_REVOKE_NONE;
		wintrust.dwUnionChoice = WTD_CHOICE_FILE;
		wintrust.dwStateAction = WTD_STATEACTION_VERIFY;
		wintrust.hWVTStateData = nullptr;
		wintrust.pwszURLReference = nullptr;
		wintrust.dwUIContext = 0;
		wintrust.pFile = &file_data;

		const uint32_t status = WinVerifyTrust(nullptr, &guid, &wintrust);
		wintrust.dwStateAction = WTD_STATEACTION_CLOSE;
		WinVerifyTrust(nullptr, &guid, &wintrust);
		return status == ERROR_SUCCESS;
	}

	__forceinline uint8_t* locate_wow64_transition()
	{
		static uint8_t* wow64_transition;

		if (!wow64_transition)
			wow64_transition = reinterpret_cast<uint8_t*>(get_proc_address(
				get_module_handle(fnv1a("win32u.dll")), fnv1a("Wow64Transition")));

		return wow64_transition;
	}

	__forceinline bool verify_call_integrity(const uint8_t* shell)
	{
		const auto jmp_pad_cmp = *reinterpret_cast<uint8_t**>(&const_cast<uint8_t*>(shell)[6]);
		const auto transition_cmp = *reinterpret_cast<uint8_t**>(&jmp_pad_cmp[2]);
		
		return transition_cmp == locate_wow64_transition();
	}

	__forceinline syscall_fn extract_syscall(const uint32_t function)
	{
		syscall_fn fn = { };

		if (!verify_integrity(_("C:\\Windows\\SysWOW64\\ntdll.dll")))
			return fn;

		std::ifstream file(_("C:\\Windows\\SysWOW64\\ntdll.dll"), std::ios::binary | std::ios::ate);
		const std::streamsize size = static_cast<uint32_t>(file.tellg());
		file.seekg(0, std::ios::beg);

		std::vector<char> buffer(static_cast<uint32_t>(size));
		if (file.read(buffer.data(), size))
		{
			const auto proc = get_proc_address(reinterpret_cast<uintptr_t>(buffer.data()), function, true);
			const auto id = *reinterpret_cast<uint32_t*>(proc + 1);
			const auto ret = *reinterpret_cast<uint16_t*>(proc + 0xD);
			fn = { id, ret };
		};

		return fn;
	}

	__forceinline static uint8_t* allocate_proxy() {
		static std::vector<uint8_t> pad =
		{
				0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 // far jmp wow64_transition
		};

		static std::vector<uint8_t> sys =
		{
				0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, index
				0xBA, 0x00, 0x00, 0x00, 0x00, // mov edx, wow64_jumppad
				0xFF, 0xD2, // call wow64_transition
				0xC2, 0x00, 0x00 // retn error_code
		};

		static uint8_t* proxy = nullptr;
		static uint8_t* jmp_pad = nullptr;

		if (proxy)
			return proxy;

		// Allocate jumppad
		auto size = pad.size();
		import("ntdll.dll", NtAllocateVirtualMemory)(current_process, reinterpret_cast<PVOID*>(&jmp_pad), nullptr,
			&size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		size = pad.size();
		*reinterpret_cast<uint32_t*>(&pad[2]) = reinterpret_cast<uint32_t>(locate_wow64_transition());
		memcpy(jmp_pad, &pad[0], size);

		// Allocate shellcode
		uint8_t* sys_code = nullptr;
		size = sys.size();
		import("ntdll.dll", NtAllocateVirtualMemory)(current_process, reinterpret_cast<PVOID*>(&sys_code), nullptr,
			&size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		size = sys.size();
		*reinterpret_cast<uint32_t*>(&sys[6]) = reinterpret_cast<uint32_t>(jmp_pad);
		memcpy(sys_code, &sys[0], size);
		return proxy = sys_code;
	}

	/*
	 * Not thread safe, make sure to implement a mutex in the future.
	 * We need to make sure that two threads never override the shellcode at the same time.
	 * Considering the fact these are native functions, the result might be catastrophic.
	 *
	 * This also only allows for immediate calls right now.
	 */
	template <uint32_t function>
	__forceinline static void* syscall()
	{
		static auto proxy = allocate_proxy();
		static auto fn = extract_syscall(function);

		if (!verify_call_integrity(proxy))
			return nullptr;

		*reinterpret_cast<uint32_t*>(&proxy[1]) = fn.index;
		*reinterpret_cast<uint32_t*>(&proxy[0xD]) = fn.retn;

		return proxy;
	}
}

#define syscall(name) (reinterpret_cast<name>(memory::syscall<fnv1a(#name)>()))
#define sys(name) (reinterpret_cast<decltype(&(name))>(memory::syscall<fnv1a(#name)>()))
