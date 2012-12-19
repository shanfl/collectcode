
//-----------------------------------------------------------------------------
// File: tessdemo.cpp
//
// Desc: Demo of the projective mesh concept
//
// Copyright (c) 2003 Claes Johanson
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <stdio.h>
#include <math.h>
#include "dxmouse.h"
#include "camera.h"
#include "surface.h"

#include "parameterhandler.h"
#include "software_noisemaker.h"

surface *sea;

dxmouse *mouse;
float mouseX=0, mouseY=0;
float mspeed = 0.005;
bool drag=false;
bool text=true;

struct duck
{
	float x,y,z;
	float y_vel;
	float angle;
};

duck duckie_pos;


parameterhandler *prm;

bool	keys[256];			// Array Used For The Keyboard Routine

#define PI	3.1415926535898
#define		n_cameras 2
int			active_camera_view = 0, active_camera_mouse = 0;
camera		*camera_mouse, *camera_view, *cameras[n_cameras];

#ifdef CPU_NORMALS
int gridsize_x = 256, gridsize_y = 512;		
#else
int gridsize_x = 128, gridsize_y = 256;		
#endif

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D				= NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL; // Buffer to hold vertices
LPDIRECT3DVERTEXBUFFER9 g_pVBproj			= NULL; // Buffer to hold vertices

LPDIRECT3DVERTEXBUFFER9 g_referencePlaneVB	= NULL;
LPD3DXFONT				g_pd3dxFont			= NULL;
LPDIRECT3DSURFACE9		g_depthstencil		= NULL;
LPDIRECT3DTEXTURE9		terrain_texture		= NULL;
LPD3DXEFFECT			terrain_effect		= NULL;
LPD3DXMESH				duckie				= NULL;
LPD3DXMESH				island				= NULL;
LPD3DXBUFFER duck_Adjacency, duck_Materials, duck_EffectInstances;
LPD3DXBUFFER island_Adjacency, island_Materials, island_EffectInstances;
D3DLIGHT9 sun;

// declarations
void createFont( void );

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	HRESULT hr;
	// Create the D3D object.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	//d3dpp.Windowed = FALSE;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	//d3dpp.BackBufferFormat = D3DFMT_A2R10G10B10;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;



	// Create the D3DDevice
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW  );		
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pd3dDevice->GetDepthStencilSurface(&g_depthstencil);

	// initialize camera objects
	cameras[0] = new camera(D3DXVECTOR3(-27,8,0),0.3,450,0,45,1.33,0.3,5000.0);
	cameras[1] = new camera(D3DXVECTOR3(-150,50,-100),-0.5,-1.3,0,45,1.33,0.1,10000.0);
	camera_mouse = cameras[active_camera_mouse];
	camera_view = cameras[active_camera_view];

	// Create a font for statistics and help output
	createFont();

	// create parameter handler
	prm = new parameterhandler();
	prm->set_float( p_fStrength,	0.9f );
	prm->set_bool(	p_bDisplace,	true );
	prm->set_int(	p_iOctaves,		8 );
	prm->set_float( p_fScale,		0.38f );
	prm->set_float( p_fFalloff,		0.607f );
	prm->set_float( p_fAnimspeed,	1.4f );
	prm->set_float( p_fTimemulti,	1.27f );
	prm->set_bool(	p_bPaused,		false );

	prm->set_float(	p_fLODbias,		0.0f);
	prm->set_bool(	p_bDisplayTargets, false );
	prm->set_float(	p_fElevation,	7.0f );

	//prm->set_float( p_fSunPosAlpha,		2.7f );
	//prm->set_float( p_fSunPosTheta,		0.1f );
	prm->set_float( p_fSunPosAlpha,		1.38f );
	prm->set_float( p_fSunPosTheta,		1.09f );
	prm->set_float( p_fSunShininess,	84.0f );
	prm->set_float( p_fSunStrength,		12.0f );

#ifdef CPU_NORMALS
	prm->set_bool(	p_bSmooth,			false );
#else
	prm->set_bool(	p_bSmooth,			true );
