#pragma once
#include "c_vector3d.h"
#include "c_user_cmd.h"
#include "../utils/c_memory.h"

class c_input
{
public:
	static c_input* get()
	{
		static const auto input = *reinterpret_cast<c_input**>(reinterpret_cast<uint32_t>
			(sig("client_panorama.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 1);
		return input;
	}

	void* pvftable{};
    bool m_fTrackIRAvailable{};
    bool m_fMouseInitialized{};
    bool m_fMouseActive{};
    bool m_fJoystickAdvancedInit{};
    char pad_0x08[0x2C]{};
    void* m_pKeys{};
    char pad_0x38[0x64]{};
    int pad_0x41{};
    int pad_0x42{};
	int pad_0x43{};
	int pad_0x44{};
    bool m_fCameraInterceptingMouse{};
    bool camera_in_third_person{};
    bool m_fCameraMovingWithMouse{};
    c_vector3d m_vecCameraOffset;
    bool m_fCameraDistanceMove{};
    int m_nCameraOldX{};
    int m_nCameraOldY{};
    int m_nCameraX{};
    int m_nCameraY{};
    bool m_CameraIsOrthographic{};
    c_vector3d m_angPreviousViewAngles;
    c_vector3d m_angPreviousViewAnglesTilt;
    float m_flLastForwardMove{};
    int m_nClearInputState{};
    c_user_cmd* commands{};
    c_verified_user_cmd* verified_commands{};
	c_user_cmd * GetUserCmd(int nSlot, int sequence_number);
};

#define input c_input::get()
