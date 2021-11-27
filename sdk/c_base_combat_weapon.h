#pragma once

#include "c_base_attributable_item.h"

enum weapon_type
{
	weapontype_knife = 1,
	weapontype_pistol,
	weapontype_submachinegun,
	weapontype_rifle,
	weapontype_shotgun,
	weapontype_sniper_rifle,
	weapontype_machinegun,
	weapontype_c4,
	weapontype_grenade,
	weapontype_unknown
};

class c_base_combat_weapon : public c_base_attributable_item
{
public:
	class strike_weapon_definition
	{
	public:
	    char pad_0x0000[0x8]; //0x0000
	    int m_nWeaponID; //0x0008
	}; 

	class weapon_data
	{
	public:

		int get_weapon_id() const
		{
			if (!m_pWeaponDef)
				return -1;

			return m_pWeaponDef->m_nWeaponID;
		}

		float get_standing_accuracy(c_base_combat_weapon* weapon) const
		{
			const auto max_speed = weapon->get_zoom_level() > 0 ? flMaxPlayerSpeedAlt : flMaxPlayerSpeed;
			return max_speed / 3.f;
		}

		virtual ~weapon_data() = default;

		char*		consoleName;				// 0x0004
		char		pad_0008[8];				// 0x0008
		strike_weapon_definition* m_pWeaponDef;  //0x0010
		int			iMaxClip1;					// 0x0014
		int			iMaxClip2;					// 0x0018
		int			iDefaultClip1;				// 0x001C
		int			iDefaultClip2;				// 0x0020
		char		pad_0024[8];				// 0x0024
		char*		szWorldModel;				// 0x002C
		char*		szViewModel;				// 0x0030
		char*		szDroppedModel;				// 0x0034
		char		pad_0038[4];				// 0x0038
		char*		N0000023E;					// 0x003C
		char		pad_0040[56];				// 0x0040
		char*		szEmptySound;				// 0x0078
		char		pad_007C[4];				// 0x007C
		char*		szBulletType;				// 0x0080
		char		pad_0084[4];				// 0x0084
		char*		szHudName;					// 0x0088
		char*		szWeaponName;				// 0x008C
		char		pad_0090[60];				// 0x0090
		int 		WeaponType;					// 0x00C8
		int			iWeaponPrice;				// 0x00CC
		int			iKillAward;					// 0x00D0
		char*		szAnimationPrefix;			// 0x00D4
		float		flCycleTime;				// 0x00D8
		float		flCycleTimeAlt;				// 0x00DC
		float		flTimeToIdle;				// 0x00E0
		float		flIdleInterval;				// 0x00E4
		bool		bFullAuto;					// 0x00E8
		char		pad_0x00E5[3];			// 0x00E9
		int			iDamage;					// 0x00EC
		float		flArmorRatio;				// 0x00F0
		int			iBullets;					// 0x00F4
		float		flPenetration;				// 0x00F8
		float		flFlinchVelocityModifierLarge;	// 0x00FC
		float		flFlinchVelocityModifierSmall;	// 0x0100
		float		flRange;					// 0x0104
		float		flRangeModifier;			// 0x0108
		float		flThrowVelocity;			// 0x010C
		char		pad_0x010C[16];			// 0x0110
		bool		bHasSilencer;				// 0x011C
		char		pad_0x0119[3];			// 0x011D
		char*		pSilencerModel;				// 0x0120
		int			iCrosshairMinDistance;		// 0x0124
		float		flMaxPlayerSpeed;	        // 0x0128
		float		flMaxPlayerSpeedAlt;		// 0x012C
		char		pad_0x0130[4];		    // 0x0130
		float		flSpread;					// 0x0134
		float		flSpreadAlt;				// 0x0138
		float		flInaccuracyCrouch;			// 0x013C
		float		flInaccuracyCrouchAlt;		// 0x0140
		float		flInaccuracyStand;			// 0x0144
		float		flInaccuracyStandAlt;		// 0x0148
		float		flInaccuracyJumpInitial;	// 0x014C
		float		flInaccuracyJump;			// 0x0150
		float		flInaccuracyJumpAlt;		// 0x0154
		float		flInaccuracyLand;			// 0x0158
		float		flInaccuracyLandAlt;		// 0x015C
		float		flInaccuracyLadder;			// 0x0160
		float		flInaccuracyLadderAlt;		// 0x0164
		float		flInaccuracyFire;			// 0x0168
		float		flInaccuracyFireAlt;		// 0x016C
		float		flInaccuracyMove;			// 0x0170
		float		flInaccuracyMoveAlt;		// 0x0174
		float		flInaccuracyReload;			// 0x0178
		int			iRecoilSeed;				// 0x017C
		float		flRecoilAngle;				// 0x0180
		float		flRecoilAngleAlt;			// 0x0184
		float		flRecoilAngleVariance;		// 0x0188
		float		flRecoilAngleVarianceAlt;	// 0x018C
		float		flRecoilMagnitude;			// 0x0190
		float		flRecoilMagnitudeAlt;		// 0x0194
		float		flRecoilMagnitudeVariance;	// 0x0198
		float		flRecoilMagnitudeVarianceAlt;	// 0x019C
		float		flRecoveryTimeCrouch;		// 0x01A0
		float		flRecoveryTimeStand;		// 0x01A4
		float		flRecoveryTimeCrouchFinal;	// 0x01A8
		float		flRecoveryTimeStandFinal;	// 0x01AC
		int			iRecoveryTransitionStartBullet;	// 0x01B0 
		int			iRecoveryTransitionEndBullet;	// 0x01B4
		bool		bUnzoomAfterShot;			// 0x01B8
		bool		bHideViewModelZoomed;		// 0x01B9
		char		pad_0x01B5[2];			// 0x01BA
		char		iZoomLevels[3];			// 0x01BC
		int			iZoomFOV[2];				// 0x01C0
		float		fZoomTime[3];				// 0x01C4
		char*		szWeaponClass;				// 0x01D4
		float		flAddonScale;				// 0x01D8
		char		pad_0x01DC[4];			// 0x01DC
		char*		szEjectBrassEffect;			// 0x01E0
		char*		szTracerEffect;				// 0x01E4
		int			iTracerFrequency;			// 0x01E8
		int			iTracerFrequencyAlt;		// 0x01EC
		char*		szMuzzleFlashEffect_1stPerson;	// 0x01F0
		char		pad_0x01F4[4];				// 0x01F4
		char*		szMuzzleFlashEffect_3rdPerson;	// 0x01F8
		char		pad_0x01FC[4];			// 0x01FC
		char*		szMuzzleSmokeEffect;		// 0x0200
		float		flHeatPerShot;				// 0x0204
		char*		szZoomInSound;				// 0x0208
		char*		szZoomOutSound;				// 0x020C
		float		flInaccuracyPitchShift;		// 0x0210
		float		flInaccuracySoundThreshold;	// 0x0214
		float		flBotAudibleRange;			// 0x0218
		char		pad_0x0218[8];			// 0x0220
		char*		pWrongTeamMsg;				// 0x0224
		bool		bHasBurstMode;				// 0x0228
		char		pad_0x0225[3];			// 0x0229
		bool		bIsRevolver;				// 0x022C
		bool		bCannotShootUnderwater;		// 0x0230
	};

