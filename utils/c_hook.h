#pragma once

#include <cstdint>
#include <memory>

template<class entity>
class c_hook
{
public:
	explicit c_hook(entity* ent)
	{
		base = reinterpret_cast<uintptr_t*>(ent);
		original = *base;

		const auto l = length() + 1;
		current = std::make_unique<uint32_t[]>(l);
		std::memcpy(current.get(), reinterpret_cast<void*>(original - sizeof(uint32_t)), l * sizeof(uint32_t));
	
		patch_pointer(base);
	}
	
	template<typename function, typename original_function>
	function apply(const uint32_t index, original_function func)
	{
		auto old =  reinterpret_cast<uintptr_t*>(original)[index];
		current.get()[index + 1] = reinterpret_cast<uintptr_t>(func);
		return reinterpret_cast<function>(old);
	}

	void patch_pointer(uintptr_t* location) const
	{
		if (!location)
			return;

		DWORD old;
		PVOID address = location;
		ULONG size = sizeof(uintptr_t);
		syscall(NtProtectVirtualMemory)(current_process, &address, &size, PAGE_READWRITE, &old);
		address = location;
		size = sizeof(uintptr_t);
		*location = reinterpret_cast<uint32_t>(current.get()) + sizeof(uint32_t);
		syscall(NtProtectVirtualMemory)(current_process, &address, &size, old, &old);
	}
	
private:
	uint32_t length() const
	{
		uint32_t index;
		const auto vmt = reinterpret_cast<uint32_t*>(original);

		for (index = 0; vmt[index]; index++)
			if (IS_INTRESOURCE(vmt[index]))
				break;

		return index;
	}

	std::uintptr_t* base;
	std::uintptr_t original;
	std::unique_ptr<uint32_t[]> current;
};