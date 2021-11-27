#include "c_renderer.h"
#include "../fonts/profont.h"
#include "../security/string_obfuscation.h"
#include "../utils/math.h"
#include "../utils/c_memory.h"
#include <utility>

c_renderer::c_renderer(IDirect3DDevice9* dev)
{
	DWORD fnt;
	AddFontMemResourceEx(reinterpret_cast<void*>(profont), sizeof(profont), nullptr, &fnt);

	fonts[fnv1a("pro12")] = c_font(_("ProFont for Powerline"), 12, FW_REGULAR);
	fonts[fnv1a("pro13")] = c_font(_("ProFont for Powerline"), 13, FW_REGULAR);
	fonts[fnv1a("pro17")] = c_font(_("ProFont for Powerline"), 17, FW_REGULAR);
	init_device_objects(dev);
}

void c_renderer::line(const c_vector2d from, const c_vector2d to, const c_color color) const
{
	const auto col = color.direct();

	vertex vert[2] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
		{ to.x, to.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::rect(const c_vector2d from, const c_vector2d size, const c_color color) const
{
	const auto to = from + size;
	const auto col = color.direct();

	vertex vert[5] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
		{ to.x, from.y, 0.0f, 1.0f, col },
		{ to.x, to.y, 0.0f, 1.0f, col },
		{ from.x, to.y, 0.0f, 1.0f, col },
		{ from.x, from.y, 0.0f, 1.0f, col }
	};

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));
}

void c_renderer::rect_linear_gradient(const c_vector2d from, const c_vector2d size, const c_color color1, const c_color color2, const bool horizontal) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[5] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col2 },
	{ to.x, from.y, 0.0f, 1.0f, horizontal ? col2 : col1 },
	{ to.x, to.y, 0.0f, 1.0f, col2 },
	{ from.x, to.y, 0.0f, 1.0f, horizontal ? col1 : col2 },
	{ from.x, from.y, 0.0f, 1.0f, col1 }
	};

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));
}

void c_renderer::rect_full_linear_gradient(const c_vector2d from, const c_vector2d size, c_color color1, c_color color2, c_color color3,
	c_color color4) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	const auto col3 = color3.direct();
	const auto col4 = color4.direct();

	vertex vert[5] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, col2 },
	{ to.x, to.y, 0.0f, 1.0f, col3 },
	{ from.x, to.y, 0.0f, 1.0f, col4 },
	{ from.x, from.y, 0.0f, 1.0f, col1 }
	};

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));
}

void c_renderer::rect_filled(const c_vector2d from, const c_vector2d size, const c_color color) const
{
	const auto to = from + size;
	const auto col = color.direct();

	vertex vert[4] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
	{ to.x, from.y, 0.0f, 1.0f, col },
	{ from.x, to.y, 0.0f, 1.0f, col },
	{ to.x, to.y, 0.0f, 1.0f, col }
	};

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(vertex));
}

void c_renderer::rect_filled_linear_gradient(const c_vector2d from, const c_vector2d size, const c_color color1,
	const c_color color2, const bool horizontal) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[4] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, horizontal ? col2 : col1 },
	{ from.x, to.y, 0.0f, 1.0f, horizontal ? col1 : col2 },
	{ to.x, to.y, 0.0f, 1.0f, col2 }
	};

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(vertex));
}

void c_renderer::rect_filled_radial_gradient(const c_vector2d from, const c_vector2d size, const c_color color1,
	const c_color color2)
{
	const auto center = from + size / 2.0f;
	const auto radius = (center - from).length();

	D3DVIEWPORT9 new_port;
	new_port.X = static_cast<uint32_t>(from.x);
	new_port.Y = static_cast<uint32_t>(from.y);
	new_port.Width = static_cast<uint32_t>(size.x);
	new_port.Height = static_cast<uint32_t>(size.y);

	dev->SetViewport(&new_port);
	circle_filled_radial_gradient(center, radius, color1, color2);
	dev->SetViewport(&port);
}

void c_renderer::rect_filled_diamond_gradient(const c_vector2d from, const c_vector2d size, const c_color color1,
	const c_color color2) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[6] =
	{
		{ (from.x + to.x) / 2.0f, (from.y + to.y) / 2.0f, 0.0f, 1.0f, col2 },
	{ from.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, to.y, 0.0f, 1.0f, col1 },
	{ from.x, to.y, 0.0f, 1.0f, col1 },
	{ from.x, from.y, 0.0f, 1.0f, col1 }
	};

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 5, &vert, sizeof(vertex));
}

