#pragma once

#include "c_drawable.h"
#include "../../d3d9/d3dx9.h"

class c_image : public c_drawable
{
public:
	c_image(c_vector2d size, void* image_data, uint32_t image_size, float scale = 1.f);

	void draw(c_vector2d position) override;

private:
	LPDIRECT3DTEXTURE9 image;
	LPD3DXSPRITE sprite;

	void* image_data;
	uint32_t image_size;

	float scale;
};