#endif
	prm->set_float(	p_bReflRefrStrength,0.1f );

	prm->set_float( p_fWaterColourR,	0.17f );
	prm->set_float( p_fWaterColourG,	0.27f );
	prm->set_float( p_fWaterColourB,	0.26f );

	prm->set_bool(	p_bAsPoints,		false );
	prm->set_bool(	p_bDrawDuckie,		true );
	prm->set_bool(	p_bDrawIsland,		false );
	prm->set_bool(	p_bDiffuseRefl,		false );

	prm->set_active_parameter(p_fStrength);


	// create sea object
	sea = new surface(&D3DXVECTOR3(0,0,0),&D3DXVECTOR3(0,1,0),gridsize_x,gridsize_y,g_pd3dDevice,cameras[0], prm);

	DWORD n_materials;
	D3DXLoadMeshFromX( "duckie.x", D3DXMESH_MANAGED, g_pd3dDevice, &duck_Adjacency, &duck_Materials, &duck_EffectInstances, &n_materials, &duckie );
	D3DXLoadMeshFromX( "island.x", D3DXMESH_MANAGED, g_pd3dDevice, &island_Adjacency, &island_Materials, &island_EffectInstances, &n_materials, &island );	

	// load terrain texture
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "textures/terrain.png", &terrain_texture ) ) )
	{
		MessageBox(NULL, "Could not find terrain texture", "lalala.exe", MB_OK);		
	}
	// load terrain underwater effect

		char *errortext;
	LPD3DXBUFFER errors;
	D3DXHANDLE hTechnique;

	D3DXCreateEffectFromFile(g_pd3dDevice, "terrain_underwater.fx", 
		NULL, NULL, 0, NULL, &terrain_effect, &errors );

	if (errors != NULL){
		errortext = (char*) errors->GetBufferPointer();
		MessageBox(NULL, errortext, "lalala.exe", MB_OK);		
	}

	terrain_effect->FindNextValidTechnique(NULL, &hTechnique);    
	terrain_effect->SetTechnique(hTechnique);



	return S_OK;
}

void loadpreset(int n)
{
	switch(n)
	{
	case 1:
		prm->set_float( p_fScale,		0.38f );
		prm->set_float( p_fStrength,	0.9f );
		prm->set_float( p_fFalloff,		0.607f );
		prm->set_float( p_fWaterColourR,	0.07f );
		prm->set_float( p_fWaterColourG,	0.11f );
		prm->set_float( p_fWaterColourB,	0.11f );
		prm->set_float(	p_fLODbias,		0.0f);
		prm->set_bool(	p_bAsPoints,		false );
		prm->set_bool(	p_bDrawDuckie,		true );
		prm->set_bool(	p_bDrawIsland,		true );
		prm->set_bool(	p_bDiffuseRefl,		false );
		break;
	case 2:
		prm->set_float( p_fScale,		0.38f );
		prm->set_float( p_fStrength,	4.0f );
		prm->set_float( p_fFalloff,		0.47f );
		prm->set_float( p_fWaterColourR,	0.13f );
		prm->set_float( p_fWaterColourG,	0.19f );
		prm->set_float( p_fWaterColourB,	0.22f );
		prm->set_float(	p_fLODbias,		4.5f);
		prm->set_bool(	p_bAsPoints,		false );
		prm->set_bool(	p_bDrawDuckie,		true );
		prm->set_bool(	p_bDrawIsland,		true );
		prm->set_bool(	p_bDiffuseRefl,		false );
		break;
	case 3:
		prm->set_float( p_fScale,		0.38f );
		prm->set_float( p_fStrength,	7.0f );
		prm->set_float( p_fFalloff,		0.53f );
		prm->set_float( p_fWaterColourR,	0.12f );
		prm->set_float( p_fWaterColourG,	0.22f );
		prm->set_float( p_fWaterColourB,	0.29f );
		prm->set_float(	p_fLODbias,		10.0f);
		prm->set_bool(	p_bAsPoints,		false );
		prm->set_bool(	p_bDrawDuckie,		false );
		prm->set_bool(	p_bDrawIsland,		false );
		prm->set_bool(	p_bDiffuseRefl,		true );
		prm->set_float( p_fSunPosAlpha,		2.91f );
		prm->set_float( p_fSunPosTheta,		0.36f );
		prm->set_float( p_fSunShininess,	1263.0f );
		prm->set_float( p_fSunStrength,		208.0f );
		break;
	case 4:
		prm->set_float( p_fScale,		0.38f );
		prm->set_float( p_fStrength,	4.0f );
		prm->set_float( p_fFalloff,		0.47f );
		prm->set_float( p_fWaterColourR,	0.13f );
		prm->set_float( p_fWaterColourG,	0.19f );
		prm->set_float( p_fWaterColourB,	0.22f );
		prm->set_float(	p_fLODbias,		4.5f);
		prm->set_bool(	p_bAsPoints,		true );
		prm->set_bool(	p_bDrawDuckie,		false );
		prm->set_bool(	p_bDrawIsland,		false );
		prm->set_bool(	p_bDiffuseRefl,		true );
		break;
	case 5:
		prm->set_float( p_fScale,		0.197f );
		prm->set_float( p_fStrength,	12.9f );
		prm->set_float( p_fFalloff,		0.467f );
		prm->set_float( p_fWaterColourR,	0.12f );
		prm->set_float( p_fWaterColourG,	0.20f );
		prm->set_float( p_fWaterColourB,	0.24f );
		prm->set_float(	p_fLODbias,			0.0f);
		prm->set_bool(	p_bAsPoints,		false );
		prm->set_bool(	p_bDrawDuckie,		true );
		prm->set_bool(	p_bDrawIsland,		true );
		prm->set_float( p_fSunPosAlpha,		2.91f );
		prm->set_float( p_fSunPosTheta,		0.88f );
		prm->set_float( p_fSunShininess,	1263.0f );
		prm->set_float( p_fSunStrength,		5270.0f );
		prm->set_bool(	p_bDiffuseRefl,		true );
		break;
	case 6:
		prm->set_float( p_fScale,		0.38f );
		prm->set_float( p_fStrength,	11.3f );
		prm->set_float( p_fFalloff,		0.56f );
		prm->set_float( p_fWaterColourR,	0.17f );
		prm->set_float( p_fWaterColourG,	0.27f );
		prm->set_float( p_fWaterColourB,	0.26f );
		prm->set_float(	p_fLODbias,			0.0f);
		prm->set_bool(	p_bAsPoints,		false );
		prm->set_bool(	p_bDrawDuckie,		true );
		prm->set_bool(	p_bDrawIsland,		true );
		prm->set_float( p_fSunPosAlpha,		2.91f );
		prm->set_float( p_fSunPosTheta,		0.256f );
		prm->set_float( p_fSunShininess,	700.0f );
		prm->set_float( p_fSunStrength,		200.85f );
		prm->set_bool(	p_bDiffuseRefl,		false );
		break;
	}
}