	bool is_grenade() { return get_item_definition() >= weapon_flashbang && get_item_definition() <= weapon_incgrenade; }

	vfunc(471, get_inaccuracy(), float(__thiscall*)(void*))()
	vfunc(472, update_accuracy(), void(__thiscall*)(void*))()

	netvar(get_current_clip(), int32_t, "CBaseCombatWeapon", "m_iClip1")
	netvar(get_next_primary_attack(), float, "CBaseCombatWeapon", "m_flNextPrimaryAttack")
	netvar(get_next_secondary_attack(), int32_t, "CBaseCombatWeapon", "m_flNextSecondaryAttack")
	netvar(get_postpone_fire_ready_time(), int32_t, "CWeaponCSBase", "m_flPostponeFireReadyTime")
	netvar(get_zoom_level(), int32_t, "CWeaponCSBaseGun", "m_zoomLevel")
	netvar(get_accuracy_penalty(), float, "CWeaponCSBase", "m_fAccuracyPenalty")
	netvar(get_recoil_index(), float, "CWeaponCSBase", "m_flRecoilIndex")
	netvar(get_last_shot_time(), float, "CWeaponCSBase", "m_fLastShotTime")
	netvar(get_pin_pulled(), bool, "CBaseCSGrenade", "m_bPinPulled")
	netvar(get_throw_time(), float, "CBaseCSGrenade", "m_fThrowTime")
	offset(get_in_reload(), bool, 0x3275)
};
