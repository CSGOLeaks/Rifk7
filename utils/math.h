#pragma once

#include "../sdk/c_vector3d.h"
#include "../sdk/matrix.h"
#include "c_memory.h"

constexpr auto rad_pi = 57.295779513082f;

#define rad2deg(x) ((float)(x) * (float)(180.f / pi))
#define deg2rad(x) ((float)(x) * (float)(pi / 180.f))
#define rnd_int(min, max) (rand() % ((max) - (min) + 1) + (min))
#define rnd_flt(min, max) ((min) + 1) + (((float) rand()) / (float) RAND_MAX) * ((max) - ((min) + 1));
#define time_to_ticks(dt) ((int)(0.5f + (float)(dt) / global_vars_base->interval_per_tick))
#define ticks_to_time(t) (global_vars_base->interval_per_tick * (t))

class math
{
public:
	static constexpr auto pitch_bounds = 178.f;
	static constexpr auto yaw_bounds = 360.f;
	static constexpr auto roll_bounds = 100.f;

	static constexpr auto forward_bounds = 450.f;
	static constexpr auto side_bounds = 450.f;
	static constexpr auto up_bounds = 320.f;

	__forceinline static void ensure_bounds(c_qangle& angle, c_vector3d& move)
	{
		math::normalize(angle);
		move.x = std::clamp(move.x, -forward_bounds, forward_bounds);
		move.y = std::clamp(move.y, -side_bounds, side_bounds);
		move.z = std::clamp(move.z, -up_bounds, up_bounds);
	}

	__forceinline static c_qangle calc_angle(const c_vector3d from, const c_vector3d to) {
		static const auto ang_zero = c_qangle(0.0f, 0.0f, 0.0f);

		const auto delta = from - to;
		if (delta.length() <= 0.0f)
			return ang_zero;

		if (delta.z == 0.0f && delta.length() == 0.0f)
			return ang_zero;

		if (delta.y == 0.0f && delta.x == 0.0f)
			return ang_zero;

		c_qangle angles;
		angles.x = asinf(delta.z / delta.length()) * rad_pi;
		angles.y = atanf(delta.y / delta.x) * rad_pi;
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		normalize(angles);
		return angles;
	}

	__forceinline static void vector_transform(const c_vector3d& in1, const matrix3x4& in2, c_vector3d& out)
	{
		out.x = in1.dot(in2[0]) + in2[0][3];
		out.y = in1.dot(in2[1]) + in2[1][3];
		out.z = in1.dot(in2[2]) + in2[2][3];
	}

	__forceinline static void vector_i_transform(const c_vector3d& in1, const matrix3x4& in2, c_vector3d& out)
	{
		out.x = (in1.x - in2[0][3]) * in2[0][0] + (in1.y - in2[1][3]) * in2[1][0] + (in1.z - in2[2][3]) * in2[2][0];
		out.y = (in1.x - in2[0][3]) * in2[0][1] + (in1.y - in2[1][3]) * in2[1][1] + (in1.z - in2[2][3]) * in2[2][1];
		out.z = (in1.x - in2[0][3]) * in2[0][2] + (in1.y - in2[1][3]) * in2[1][2] + (in1.z - in2[2][3]) * in2[2][2];
	}

	__forceinline static c_vector3d vector_rotate(const c_vector3d& in1, const matrix3x4& in2)
	{
		return c_vector3d(in1.dot(in2[0]), in1.dot(in2[1]), in1.dot(in2[2]));
	}

	__forceinline static c_vector3d vector_rotate(const c_vector3d& in1, const c_qangle& in2)
	{
		const auto matrix = angle_matrix(in2);
		return vector_rotate(in1, matrix);
	}

	__forceinline static void vector_i_rotate(const c_vector3d& in1, const matrix3x4& in2, c_vector3d& out)
	{
		out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
		out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
		out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
	}

	__forceinline static void vector_angles(const c_vector3d& in, c_vector3d& out)
	{
		if (in.z == 0.f && in.x == 0.f)
		{
			out.y = 0.f;

			if (in.z > 0.f)
				out.x = 90.f;
			else
				out.x = 270.f;
		}
		else
		{
			out.x = rad2deg(atan2f(-in.z, in.length2d()));
			out.y = rad2deg(atan2f(in.y, in.x));

			if (out.x < 0.0f)
				out.x += 360.0f;

			if (out.y < 0.0f)
				out.y += 360.0f;
		}

		out.x -= floorf(out.x / 360.0f + 0.5f) * 360.0f;
		out.y -= floorf(out.y / 360.0f + 0.5f) * 360.0f;

		if (out.x > 89.0f)
			out.x = 89.0f;
		else if (out.x < -89.0f)
			out.x = -89.0f;
	}

