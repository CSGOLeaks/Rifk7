#pragma once

#include "../includes.h"
#include "../renderer/c_renderer.h"

class idirect3ddevice9 : public c_singleton<idirect3ddevice9>
{
	typedef HRESULT(__stdcall* reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	typedef HRESULT(__stdcall* end_scene_t)(IDirect3DDevice9*);

public:
    idirect3ddevice9();

	c_renderer* get_renderer() const;
	static IDirect3DDevice9* direct_device();
private:
	std::unique_ptr<c_renderer> default_renderer;

	reset_t _reset;
	end_scene_t _end_scene;

	static HRESULT __stdcall reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* param);
	static HRESULT __stdcall end_scene(IDirect3DDevice9* dev);
};

#define device idirect3ddevice9::instance()->direct_device()
#define renderer idirect3ddevice9::instance()->get_renderer()