//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
#define color1 0xff000000
#define color2 0xffff7f7f
#define color3 0xff007fff
	// Initialize frustum box
	CUSTOMVERTEX g_Vertices[] =
	{
		// front
		{ -1.0f,-1.0f, -1.0f, color1, },
		{ +1.0f,-1.0f, -1.0f, color1, },
		{ -1.0f,-1.0f, -1.0f, color1, },
		{ -1.0f,+1.0f, -1.0f, color1, },
		{ +1.0f,-1.0f, -1.0f, color1, },
		{ +1.0f,+1.0f, -1.0f, color1, },
		{ -1.0f,+1.0f, -1.0f, color1, },
		{ +1.0f,+1.0f, -1.0f, color1, },
		// back
		{ -1.0f,-1.0f, +1.0f, color1, },
		{ +1.0f,-1.0f, +1.0f, color1, },
		{ -1.0f,-1.0f, +1.0f, color1, },
		{ -1.0f,+1.0f, +1.0f, color1, },
		{ +1.0f,-1.0f, +1.0f, color1, },
		{ +1.0f,+1.0f, +1.0f, color1, },
		{ -1.0f,+1.0f, +1.0f, color1, },
		{ +1.0f,+1.0f, +1.0f, color1, },
		// connects
		{ -1.0f,-1.0f, -1.0f, color1, },
		{ -1.0f,-1.0f, +1.0f, color1, },
		{ +1.0f,-1.0f, -1.0f, color1, },
		{ +1.0f,-1.0f, +1.0f, color1, },
		{ -1.0f,+1.0f, -1.0f, color1, },
		{ -1.0f,+1.0f, +1.0f, color1, },
		{ +1.0f,+1.0f, -1.0f, color1, },
		{ +1.0f,+1.0f, +1.0f, color1, },
	};
		CUSTOMVERTEX g_Vertices2[] =
	{
		// front
		{ -1.0f,-1.0f, -1.0f, color2, },
		{ +1.0f,-1.0f, -1.0f, color2, },
		{ -1.0f,-1.0f, -1.0f, color2, },
		{ -1.0f,+1.0f, -1.0f, color2, },
		{ +1.0f,-1.0f, -1.0f, color2, },
		{ +1.0f,+1.0f, -1.0f, color2, },
		{ -1.0f,+1.0f, -1.0f, color2, },
		{ +1.0f,+1.0f, -1.0f, color2, },
		// back
		{ -1.0f,-1.0f, +1.0f, color2, },
		{ +1.0f,-1.0f, +1.0f, color2, },
		{ -1.0f,-1.0f, +1.0f, color2, },
		{ -1.0f,+1.0f, +1.0f, color2, },
		{ +1.0f,-1.0f, +1.0f, color2, },
		{ +1.0f,+1.0f, +1.0f, color2, },
		{ -1.0f,+1.0f, +1.0f, color2, },
		{ +1.0f,+1.0f, +1.0f, color2, },
		// connects
		{ -1.0f,-1.0f, -1.0f, color2, },
		{ -1.0f,-1.0f, +1.0f, color2, },
		{ +1.0f,-1.0f, -1.0f, color2, },
		{ +1.0f,-1.0f, +1.0f, color2, },
		{ -1.0f,+1.0f, -1.0f, color2, },
		{ -1.0f,+1.0f, +1.0f, color2, },
		{ +1.0f,+1.0f, -1.0f, color2, },
		{ +1.0f,+1.0f, +1.0f, color2, },
	};

	CUSTOMVERTEX g_plane[] = {
		{ -200.0f, 0.0f, -200.0f, 0x7f00007f, },
		{ 200.0f, 0.0f, -200.0f, 0x7f00007f, },
		{ 200.0f, 0.0f, 200.0f, 0x7f00007f, },
		{ -200.0f, 0.0f, 200.0f, 0x7f00007f, },
	};

	// Create the vertex buffer.
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 24*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// Fill the vertex buffer.
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
	g_pVB->Unlock();

	
	// Create the vertex buffer.
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 24*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVBproj, NULL ) ) )
	{
		return E_FAIL;
	}

	// Fill the vertex buffer.	
	if( FAILED( g_pVBproj->Lock( 0, sizeof(g_Vertices2), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, g_Vertices2, sizeof(g_Vertices2) );
	g_pVBproj->Unlock();

	// Create the vertex buffer.
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_referencePlaneVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// Fill the vertex buffer.

	if( FAILED( g_referencePlaneVB->Lock( 0, sizeof(g_plane), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, g_plane, sizeof(g_plane) );
	g_referencePlaneVB->Unlock();

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if( g_pVB != NULL )
		g_pVB->Release();

	if( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
	D3DXMATRIXA16 matWorld,matProj;
	D3DXMatrixIdentity(&matWorld);
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &(camera_view->view) );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &(camera_view->proj) );
}



float getheight(float x, float z)
{
	//	return displacement*0.5*(1+sin(0.15*x)*sin(z));
	return 0;
}

set_clipplane_height(float height)
{
	float plane[4];

	g_pd3dDevice->GetClipPlane(0,plane);

	plane[3] = height;

	g_pd3dDevice->SetClipPlane(0,plane);
}

enum rs_mode {
	rsm_normal=0,
	rsm_refraction,
	rsm_reflection,
	rsm_reflection_backface,
};

void render_scene(int mode)
{
	// if displaced==true, objects should be displaced according to the waveheight at their location, so the reflections will match

	// set lightning
	sun.Direction.x = -cos(prm->get_float(p_fSunPosTheta))*sin(prm->get_float(p_fSunPosAlpha));
	sun.Direction.y = -sin(prm->get_float(p_fSunPosTheta));
	sun.Direction.z = -cos(prm->get_float(p_fSunPosTheta))*cos(prm->get_float(p_fSunPosAlpha));
	if(mode==rsm_reflection)
	{
		//sun.Direction.x = -sun.Direction.x;
		sun.Direction.y = -sun.Direction.y;
		//sun.Direction.z = -sun.Direction.z;
	}
	sun.Diffuse.r = 2.0f;
	sun.Diffuse.g = 2.0f;
	sun.Diffuse.b = 2.0f;
	sun.Diffuse.a = 1.0f;
	sun.Ambient.a = 1.0f;
	sun.Ambient.r = 0.2f;
	sun.Ambient.g = 0.3f;
	sun.Ambient.b = 0.3f;
	sun.Specular.r = 1.0f;
	sun.Specular.g = 1.0f;
	sun.Specular.b = 1.0f;
	sun.Specular.a = 1.0f;

	sun.Attenuation0 = 1.0f;
	sun.Type = D3DLIGHT_DIRECTIONAL;
	g_pd3dDevice->SetLight(0, &sun);
	g_pd3dDevice->LightEnable( 0, true);		

	// draw the badanka
	if (mode!=rsm_refraction)
	{
		D3DXMATRIXA16 store, offset, yrot,scale;
		g_pd3dDevice->GetTransform(D3DTS_WORLD,&store);
		float duckX = -15, duckZ = -5;
		if (mode == rsm_normal)
			D3DXMatrixTranslation( &offset, duckie_pos.x, duckie_pos.y, duckie_pos.z);
		else if(mode == rsm_reflection)
			D3DXMatrixTranslation( &offset, duckie_pos.x, -duckie_pos.y + sea->get_height_at(duckie_pos.x,duckie_pos.z), duckie_pos.z);
		else
			D3DXMatrixTranslation( &offset, duckie_pos.x, 1.33*duckie_pos.y, duckie_pos.z);

		g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &offset );
		D3DXMatrixRotationY( &yrot, duckie_pos.angle);
		g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &yrot );
		D3DXMatrixScaling( &scale, 3, 3, 3 );
		g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &scale );

		//g_pd3dDevice->SetMaterial((D3DMATERIAL9*)duck_Materials->GetBufferPointer());
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true);

		g_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		g_pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1 );
		g_pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );

		set_clipplane_height(sea->get_height_at(duckX,duckZ) + 0.1);
		if (prm->get_bool(p_bDrawDuckie)) duckie->DrawSubset(0);		


		g_pd3dDevice->SetTransform(D3DTS_WORLD, &store);
	}
	sun.Diffuse.r = 1.0f;
	sun.Diffuse.g = 1.0f;
	sun.Diffuse.b = 1.0f;
	g_pd3dDevice->SetLight(0, &sun);
	if (prm->get_bool(p_bDrawIsland)) 
	{	
		if (mode == rsm_refraction)
		{
			//g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW  );	//lalal
			
			LPDIRECT3DVERTEXBUFFER9 vb;
			LPDIRECT3DINDEXBUFFER9 ib;
			island->GetVertexBuffer(&vb);
			island->GetIndexBuffer(&ib);	
			g_pd3dDevice->SetStreamSource(0, vb, 0, island->GetNumBytesPerVertex());
			g_pd3dDevice->SetIndices(ib);
			g_pd3dDevice->SetFVF( island->GetFVF() );

			terrain_effect->Begin(NULL,NULL);
			terrain_effect->Pass(0);			
			terrain_effect->SetMatrix("mViewProj",&(camera_view->viewproj));
			terrain_effect->SetMatrix("mView",&(camera_view->view));			
			float sa = prm->params[p_fSunPosAlpha].fData, st = prm->params[p_fSunPosTheta].fData;
			terrain_effect->SetVector("sun_vec",&D3DXVECTOR4(cos(st)*sin(sa), sin(st), cos(st)*cos(sa),0));
			terrain_effect->SetFloat("sun_shininess", prm->params[p_fSunShininess].fData);			
			terrain_effect->SetFloat("sun_strength", prm->params[p_fSunStrength].fData);
			terrain_effect->SetVector("watercolour", &D3DXVECTOR4(prm->params[p_fWaterColourR].fData,prm->params[p_fWaterColourG].fData,prm->params[p_fWaterColourB].fData,1));
			terrain_effect->SetVector("view_position", &D3DXVECTOR4(camera_view->position.x,camera_view->position.y,camera_view->position.z,1));
			terrain_effect->SetTexture("texDiffuse",terrain_texture);						
		
			g_pd3dDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST, 0,	0, island->GetNumVertices(), 0, island->GetNumFaces() );				
			terrain_effect->End();
		} else {

			//g_pd3dDevice->SetMaterial((D3DMATERIAL9*)duck_Materials->GetBufferPointer());
			g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true);

			g_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
			g_pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
			g_pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
			g_pd3dDevice->SetTexture(0, terrain_texture);

			/*D3DXATTRIBUTERANGE AttribTable;
			DWORD AttribTableSize;
			island->GetAttributeTable( &AttribTable, &AttribTableSize);*/
			//AttribTable
			set_clipplane_height(0);
			for(int i=0; i<7; i++)
			{
				LPD3DXMATERIAL mat = (LPD3DXMATERIAL) island_Materials->GetBufferPointer();			
				g_pd3dDevice->SetMaterial(&(mat->MatD3D));
				island->DrawSubset(i);
			}					
		}
	}	

	// restore stuff
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, false);		
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW  );
}

