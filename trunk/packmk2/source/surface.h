#ifndef _surface_
#define _surface_
//
// surface
//
// Copyright (c) 2003 Claes Johanson

#include <d3dx9.h>
#include "camera.h"
#include "parameterhandler.h"
#include "software_noisemaker.h"

#define skyboxdetail 16	

#define reflrefrdetail 512

struct togglestate
{
	bool	animation,
			view_normals;			
};

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z;      // The untransformed, 3D position for the vertex
	DWORD color;        // The vertex color
};

struct SURFACEVERTEX
{
	D3DXVECTOR3	position;	
	float displacement;
};

struct DISPLACEMENT
{
	DWORD displacement;
};

enum rendermode
{
	RM_POINTS = 0,
	RM_WIREFRAME,
	RM_SOLID
};
// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define D3DFVF_SURFACEVERTEX (D3DFVF_XYZ|D3DFVF_TEX1) //|D3DFVF_TEX1
#define D3DFVF_DISPLACEMENT (D3DFVF_DIFFUSE)
#define D3DFVF_SURFACEVERTEX_AND_DISPLACEMENT (D3DFVF_XYZ|D3DFVF_DIFFUSE)
class surface;

class surface{
public:
	surface(const D3DXVECTOR3* position, const D3DXVECTOR3* normal,int gridsize_x, int gridsize_y,const LPDIRECT3DDEVICE9 device, camera *tesscamera, parameterhandler *prm);
	~surface();
	bool prepare(const camera*);
	bool render();
	void render_cutter();
	void render_skybox();
	void calc_efficiency();
	void set_render_mode(int rendermode);
	void set_grid_size(int size_x, int size_y);
	void set_displacement_amplitude(float amplitude);
	float get_height_at(float,float);
	
	char debugdata[500];
	float efficiency;
	D3DXMATRIXA16 range;
	D3DXPLANE	plane, upper_bound, lower_bound;
	LPDIRECT3DTEXTURE9			surf_refraction, surf_reflection;
	LPDIRECT3DSURFACE9			depthstencil;

	camera						*projecting_camera,	// the camera that does the actual projection
								*rendering_camera,		// the camera whose frustum the projection is created for
								*observing_camera;		// the camera the geometry is transformed through when rendering.
														// in real use rendering_camera and observing_camera should be the same
														// they can be different for demonstration purposes though

private:
	parameterhandler *prm;
	bool initbuffers();
	bool getMinMax(D3DXMATRIXA16 *range);
	void surface::SetupMatrices(const camera *camera_view);
	bool surface::within_frustum(const D3DXVECTOR3 *pos);
	void surface::LoadEffect();
	bool initialized, boxfilter;

protected:	
	D3DXVECTOR3	normal, u, v, pos;
	float		min_height,max_height;
	int			gridsize_x, gridsize_y, rendermode;
	bool		plane_within_frustum;
	
	
	software_noisemaker *software_brus;
		
	LPDIRECT3DVERTEXBUFFER9		surf_software_vertices;	
	LPDIRECT3DINDEXBUFFER9		surf_indicies;
	
	LPDIRECT3DTEXTURE9			surf_texture, surf_fresnel, underwater_fresnel, noise2D;
	LPD3DXEFFECT				surf_software_effect, underwater_software_effect;


	LPDIRECT3DVERTEXBUFFER9		skybox_vertices;
	LPDIRECT3DINDEXBUFFER9		skybox_indicies;
	LPD3DXEFFECT				skybox_effect;
	
	LPDIRECT3DCUBETEXTURE9		sky_cubemap;	
	LPDIRECT3DDEVICE9			device;	
	
};

#endif