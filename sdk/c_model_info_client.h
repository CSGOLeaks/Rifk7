#pragma once

#include "c_vector3d.h"
#include "matrix.h"
#include "macros.h"
#include "c_model_render.h"
#include "c_utlvector.h"

struct studiohdr;

enum bone
{
	bone_used_mask = 0x0007FF00,
	bone_used_by_anything = 0x0007FF00,
	bone_used_by_hitbox = 0x00000100,
	bone_used_by_attachment = 0x00000200,
	bone_used_by_vertex_mask = 0x0003FC00,
	bone_used_by_vertex_lod0 = 0x00000400,
	bone_used_by_vertex_lod1 = 0x00000800,
	bone_used_by_vertex_lod2 = 0x00001000,
	bone_used_by_vertex_lod3 = 0x00002000,
	bone_used_by_vertex_lod4 = 0x00004000,
	bone_used_by_vertex_lod5 = 0x00008000,
	bone_used_by_vertex_lod6 = 0x00010000,
	bone_used_by_vertex_lod7 = 0x00020000,
	bone_used_by_bone_merge = 0x00040000
};

static constexpr auto bone_used_by_server = bone_used_by_hitbox | bone_used_by_vertex_lod0 | bone_used_by_vertex_lod1 | bone_used_by_vertex_lod2
	| bone_used_by_vertex_lod3 | bone_used_by_vertex_lod4 | bone_used_by_vertex_lod5 | bone_used_by_vertex_lod6 | bone_used_by_vertex_lod7;

static constexpr auto bone_always_procedural = 4;

class quaternion {
public:
	quaternion() : x(0), y(0), z(0), w(0) { }
	quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) { }

	void init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }

	float* base() { return reinterpret_cast<float*>(this); }
	const float* base() const { return reinterpret_cast<const float*>(this); }

	float x, y, z, w;
};

struct mstudiobone_t {
	int sznameindex;

	bool has_parent(const studiohdr* hdr, const int i) const;

	char* get_name() const {
		return reinterpret_cast<char*>(const_cast<mstudiobone_t*>(this)) + sznameindex;
	}

	int parent;
	int bonecontroller[6];

	c_vector3d pos;
	quaternion quat;
	c_qangle rot;
	c_vector3d posscale;
	c_vector3d rotscale;

	matrix3x4 pose_to_bone;
	quaternion q_alignment;
	int flags;
	int proctype;
	int procindex;
	mutable int physicsbone;

	int surfacepropidx;

	char* get_surface_id() const {
		return reinterpret_cast<char*>(const_cast<mstudiobone_t*>(this)) + surfacepropidx;
	}

	int get_surface_prop() const {
		return surfacepropLookup;
	}

	int contents;
	int surfacepropLookup;
	int unused[7];
};

struct mstudiobbox_t {
	int bone;
	int group;
	c_vector3d bbmin;
	c_vector3d bbmax;
	int hitboxnameindex;
	c_vector3d rotation;
	float radius;
	int pad2[4];

	char* get_hitbox_name() {
		if (hitboxnameindex == 0)
			return nullptr;

		return reinterpret_cast<char*>(const_cast<mstudiobbox_t*>(this)) + hitboxnameindex;
	}
};

struct mstudiohitboxset_t {
	int sznameindex;

	char* get_name() const {
		return reinterpret_cast<char*>(const_cast<mstudiohitboxset_t*>(this)) + sznameindex;
	}

	int numhitboxes;
	int hitboxindex;

	__forceinline mstudiobbox_t* get_hitbox(int i) const {
		const auto set = reinterpret_cast<unsigned char*>(const_cast<mstudiohitboxset_t*>(this));
		return reinterpret_cast<mstudiobbox_t*>(set + hitboxindex) + i;
	}
};

struct mstudioattachment_t {
	int	sznameindex;

	char* get_name() const {
		return reinterpret_cast<char*>(const_cast<mstudioattachment_t*>(this)) + sznameindex;
	}

	unsigned int flags;
	int localbone;
	float local[12];
	int unused[8];
};

struct studiohdr {
	int id;
	int version;
	long checksum;
	char name[64];
	int length;

	c_vector3d eyeposition;
	c_vector3d illumposition;
	c_vector3d hull_min;
	c_vector3d hull_max;
	c_vector3d view_bbmin;
	c_vector3d view_bbmax;

	int flags;
	int numbones;
	int boneindex;

	mstudiobone_t* get_bone(int i) const {
		if (!(i >= 0 && i < numbones))
			return nullptr;

		const auto hdr = reinterpret_cast<unsigned char*>(const_cast<studiohdr*>(this));
		return reinterpret_cast<mstudiobone_t*>(hdr + boneindex) + i;
	}

	mstudioattachment_t* get_local_attachment(int i) const
	{
		const auto hdr = reinterpret_cast<unsigned char*>(const_cast<studiohdr*>(this));
		return reinterpret_cast<mstudioattachment_t*>(hdr + localattachmentindex) + i;
	}

	int numbonecontrollers;
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;

	__forceinline mstudiohitboxset_t* get_hitbox_set(int i) const {
		const auto hdr = reinterpret_cast<unsigned char*>(const_cast<studiohdr*>(this));
		return reinterpret_cast<mstudiohitboxset_t *>(hdr + hitboxsetindex) + i;
	}