void render_refracted_scene()
{
	// set rendertarget
	LPDIRECT3DSURFACE9 target,bb;
	g_pd3dDevice->GetRenderTarget(0, &bb );
	sea->surf_refraction->GetSurfaceLevel( 0,&target );
	g_pd3dDevice->SetRenderTarget(0, target);

	/*g_pd3dDevice->Clear( 0, NULL,D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
	sea->render_cutter();
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_GREATEREQUAL  );	*/

	g_pd3dDevice->Clear( 0, NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB((int)(255*prm->params[p_fWaterColourR].fData),(int)(255*prm->params[p_fWaterColourG].fData),(int)(255*prm->params[p_fWaterColourB].fData)), 1.0f, 0 );
	D3DXMATRIXA16 store, scale;	
	// squach the scene
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&store);
	D3DXMatrixScaling( &scale, 1, 0.75, 1 );
	g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &scale );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW  );	
	// add a clip-plane as well	
	float plane[4];

	plane[0] = 0;
	plane[1] = -1;
	plane[2] = 0;
	plane[3] = 1.7*prm->params[p_fStrength].fData;	// a slight offset to avoid seams

	g_pd3dDevice->SetClipPlane(0,plane);
	//g_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 1);

	render_scene(rsm_refraction);

	// restore	
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	g_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &store);	
	g_pd3dDevice->SetRenderTarget(0, bb);
}