	__forceinline static void angle_vectors(const c_qangle& angles, c_vector3d& forward) {
		const auto sp = sin(deg2rad(angles.x)), cp = cos(deg2rad(angles.x)),
			sy = sin(deg2rad(angles.y)), cy = cos(deg2rad(angles.y));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	__forceinline static void angle_vectors(const c_qangle& angles, c_vector3d& forward, c_vector3d& right, c_vector3d& up)
	{
		const auto sp = sin(deg2rad(angles.x)), cp = cos(deg2rad(angles.x)),
			sy = sin(deg2rad(angles.y)), cy = cos(deg2rad(angles.y)),
			sr = sin(deg2rad(angles.z)), cr = cos(deg2rad(angles.z));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;

		right.x = -1 * sr * sp * cy + -1 * cr * -sy;
		right.y = -1 * sr * sp * sy + -1 * cr * cy;
		right.z = -1 * sr * cp;

		up.x = cr * sp * cy + -sr * -sy;
		up.y = cr * sp * sy + -sr * cy;
		up.z = cr * cp;
	}

	__forceinline static matrix3x4 rotation_matrix(const c_vector3d& direction, const c_vector3d& up = c_vector3d(0, 1, 0))
	{
		matrix3x4 out{};

		const auto xaxis = up.cross(direction).normalize();
		const auto yaxis = direction.cross(xaxis).normalize();

		out[0][0] = xaxis.x;
		out[0][1] = yaxis.x;
		out[0][2] = direction.x;

		out[1][0] = xaxis.y;
		out[1][1] = yaxis.y;
		out[1][2] = direction.y;

		out[2][0] = xaxis.z;
		out[2][1] = yaxis.z;
		out[2][2] = direction.z;

		return out;
	}

	__forceinline static matrix3x4 angle_matrix(const c_qangle angles)
	{
		matrix3x4 result{};

		m128 angle, sin, cos;
		angle.f[0] = deg2rad(angles.x);
		angle.f[1] = deg2rad(angles.y);
		angle.f[2] = deg2rad(angles.z);
		sincos_ps(angle.v, &sin.v, &cos.v);

		result[0][0] = cos.f[0] * cos.f[1];
		result[1][0] = cos.f[0] * sin.f[1];
		result[2][0] = -sin.f[0];

		const auto crcy = cos.f[2] * cos.f[1];
		const auto crsy = cos.f[2] * sin.f[1];
		const auto srcy = sin.f[2] * cos.f[1];
		const auto srsy = sin.f[2] * sin.f[1];

		result[0][1] = sin.f[0] * srcy - crsy;
		result[1][1] = sin.f[0] * srsy + crcy;
		result[2][1] = sin.f[2] * cos.f[0];

		result[0][2] = sin.f[0] * crcy + srsy;
		result[1][2] = sin.f[0] * crsy - srcy;
		result[2][2] = cos.f[2] * cos.f[0];

		return result;
	}

	__forceinline static matrix3x4 angle_matrix(const c_qangle angle, const c_vector3d pos)
	{
		auto result = angle_matrix(angle);
		result[0][3] = pos.x;
		result[1][3] = pos.y;
		result[2][3] = pos.z;
		return result;
	}

	__forceinline static c_vector3d matrix_position(matrix3x4& matrix)
	{
		return c_vector3d(matrix[0][3], matrix[1][3], matrix[2][3]);
	}

	__forceinline static c_qangle matrix_angles(matrix3x4& matrix)
	{
		float forward[3];
		float left[3];
		float up[3];

		forward[0] = matrix[0][0];
		forward[1] = matrix[1][0];
		forward[2] = matrix[2][0];

		left[0] = matrix[0][1];
		left[1] = matrix[1][1];
		left[2] = matrix[2][1];

		up[2] = matrix[2][2];

		m128 a1;
		a1.f[0] = forward[0] * forward[0] + forward[1] * forward[1];
		auto calc = sqrt_ps(a1.v);
		const auto xy_dist = reinterpret_cast<const m128*>(&calc)->f[0];

		if (xy_dist > 0.001f)
			return c_qangle(rad2deg(atan2f(-forward[2], xy_dist)),
				rad2deg(atan2f(forward[1], forward[0])),
				rad2deg(atan2f(left[2], up[2])));

		return c_qangle(rad2deg(atan2f(-forward[2], xy_dist)),
			rad2deg(atan2f(-left[0], left[1])),
			0);
	}

	__forceinline static matrix3x4 multiply_matrix(matrix3x4 in1, matrix3x4 in2)
	{
		matrix3x4 result{};
		result[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
			in1[0][2] * in2[2][0];
		result[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
			in1[0][2] * in2[2][1];
		result[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
			in1[0][2] * in2[2][2];
		result[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
			in1[0][2] * in2[2][3] + in1[0][3];
		result[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
			in1[1][2] * in2[2][0];
		result[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
			in1[1][2] * in2[2][1];
		result[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
			in1[1][2] * in2[2][2];
		result[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
			in1[1][2] * in2[2][3] + in1[1][3];
		result[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
			in1[2][2] * in2[2][0];
		result[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
			in1[2][2] * in2[2][1];
		result[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
			in1[2][2] * in2[2][2];
		result[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
			in1[2][2] * in2[2][3] + in1[2][3];
		return result;
	}

	__forceinline static float interpolate(const float from, const float to, const float percent)
	{
		return to * percent + from * (1.f - percent);
	}

	__forceinline static c_vector3d interpolate(const c_vector3d from, const c_vector3d to, const float percent)
	{
		return to * percent + from * (1.f - percent);
	}

	__forceinline static void normalize(c_qangle& angle)
	{
		if (!angle.is_valid())
		{
			angle = { 0.f, 0.f, 0.f };
			return;
		}

		angle.x = std::remainderf(angle.x, pitch_bounds);
		angle.y = std::remainderf(angle.y, yaw_bounds);
		angle.z = std::remainderf(angle.z, roll_bounds);
	}

	__forceinline static float normalize_yaw(float angle)
	{
		if (!std::isfinite(angle))
			angle = 0.f;

		return std::remainderf(angle, yaw_bounds);
	}

	__forceinline static float get_fov(c_qangle& current, c_vector3d& start, c_vector3d& end) {
		c_vector3d currentAim, targetAim = (end - start).normalize();
		angle_vectors(current, currentAim);
		return rad2deg(acos(targetAim.dot(currentAim)));
	}

	__forceinline static float random_float(float min, float max) {
		static auto fn = reinterpret_cast<float(__cdecl*)(float, float)>(
			memory::get_proc_address(memory::get_module_handle(fnv1a("vstdlib.dll")), fnv1a("RandomFloat")));
		return fn(min, max);
	}

	__forceinline static void random_seed(uint32_t seed) {
		static auto fn = reinterpret_cast<void(__cdecl*)(uint32_t)>(
			memory::get_proc_address(memory::get_module_handle(fnv1a("vstdlib.dll")), fnv1a("RandomSeed")));
		fn(seed);
	}

	static float segment_to_segment(const c_vector3d s1, const c_vector3d s2, const c_vector3d k1, const c_vector3d k2)
	{
		static auto constexpr epsilon = 0.00000001;

		auto u = s2 - s1;
		auto v = k2 - k1;
		const auto w = s1 - k1;

		const auto a = u.dot(u);
		const auto b = u.dot(v);
		const auto c = v.dot(v);
		const auto d = u.dot(w);
		const auto e = v.dot(w);
		const auto D = a * c - b * b;
		float sn, sd = D;
		float tn, td = D;

		if (D < epsilon) {
			sn = 0.0;
			sd = 1.0;
			tn = e;
			td = c;
		}
		else {
			sn = b * e - c * d;
			tn = a * e - b * d;

			if (sn < 0.0) {
				sn = 0.0;
				tn = e;
				td = c;
			}
			else if (sn > sd) {
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if (tn < 0.0) {
			tn = 0.0;

			if (-d < 0.0)
				sn = 0.0;
			else if (-d > a)
				sn = sd;
			else {
				sn = -d;
				sd = a;
			}
		}
		else if (tn > td) {
			tn = td;

			if (-d + b < 0.0)
				sn = 0;
			else if (-d + b > a)
				sn = sd;
			else {
				sn = -d + b;
				sd = a;
			}
		}

		const float sc = abs(sn) < epsilon ? 0.0 : sn / sd;
		const float tc = abs(tn) < epsilon ? 0.0 : tn / td;

		m128 n;
		auto dp = w + u * sc - v * tc;
		n.f[0] = dp.dot(dp);
		const auto calc = sqrt_ps(n.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}

	__forceinline static bool intersect_line_with_bb(c_vector3d& start, c_vector3d& end, c_vector3d& min, c_vector3d& max) {
		float d1, d2, f;
		auto start_solid = true;
		auto t1 = -1.0f, t2 = 1.0f;

		const float s[3] = { start.x, start.y, start.z };
		const float e[3] = { end.x, end.y, end.z };
		const float mi[3] = { min.x, min.y, min.z };
		const float ma[3] = { max.x, max.y, max.z };

		for (auto i = 0; i < 6; i++) {
			if (i >= 3) {
				const auto j = i - 3;

				d1 = s[j] - ma[j];
				d2 = d1 + e[j];
			}
			else {
				d1 = -s[i] + mi[i];
				d2 = d1 - e[i];
			}

			if (d1 > 0.0f && d2 > 0.0f)
				return false;

			if (d1 <= 0.0f && d2 <= 0.0f)
				continue;

			if (d1 > 0)
				start_solid = false;

			if (d1 > d2) {
				f = d1;
				if (f < 0.0f)
					f = 0.0f;

				f /= d1 - d2;
				if (f > t1)
					t1 = f;
			}
			else {
				f = d1 / (d1 - d2);
				if (f < t2)
					t2 = f;
			}
		}

		return start_solid || (t1 < t2 && t1 >= 0.0f);
	}

	__forceinline static void matrix_set_origin(c_vector3d pos, matrix3x4& matrix)
	{
		matrix[0][3] = pos.x;
		matrix[1][3] = pos.y;
		matrix[2][3] = pos.z;
	}

	__forceinline static c_vector3d matrix_get_origin(const matrix3x4& src)
	{
		return { src[0][3], src[1][3], src[2][3] };
	}

	__forceinline static c_vector3d rotate_2d(c_vector3d start, float rotation, float distance)
	{
		return c_vector3d(
			start.x + std::cos(deg2rad(rotation)) * distance,
			start.y + std::sin(deg2rad(rotation)) * distance,
			start.z);
	};
};