	__forceinline mstudiobbox_t* get_hitbox(int i, int set) const {
		mstudiohitboxset_t const *s = get_hitbox_set(set);
		if (!s)
			return nullptr;

		return s->get_hitbox(i);
	}

	int get_hitbox_count(int set) const {
		mstudiohitboxset_t const* s = get_hitbox_set(set);
		if (!s)
			return 0;

		return s->numhitboxes;
	}

	int numlocalanim;
	int localanimindex;
	int numlocalseq;
	int localseqindex;

	mutable int activitylistversion;
	mutable int eventsindexed;

	int numtextures;
	int textureindex;
	int numcdtextures;
	int cdtextureindex;

	int numskinref;
	int numskinfamilies;
	int skinindex;

	short* get_skin_ref(int i) const {
		const auto hdr = reinterpret_cast<unsigned char*>(const_cast<studiohdr*>(this));
		return reinterpret_cast<short*>(hdr + skinindex) + i;
	}

	int numbodyparts;
	int bodypartindex;
	int numlocalattachments;
	int localattachmentindex;

	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;

	char* get_local_node_name(int iNode) const {
		const auto hdr = reinterpret_cast<unsigned char*>(const_cast<studiohdr*>(this));
		return reinterpret_cast<char*>(hdr) + *(reinterpret_cast<int *>(hdr + localnodenameindex) + iNode);
	}

	unsigned char* get_local_transition(int i) const {
		const auto hdr = reinterpret_cast<unsigned char*>(const_cast<studiohdr*>(this));
		return static_cast<unsigned char *>(hdr + localnodeindex) + i;
	}

	int numflexdesc;
	int flexdescindex;
	int numflexcontrollers;
	int flexcontrollerindex;
	int numflexrules;
	int flexruleindex;
	int numikchains;
	int ikchainindex;
	int nummouths;
	int mouthindex;
	int numlocalposeparameters;
	int localposeparamindex;
	int surfacepropindex;

	char* get_surface_prop() const {
		return reinterpret_cast<char*>(const_cast<studiohdr*>(this)) + surfacepropindex;
	}

	int keyvalueindex;
	int keyvaluesize;

	const char* get_key_value_text() const {
		return keyvaluesize != 0 ? reinterpret_cast<char*>(const_cast<studiohdr*>(this)) + keyvalueindex : nullptr;
	}

	int numlocalikautoplaylocks;
	int localikautoplaylockindex;

	float mass;
	int contents;
	int numincludemodels;
	int includemodelindex;

	mutable void* virtual_model;
	int szanimblocknameindex;

	char* get_anim_block_name() const {
		return reinterpret_cast<char *>(const_cast<studiohdr*>(this)) + szanimblocknameindex;
	}

	int numanimblocks;
	int animblockindex;
	mutable void* animblock_model;
	int bonetablebynameindex;

	const unsigned char* get_bone_table_sorted_by_name() const {
		return reinterpret_cast<unsigned char *>(const_cast<studiohdr*>(this)) + bonetablebynameindex;
	}

	void* vertex_base;
	void* index_base;
	unsigned char constdirectionallightdot;
	unsigned char root_lod;
	unsigned char num_allowed_root_lods;
	unsigned char unused[1];
	int unused4;
	int numflexcontrollerui;
	int flexcontrolleruiindex;
	int unused3[2];
	int studiohdr2index;
	int unused2[1];

	// ReSharper disable once CppPossiblyUninitializedMember
	studiohdr() { }
private:
	studiohdr(const studiohdr& other);
};

class virtualmodel_t;

class c_studio_hdr
{
public:
	mutable studiohdr* m_pStudioHdr; //0x0000 
	mutable virtualmodel_t* m_pVModel; //0x0004 
	char				pad_unknown[ 0x4 ]; //0x0008 
	mutable CUtlVector<const studiohdr*> m_pStudioHdrCache; //0x000C 
	mutable int			m_nFrameUnlockCounter; //0x0020 
	int*				m_pFrameUnlockCounter; //0x0024 
	char				pad_mutex[ 0x8 ]; //0x0028 
	CUtlVector<int>	bone_flags; //0x0030
	CUtlVector<int>	m_boneParent; //0x0050
};

inline bool mstudiobone_t::has_parent(const studiohdr* hdr, const int i) const
{
	if (parent == -1)
		return false;

	if (parent == i)
		return true;

	return hdr->get_bone(parent)->has_parent(hdr, i);
}

struct draw_model_state
{
    studiohdr* hdr;
    void* hwdata;
    c_client_renderable* renderable;
    const matrix3x4* model_to_world;
    int32_t decals;
    int32_t draw_flags;
    int32_t lod;
};

class c_model_info_client {
public:
	virtual	~c_model_info_client() = default;
	virtual const model_t* get_model(int modelindex) const = 0;
	virtual int	get_model_index(const char *name) const = 0;
	virtual const char* get_model_name(const model_t *model) const = 0;

	vfunc(19, get_model_materials(const model* mod, const int count, c_material** out),
		void(__thiscall*)(c_model_info_client*, const model*, int, c_material**))(mod, count, out)

	vfunc(32, get_studio_model(const model* mod), studiohdr*(__thiscall*)(c_model_info_client*, const model*))(mod)
};

interface_var(c_model_info_client, model_info_client, "engine.dll", "VModelInfoClient")