void render_actual_scene()
{
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW  );	
	g_pd3dDevice->SetDepthStencilSurface(g_depthstencil);
	render_scene(rsm_normal);
}

void render_reflected_scene()
{
	// set rendertarget
	LPDIRECT3DSURFACE9 target,bb;
	g_pd3dDevice->GetRenderTarget(0, &bb );
	sea->surf_reflection->GetSurfaceLevel( 0,&target );
	g_pd3dDevice->SetRenderTarget(0, target);
	g_pd3dDevice->SetDepthStencilSurface( sea->depthstencil );

	// alpha & z must be cleared
	g_pd3dDevice->Clear( 0, NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );

	D3DXMATRIXA16 store, scale;	
	// mirror the scene
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&store);
	D3DXMatrixScaling( &scale, 1, -1, 1 );
	g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &scale );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW  );		

	// add a clip-plane as well	
	float plane[4];

	plane[0] = 0;
	plane[1] = -1;
	plane[2] = 0;
	plane[3] = 0;

	g_pd3dDevice->SetClipPlane(0,plane);
	g_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 1);
	//g_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 1);

	//g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER );

	render_scene(rsm_reflection);

	//g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA );
	//g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, false);
	/*D3DMATERIAL9 noalpha;
	noalpha.Diffuse.g = 255;
	noalpha.Diffuse.a = 0;
	noalpha.Ambient.a = 0;
	noalpha.Specular.a = 0;
	g_pd3dDevice->SetMaterial(&noalpha);
	g_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
	g_pd3dDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	sea->render_cutter();
	g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, true);
	g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F );*/

	// restore
	g_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &store);	
	g_pd3dDevice->SetRenderTarget(0, bb);
	g_pd3dDevice->SetDepthStencilSurface( g_depthstencil );
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws everything
//-----------------------------------------------------------------------------
void Render()
{	
	static DWORD it=0;
	static DWORD time_prepare, time_total, time_total_start;
	it++;
	// measure fps
	if (it>63)
	{
		time_total = timeGetTime() - time_total_start;
		time_total_start = timeGetTime();
		it = 0;
	}

	// update mouse & kbd
	mouse->Update();
	float movespeed;
	if( GetAsyncKeyState(VK_LCONTROL) != 0)
	{
		mspeed = 0.0001;
		movespeed = 0.03f;
	} 
	else if( GetAsyncKeyState(VK_LSHIFT) != 0)
	{
		mspeed = 0.05;
		movespeed = 5.0f;
	} 
	else
	{
		mspeed = 0.005;
		movespeed = 0.3f;
	}

	// keys

	if(keys['A'])
		camera_mouse->position -= movespeed*camera_mouse->right;
	if(keys['D'])
		camera_mouse->position += movespeed*camera_mouse->right;
	if(keys['W'])
		camera_mouse->position += movespeed*camera_mouse->forward;
	if(keys['S'])
		camera_mouse->position -= movespeed*camera_mouse->forward;
	if(keys['Q'])
		camera_mouse->position += movespeed*camera_mouse->up;
	if(keys['Z'])
		camera_mouse->position -= movespeed*camera_mouse->up;
	camera_mouse->update();

	if(mouse->mousedown(MOUSE_LEFT)){
		camera_mouse->roty -= 0.005 * mouse->x;
		camera_mouse->rotx -= 0.005 * mouse->y;
		camera_mouse->update();
	} else if(mouse->mousedown(MOUSE_RIGHT)){
		camera_mouse->position += 5 * mspeed * mouse->x * camera_mouse->right;
		camera_mouse->position -= 5 * mspeed * mouse->y * camera_mouse->up;
		camera_mouse->update();
	} else if(mouse->mousedown(MOUSE_MIDDLE)){
		camera_mouse->position -= 10*mspeed * mouse->y * camera_mouse->forward;
		//camera_mouse->rotz -= mspeed * mouse->x;		
		camera_mouse->update();
	} 

	DWORD time_prepare_start = timeGetTime();
	sea->prepare(camera_view);
	time_prepare = timeGetTime() - time_prepare_start;

	// move the duck
	duckie_pos.angle += 0.001;
	if (duckie_pos.angle > (2*PI)) duckie_pos.angle -= 2*PI;
	duckie_pos.x = 20*cos(duckie_pos.angle);
	duckie_pos.z = -20*sin(duckie_pos.angle);
	float new_y = sea->get_height_at(duckie_pos.x,duckie_pos.z);
	duckie_pos.y_vel = new_y - duckie_pos.y;
	duckie_pos.y += 0.2*duckie_pos.y_vel;


	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{			
		// Setup the world, view, and projection matrices (this is used to render the frustum)
		SetupMatrices();

		
		// render reflections/refractions into textures
//#ifdef REFRACTION
		render_refracted_scene();
//#endif
		
		render_reflected_scene();		

		if (prm->get_bool(p_bAsPoints))
			g_pd3dDevice->Clear( 0, NULL,D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255,255), 1.0f, 0 );
		else
			g_pd3dDevice->Clear( 0, NULL,D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, D3DCOLOR_XRGB((int)(255*prm->params[p_fWaterColourR].fData),(int)(255*prm->params[p_fWaterColourG].fData),(int)(255*prm->params[p_fWaterColourB].fData)), 1.0f, 0 );	
		
		// Begin the scene		

		// set rendering states		
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,true);
		g_pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);		

		// render sea
		if (!prm->get_bool(p_bAsPoints))	sea->render_skybox();
		sea->render();	

		render_actual_scene();		

		//D3DRS_BLENDOP
		// Render the camera frustum		
		if(active_camera_view == 1)
		{
			{
				D3DXMATRIXA16 store;
				g_pd3dDevice->GetTransform(D3DTS_WORLD,&store);
				g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &(cameras[0]->invviewproj));
				g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
				g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
				g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 12 );

				g_pd3dDevice->SetTransform(D3DTS_WORLD, &store);
			}
			// Render the projector frustum		
			{
				D3DXMATRIXA16 store;
				g_pd3dDevice->GetTransform(D3DTS_WORLD,&store);
				g_pd3dDevice->MultiplyTransform(D3DTS_WORLD, &(sea->projecting_camera->invviewproj));
				g_pd3dDevice->SetStreamSource( 0, g_pVBproj, 0, sizeof(CUSTOMVERTEX) );
				g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
				g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 12 );

				g_pd3dDevice->SetTransform(D3DTS_WORLD, &store);
			}
		}

		// render the reference plane
		/*
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		g_pd3dDevice->SetStreamSource( 0, g_referencePlaneVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		*/			


		// Draw text		
		if (text)
		{
			//		g_pd3dxFont->Begin();		

			RECT textpos;
			char textString[900],activecamera[16],view[16];
			SetRect( &textpos, 5, 5, 0, 0 );

			if(camera_mouse==cameras[0])
				sprintf(activecamera,"tesselating");
			else if(camera_mouse==cameras[1])
				sprintf(activecamera,"observing");

			if(camera_view==cameras[0])
				sprintf(view,"primary");
			else if(camera_view==cameras[1])
				sprintf(view,"observer");

			sprintf( textString, "Mouse is currently controlling the %s camera (switch with TAB)\n",activecamera);
			sprintf( textString, "%sefficiency: %.2f percent  \n",textString,100*sea->efficiency);
			sprintf( textString, "%sdetail: %ix%i\n",textString,gridsize_x,gridsize_y);
			sprintf( textString, "%sview(switch with 'c'): %s\n",textString,view);
			//sprintf( textString, "%s\n%s",textString, sea->debugdata);
			sprintf( textString, "%stime_prepare: %i\ntotal time: %f\nframerate: %f Hz\n",textString,time_prepare, (double)time_total/64.0f,64000.0f/((float)(time_total)));
			//		sprintf( textString, "%sn_points: %i\n",textString,n_points);
			//		sprintf( textString, "%sx_min: %f x_max: %f y_min: %f y_max: %f ",textString,x_min,x_max,y_min,y_max);

			g_pd3dxFont->DrawText(NULL, textString,-1,&textpos,DT_NOCLIP,D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ));

			sprintf( textString, "%s\n", prm->get_display() );
			textpos.top = 200;
			g_pd3dxFont->DrawText(NULL, textString,-1,&textpos,DT_NOCLIP,D3DXCOLOR( 1.0f, 0.5f, 0.5f, 1.0f ));
			//		g_pd3dxFont->End();
		}

		// End the scene
		g_pd3dDevice->EndScene();
	}
	// Present the backbuffer contents to the display


	//LPDIRECT3DSWAPCHAIN9 SwapChain;
	//g_pd3dDevice->GetSwapChain( 0,&SwapChain);
	//SwapChain->Present( NULL, NULL, NULL, NULL, D3DPRESENT_LINEAR_CONTENT );			

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL );	
}