void c_renderer::parallelogram(const c_vector2d from, const c_vector2d size, c_color color, const uint8_t side, const float radius) const
{
	const auto to = from + size;
	const auto col = color.direct();

	vertex vert[5] =
	{
		{ from.x + (side != 1 ? radius : 0.0f), from.y, 0.0f, 1.0f, col },
	{ to.x, from.y, 0.0f, 1.0f, col },
	{ to.x - (side != 2 ? radius : 0.0f), to.y, 0.0f, 1.0f, col },
	{ from.x, to.y, 0.0f, 1.0f, col },
	{ from.x + (side != 1 ? radius : 0.0f), from.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::parallelogram_filled_linear_gradient(const c_vector2d from, const c_vector2d size, c_color color1,
	c_color color2, const uint8_t side, const bool horizontal, const float radius) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[4] =
	{
		{ from.x + (side != 1 ? radius : 0.0f), from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, horizontal ? col2 : col1 },
	{ from.x, to.y, 0.0f, 1.0f, horizontal ? col1 : col2 },
	{ to.x - (side != 2 ? radius : 0.0f), to.y, 0.0f, 1.0f, col2 }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::triangle(const c_vector2d pos1, const c_vector2d pos2, const c_vector2d pos3, const c_color color) const
{
	const auto col = color.direct();

	vertex vert[4] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col },
	{ pos1.x, pos1.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::triangle_linear_gradient(const c_vector2d pos1, const c_vector2d pos2, const c_vector2d pos3, c_color color1, c_color color2, c_color color3) const
{
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	const auto col3 = color3.direct();

	vertex vert[4] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col1 },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col2 },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col3 },
	{ pos1.x, pos1.y, 0.0f, 1.0f, col1 }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::triangle_filled(const c_vector2d pos1, const c_vector2d pos2, const c_vector2d pos3, const c_color color) const
{
	const auto col = color.direct();

	vertex vert[4] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::triangle_filled_linear_gradient(const c_vector2d pos1, const c_vector2d pos2, const c_vector2d pos3,
	const c_color color1, const c_color color2, const c_color color3) const
{
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	const auto col3 = color3.direct();

	vertex vert[4] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col1 },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col2 },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col3 }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::circle(const c_vector2d center, const float radius, const c_color color)
{
	const auto col = color.direct();
	build_lookup_table();

	vertex vert[points + 1] = {};

	for (auto i = 0; i <= points; i++)
		vert[i] =
	{
		center.x + radius * lookup_table[i].x,
		center.y - radius * lookup_table[i].y,
		0.0f,
		1.0f,
		col
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, points, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::circle_filled(const c_vector2d center, const float radius, const c_color color)
{
	const auto col = color.direct();
	build_lookup_table();

	vertex vert[points + 1] = {};

	for (auto i = 0; i <= points; i++)
		vert[i] =
	{
		center.x + radius * lookup_table[i].x,
		center.y - radius * lookup_table[i].y,
		0.0f,
		1.0f,
		col
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::circle_filled_radial_gradient(const c_vector2d center, const float radius, const c_color color1,
	const c_color color2)
{
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	build_lookup_table();

	vertex vert[points + 2] = {};

	for (auto i = 1; i <= points; i++)
		vert[i] =
	{
		center.x + radius * lookup_table[i].x,
		center.y - radius * lookup_table[i].y,
		0.0f,
		1.0f,
		col1
	};

	vert[0] = { center.x, center.y, 0.0f, 1.0f, col2 };
	vert[points + 1] = vert[1];

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points, &vert, sizeof(vertex));

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::text(const c_vector2d pos, const char* str, c_color color, const uint32_t font, const uint8_t flags)
{
	fonts.at(font).draw_string(std::roundf(pos.x), std::roundf(pos.y), color.direct(), str, flags);
}

c_vector2d c_renderer::get_text_size(const char* str, const uint32_t font)
{
	SIZE size;
	fonts.at(font).get_text_extent(str, &size);
	return c_vector2d(static_cast<float>(size.cx), static_cast<float>(size.cy));
}

void c_renderer::ball(const c_vector3d center, const float radius, const matrix3x4 transform, const c_color col, viewmatrix& matrix)
{
	build_lookup_table();

	const auto color = col.direct();

	vertex vert[points_sphere_longitude + 1] = {};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	for (auto i = 0u; i < lookup_sphere.size(); i++)
	{
		auto& point = lookup_sphere[i];
		const auto element = i % (points_sphere_longitude + 1);

		c_vector2d to;

		if (!screen_transform(math::vector_rotate(point, transform) * radius + center, to, matrix))
		{
			i += points_sphere_longitude + 1 - element;
			continue;
		}

		vert[element] = {
			to.x,
			to.y,
			0.0f,
			1.0f,
			color
		};

		if (!element)
		{
			vert[0] = vert[points_sphere_longitude];
			dev->SetTexture(0, nullptr);
			dev->DrawPrimitiveUP(D3DPT_LINESTRIP, points_sphere_longitude, &vert, sizeof(vertex));
		}
	}

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::create_texture(void* src, const uint32_t size, LPDIRECT3DTEXTURE9* texture) const
{
	D3DXCreateTextureFromFileInMemory(dev, src, size, texture);
}

void c_renderer::create_sprite(LPD3DXSPRITE* sprite) const
{
	D3DXCreateSprite(dev, sprite);
}

void c_renderer::image(const c_vector2d position, LPDIRECT3DTEXTURE9 texture, LPD3DXSPRITE sprite, const float scl, const float alpha)
{
	D3DXMATRIX world;
	D3DXMATRIX rotation;
	D3DXMATRIX scale;
	D3DXMATRIX translation;
	D3DXMatrixIdentity(&world);

	D3DXMatrixScaling(&scale, scl, scl, 1.f);
	D3DXMatrixRotationYawPitchRoll(&rotation, 0.f, 0.f, 0.f);
	D3DXMatrixTranslation(&translation, 0.f, 0.f, 0.f);
	world = rotation * scale * translation;

	D3DSURFACE_DESC img_info;
	texture->GetLevelDesc(0, &img_info);

	auto vec = D3DXVECTOR3(position.x, position.y, 0.f);
	sprite->SetTransform(&world);
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(texture, nullptr, nullptr, &vec, D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(255 * alpha)));
	sprite->End();
}

c_vector2d c_renderer::get_center() const
{
	return c_vector2d(static_cast<float>(port.Width), static_cast<float>(port.Height)) / 2.f;
}

viewmatrix& c_renderer::world_to_screen_matrix()
{
	static auto view_matrix = *reinterpret_cast<uintptr_t*>(sig("client_panorama.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9") + 3) + 176;
	return *reinterpret_cast<viewmatrix*>(view_matrix);
}

void c_renderer::limit(const rectangle rect) const
{
	RECT rec;
	rec.left = static_cast<LONG>(rect.first.x);
	rec.top = static_cast<LONG>(rect.first.y);
	rec.right = static_cast<LONG>(rect.first.x + rect.second.x);
	rec.bottom = static_cast<LONG>(rect.first.y + rect.second.y);

	dev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	dev->SetScissorRect(&rec);
}

rectangle c_renderer::get_limit() const
{
	RECT rec;
	dev->GetScissorRect(&rec);

	return rectangle({
		static_cast<float>(rec.left),
		static_cast<float>(rec.top)
		}, {
			static_cast<float>(rec.right),
			static_cast<float>(rec.bottom)
		});
}

void c_renderer::reset_limit() const
{
	dev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}

void c_renderer::refresh_viewport()
{
	this->dev->GetViewport(&port);
}

void c_renderer::init_device_objects(IDirect3DDevice9* dev)
{
	this->dev = dev;
	this->dev->GetViewport(&port);

	for (auto& font : fonts)
	{
		font.second.init_device_objects(dev);
		font.second.restore_device_objects();
	}
}

void c_renderer::invalidate_device_objects()
{
	dev = nullptr;

	for (auto& font : fonts)
		font.second.invalidate_device_objects();

	for (auto& handler : reset_handlers)
		handler();
}

void c_renderer::setup_render_state() const
{
	dev->SetVertexShader(nullptr);
	dev->SetPixelShader(nullptr);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE); // NOLINT
	dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	dev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	dev->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	dev->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	dev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN // NOLINT NOLINTNEXTLINE
		| D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

	dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
}

void c_renderer::register_reset_handler(const std::function<void()> handler)
{
	reset_handlers.push_back(handler);
}

void c_renderer::build_lookup_table()
{
	if (!lookup_table.empty())
		return;

	for (auto i = 0; i <= points; i++)
		lookup_table.emplace_back(
		std::cos(2.f * D3DX_PI * (i / static_cast<float>(points))),
		std::sin(2.f * D3DX_PI * (i / static_cast<float>(points)))
		);

	for (auto lat = 0; lat < points_sphere_latitude; lat++)
	{
		const auto a1 = D3DX_PI * static_cast<float>(lat + 1) / (points_sphere_latitude + 1);
		const auto sin1 = sin(a1);
		const auto cos1 = cos(a1);

		for (auto lon = 0; lon <= points_sphere_longitude; lon++)
		{
			const auto a2 = 2 * D3DX_PI * static_cast<float>(lon) / points_sphere_longitude;
			const auto sin2 = sin(a2);
			const auto cos2 = cos(a2);

			lookup_sphere.emplace_back(sin1 * cos2, cos1, sin1 * sin2);
		}
	}
}
