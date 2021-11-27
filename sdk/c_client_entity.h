#pragma once

#include <cstdint>
#include "c_vector3d.h"
#include "matrix.h"
#include "class_id.h"
#include "recv.h"

class c_base_entity;
class c_collideable;
class c_client_networkable;
class c_client_renderable;
class c_client_entity;
class c_client_thinkable;
class c_client_unknown;
class c_client_alpha_property;
class game_trace;

struct ray;
struct model;
struct bf_read;
struct renderable_instance
{
	uint8_t alpha;
};

using c_base_handle = uint32_t;
using client_shadow_handle_t = uint32_t;
using client_render_handle_t = uint32_t;
using model_instance_handle_t = uint32_t;
using trace = game_trace;
using create_client_class_fn = c_client_networkable*(*)(int ent, int serial);
using create_event_fn = c_client_networkable*(*)();

enum solid_type
{
    solid_none = 0,
    solid_bsp = 1,
    solid_bbox = 2,
    solid_obb = 3,
    solid_obb_yaw = 4,
    solid_custom = 5,
    solid_vphysics = 6,
    solid_last
};

class i_handle_entity
{
public:
	virtual ~i_handle_entity() = default;
	virtual void set_ref_handle(const c_base_handle& handle) = 0;
	virtual const c_base_handle& get_handle() const = 0;
};

class client_class
{
public:
    create_client_class_fn create_fn;
    create_event_fn create_event_fn;
    char* network_name;
    recv_table* table;
    client_class* next;
    class_id id;
};

class c_collideable
{
protected:
	~c_collideable() = default;
public:
	virtual i_handle_entity* get_entity_handle() = 0;
	virtual c_vector3d&	obb_mins() const = 0;
	virtual c_vector3d&	obb_maxs() const = 0;
	virtual void world_space_trigger_bounds(c_vector3d* mins, c_vector3d* maxs) const = 0;
	virtual bool test_collision(const ray& ray, unsigned int mask, trace& trace) = 0;
	virtual bool test_hitboxes(const ray& ray, unsigned int mask, trace& trace) = 0;
	virtual int get_collision_model_index() = 0;
	virtual const model* get_collision_model() = 0;
	virtual c_vector3d&	get_collision_origin() const = 0;
	virtual c_qangle& get_collision_angles() const = 0;
	virtual const matrix3x4& collision_to_world_transform() const = 0;
	virtual solid_type get_solid() const = 0;
	virtual int get_solid_flags() const = 0;
	virtual c_client_unknown* get_client_unknown() = 0;
	virtual int get_collision_group() const = 0;
	virtual void world_space_surrounding_bounds(c_vector3d* mins, c_vector3d* maxs) = 0;
	virtual bool should_touch_trigger(int flags) const = 0;
	virtual const matrix3x4* get_root_parent_to_world_transform() const = 0;
};

class c_client_unknown : public i_handle_entity
{
public:
	virtual c_collideable* get_collideable() = 0;
	virtual c_client_networkable* get_client_networkable() = 0;
	virtual c_client_renderable* get_client_renderable() = 0;
	virtual c_client_entity* get_client_entity() = 0;
	virtual c_base_entity* get_base_entity() = 0;
	virtual c_client_thinkable*	get_client_thinkable() = 0;
	virtual c_client_alpha_property* get_client_alpha_property() = 0;
};

class c_client_thinkable
{
public:
	virtual ~c_client_thinkable() = default;
};

class c_client_renderable
{
protected:
	~c_client_renderable() = default;
public:
	virtual c_client_unknown* get_client_unknown() = 0;
	virtual c_vector3d const& get_render_origin() = 0;
	virtual c_qangle const& get_render_angles() = 0;
	virtual bool should_draw() = 0;
	virtual int get_render_flags() = 0;
	virtual void unused() const = 0;
	virtual client_shadow_handle_t get_shadow_handle() const = 0;
	virtual client_render_handle_t& render_handle() = 0;
	virtual const model* get_model() const = 0;
	virtual int draw_model(int flags, uint8_t alpha) = 0;
	virtual int get_body() = 0;
	virtual void get_color_modulation(float* color) = 0;
	virtual bool lod_test() = 0;
	virtual bool setup_bones(matrix3x4* bones, int max, int mask, float time) = 0;
	virtual void setup_weights(const matrix3x4* bones, int count, float* weights, float* delayed_weights) = 0;
	virtual void do_animation_events() = 0;
	virtual void* get_pvs_notify_interface() = 0;
	virtual void get_render_bounds(c_vector3d& mins, c_vector3d& maxs) = 0;
	virtual void get_render_bounds_worldspace(c_vector3d& mins, c_vector3d& maxs) = 0;
	virtual void get_shadow_render_bounds(c_vector3d &mins, c_vector3d &maxs, int type) = 0;
	virtual bool should_receive_projected_textures(int flags) = 0;
	virtual bool get_shadow_cast_distance(float* dist, int type) const = 0;
	virtual bool get_shadow_cast_direction(c_vector3d* direction, int type) const = 0;
	virtual bool is_shadow_dirty() = 0;
	virtual void mark_shadow_dirty(bool dirty) = 0;
	virtual c_client_renderable* get_shadow_parent() = 0;
	virtual c_client_renderable* first_shadow_child() = 0;
	virtual c_client_renderable* next_shadow_peer() = 0;
	virtual int shadow_cast_type() = 0;
	virtual void create_model_instance() = 0;
	virtual model_instance_handle_t get_model_instance() = 0;
	virtual const matrix3x4& renderable_to_world_transform() = 0;
	virtual int lookup_attachment(const char* name) = 0;
	virtual bool get_attachment(int index, c_vector3d& origin, c_vector3d& angles) = 0;
	virtual bool get_attachment(int index, matrix3x4& matrix) = 0;
	virtual float* get_render_clip_plane() = 0;
	virtual int get_skin() = 0;
	virtual void on_threaded_draw_setup() = 0;
	virtual bool uses_flex_delayed_weights() = 0;
	virtual void record_tool_message() = 0;
	virtual bool should_draw_for_split_screen_user(int slot) = 0;
	virtual uint8_t override_alpha_modulation(uint8_t alpha) = 0;
	virtual uint8_t override_shadow_alpha_modulation(uint8_t alpha) = 0;
};

class c_client_networkable
{
protected:
	~c_client_networkable() = default;
public:
	virtual c_client_unknown* get_client_unknown() = 0;
	virtual void release() = 0;
	virtual client_class* get_client_class() = 0;
	virtual void notify_should_transmit(int state) = 0;
	virtual void on_pre_data_changed(int type) = 0;
	virtual void on_data_changed(int type) = 0;
	virtual void pre_data_update(int type) = 0;
	virtual void post_data_update(int type) = 0;
	virtual void on_data_unchanged_in_pvs() = 0;
	virtual bool is_dormant_virt() = 0;
	virtual int	index() const = 0;
	virtual void receive_message(int class_id, bf_read& msg) = 0;
	virtual void* get_data_table() = 0;
	virtual void set_destroyed_on_recreate_entities() = 0;

	class_id get_class_id()
	{
		return get_client_class()->id;
	}

	bool is_player()
	{
		return get_class_id() == ccsplayer;
	}
};

class c_client_entity : public c_client_unknown, public c_client_renderable, public c_client_networkable, public c_client_thinkable
{
public:
	virtual ~c_client_entity() {};
};