//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		return 0;
	case WM_KEYDOWN:							// Is A Key Being Held Down?
		{			
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			// preset handling
			if (keys['1'])
			{
				loadpreset(1);
				return 0;
			}
			if (keys['2'])
			{
				loadpreset(2);
				return 0;
			}
			if (keys['3'])
			{
				loadpreset(3);
				return 0;
			}
			if (keys['4'])
			{
				loadpreset(4);
				return 0;
			}
			if (keys['5'])
			{
				loadpreset(5);
				return 0;
			}
			if (keys['6'])
			{
				loadpreset(6);
				return 0;
			}
			if (keys['T'])
			{
				text = !text;
				return 0;
			}
			// other keys
			if (keys[VK_TAB])
			{
				active_camera_mouse = (active_camera_mouse+1)%n_cameras;
				camera_mouse = cameras[active_camera_mouse];
				return 0;
			}
			if (keys['C'])
			{
				active_camera_view = (active_camera_view+1)%n_cameras;
				camera_view = cameras[active_camera_view];
				return 0;
			} 
			if (keys['E'])
			{
				sea->calc_efficiency();
				return 0;
			} 
			if (keys['0']) 
			{
				prm->value_reset();
				return 0;
			}

			if (keys[VK_PRIOR]){
				gridsize_x = gridsize_x<<1;
				sea->set_grid_size(gridsize_x,gridsize_y);
				return 0;
			}
			if (keys[VK_NEXT]){
				gridsize_x = gridsize_x>>1;
				if (gridsize_x<2) gridsize_x = 2;
				sea->set_grid_size(gridsize_x,gridsize_y);
				return 0;
			}
			if (keys[VK_HOME]){
				gridsize_y = gridsize_y<<1;
				sea->set_grid_size(gridsize_x,gridsize_y);
				return 0;
			}
			if (keys[VK_END]){
				gridsize_y = gridsize_y>>1;
				if (gridsize_y<2) gridsize_y = 2;
				sea->set_grid_size(gridsize_x,gridsize_y);
				return 0;
			}
			if (keys[VK_SUBTRACT])
			{
				prm->value_decrease();
				return 0;
			}
			if (keys[VK_ADD])
			{
				prm->value_increase();				
				return 0;
			}

			if (keys[VK_LEFT])
			{
				prm->value_decreaseXL();
				return 0;
			}
			if (keys[VK_RIGHT])
			{
				prm->value_increaseXL();
				return 0;
			}
			if (keys[VK_UP])
			{
				prm->previous_parameter();
				return 0;
			}
			if (keys[VK_DOWN])
			{
				prm->next_parameter();
				return 0;
			}			

			return 0;								// Jump Back
		}

	case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	// Register the window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Tutorial", NULL };
	RegisterClassEx( &wc );

	// Create the application's window
	HWND hWnd = CreateWindow( "D3D Tutorial", "Projected grid concept demo",
		WS_OVERLAPPEDWINDOW, 100, 100, 1024, 768,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );
	// init mouse
	mouse = new dxmouse(hWnd,hInst);
	mouse->Init();

	// Initialize Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		// Create the scene geometry
		if( SUCCEEDED( InitGeometry() ) )
		{
			// Show the window
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

			// Enter the message loop
			MSG msg;
			ZeroMemory( &msg, sizeof(msg) );
			while( msg.message!=WM_QUIT )
			{
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
					Render();
			}
		}
	}

	UnregisterClass( "D3D Tutorial", wc.hInstance );
	return 0;
}



//-----------------------------------------------------------------------------
// Name: createFont()
// Desc: 
//-----------------------------------------------------------------------------
void createFont( void )
{
	//
	// To create a Windows friendly font using only a point size, an 
	// application must calculate the logical height of the font.
	// 
	// This is because functions like CreateFont() and CreateFontIndirect() 
	// only use logical units to specify height.
	//
	// Here's the formula to find the height in logical pixels:
	//
	//             -( point_size * LOGPIXELSY )
	//    height = ----------------------------
	//                          72
	//

	HRESULT hr;
	HDC hDC;
	HFONT hFont;
	int nHeight;
	int nPointSize = 7;
	char strFontName[] = "Verdana";

	hDC = GetDC( NULL );

	nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );


	hFont = CreateFont( nHeight, 
		0, 0, 0,
		FW_DONTCARE,
		false, false, false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		strFontName );

	if( hFont != NULL )
	{

		//if( FAILED( D3DXCreateFont( g_pd3dDevice, hFont, &g_pd3dxFont ) ) )
		if( FAILED( D3DXCreateFont( g_pd3dDevice,nHeight, 5, 1, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, strFontName, &g_pd3dxFont ) ) )
		{
			MessageBox(NULL,"Call to D3DXCreateFont failed!", "ERROR",MB_OK|MB_ICONEXCLAMATION);
		}


		DeleteObject( hFont );
	}
	else
	{
		MessageBox(NULL,"Call to CreateFont failed!", "ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

}

