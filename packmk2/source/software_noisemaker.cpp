#include "software_noisemaker.h"
#include <xmmintrin.h>
#include <math.h>
#include "tools.h"

#define SSE false	// SSE is broken atm
#define quadpipe false
#define packednoise true

software_noisemaker::software_noisemaker(int sX, int sY, parameterhandler *prm, const LPDIRECT3DDEVICE9 dev)
{
	this->device = dev;
	this->sizeX = sX;
	this->sizeY = sY;
	this->prm = prm;
	time = 0.0;
	last_time = timeGetTime();
	octaves = 0;	// don't want to have the noise accessed before it's calculated

	f_sizeX = (float) sizeX;
	f_sizeY = (float) sizeY;	

	// reset normals
	vertices	= new SOFTWARESURFACEVERTEX[sizeX*sizeY];	
	for(int v=0; v<sizeY; v++)
	{
		for(int u=0; u<sizeX; u++)
		{
			vertices[v*sizeX + u].nx =	0.0f;
			vertices[v*sizeX + u].ny =	1.0f;
			vertices[v*sizeX + u].nz =	0.0f;
			vertices[v*sizeX + u].tu = (float)u/(sizeX-1);
			vertices[v*sizeX + u].tv = (float)v/(sizeY-1);
		}
	}	
	this->init_noise();	
	#ifndef CPU_NORMALS
	this->load_effects();
	this->init_textures();
	#endif
}

void software_noisemaker::resize(int sX, int sY)
{
	delete vertices;
	this->sizeX = sX;
	this->sizeY = sY;

	f_sizeX = (float) sizeX;
	f_sizeY = (float) sizeY;

	// reset normals
	delete vertices;
	vertices	= new SOFTWARESURFACEVERTEX[sizeX*sizeY];	
	for(int v=0; v<sizeY; v++)
	{
		for(int u=0; u<sizeX; u++)
		{
			vertices[v*sizeX + u].nx =	0.0f;
			vertices[v*sizeX + u].ny =	1.0f;
			vertices[v*sizeX + u].nz =	0.0f;
			vertices[v*sizeX + u].tu = (float)u/(sizeX-1);
			vertices[v*sizeX + u].tv = (float)v/(sizeY-1);
		}
	}
}

void software_noisemaker::init_noise()
{	
	// create noise (uniform)
	float tempnoise[n_size_sq*noise_frames];
	for(int i=0; i<(n_size_sq*noise_frames); i++)
	{
		//this->noise[i] = rand()&0x0000FFFF;		
		float temp = (float) rand()/RAND_MAX;		
		tempnoise[i] = 4*(temp - 0.5f);	
	}	

	for(int frame=0; frame<noise_frames; frame++)
	{
		for(int v=0; v<n_size; v++)
		{
			for(int u=0; u<n_size; u++)
			{	
				/*float temp = 0.25f * (tempnoise[frame*n_size_sq + v*n_size + u] +
									  tempnoise[frame*n_size_sq + v*n_size + ((u+1)&n_size_m1)] + 
									  tempnoise[frame*n_size_sq + ((v+1)&n_size_m1)*n_size + u] +
									  tempnoise[frame*n_size_sq + ((v+1)&n_size_m1)*n_size + ((u+1)&n_size_m1)]);*/
				int v0 = ((v-1)&n_size_m1)*n_size,
					v1 = v*n_size,
					v2 = ((v+1)&n_size_m1)*n_size,
					u0 = ((u-1)&n_size_m1),
					u1 = u,
					u2 = ((u+1)&n_size_m1),					
					f  = frame*n_size_sq;
				float temp = (1.0f/14.0f) * (	tempnoise[f + v0 + u0] + tempnoise[f + v0 + u1] + tempnoise[f + v0 + u2] +
										tempnoise[f + v1 + u0] + 6.0f*tempnoise[f + v1 + u1] + tempnoise[f + v1 + u2] +
										tempnoise[f + v2 + u0] + tempnoise[f + v2 + u1] + tempnoise[f + v2 + u2]);
									  
				this->noise[frame*n_size_sq + v*n_size + u] = noise_magnitude*temp;
			}
		}
	}	
	
}

void software_noisemaker::calc_noise()
{
	octaves = min(prm->params[p_iOctaves].iData, max_octaves);		

	// calculate the strength of each octave
	float sum=0.0f;
	for(int i=0; i<octaves; i++)
	{
		f_multitable[i] = powf(prm->get_float(p_fFalloff),1.0f*i);
		sum += f_multitable[i];
	}

	{
	for(int i=0; i<octaves; i++)
	{
		f_multitable[i] /= sum;
	}}
	
	{
	for(int i=0; i<octaves; i++)
	{
		multitable[i] = scale_magnitude*f_multitable[i];
	}}
	

	DWORD this_time = timeGetTime();
	double itime = this_time - last_time;
	static double lp_itime=0.0;	
	last_time = this_time;
	itime *= 0.001 * prm->get_float( p_fAnimspeed );
	lp_itime = 0.99*lp_itime + 0.01 * itime;
	if ( !prm->get_bool(p_bPaused) )
		time += lp_itime;			

	
	double	r_timemulti = 1.0;

	for(int o=0; o<octaves; o++)
	{		
		unsigned int image[3];
		int amount[3];
		double dImage, fraction = modf(time*r_timemulti,&dImage);
		int iImage = (int)dImage;
		amount[0] = scale_magnitude*f_multitable[o]*(pow(sin((fraction+2)*PI/3),2)/1.5);
		amount[1] = scale_magnitude*f_multitable[o]*(pow(sin((fraction+1)*PI/3),2)/1.5);
		amount[2] = scale_magnitude*f_multitable[o]*(pow(sin((fraction)*PI/3),2)/1.5);
		image[0] = (iImage) & noise_frames_m1;
		image[1] = (iImage+1) & noise_frames_m1;
		image[2] = (iImage+2) & noise_frames_m1;
		{	
			for(int i=0; i<n_size_sq; i++)
			{
				o_noise[i + n_size_sq*o] =	(	((amount[0] * noise[i + n_size_sq * image[0]])>>scale_decimalbits) + 
												((amount[1] * noise[i + n_size_sq * image[1]])>>scale_decimalbits) + 
												((amount[2] * noise[i + n_size_sq * image[2]])>>scale_decimalbits));
			}
		}

		r_timemulti *= prm->get_float( p_fTimemulti );
	}

	if(packednoise)
	{
		int octavepack = 0;
		for(int o=0; o<octaves; o+=n_packsize)
		{
			for(int v=0; v<np_size; v++)
			for(int u=0; u<np_size; u++)
			{
				p_noise[v*np_size+u+octavepack*np_size_sq] = o_noise[(o+3)*n_size_sq + (v&n_size_m1)*n_size + (u&n_size_m1)];
				p_noise[v*np_size+u+octavepack*np_size_sq] += mapsample( u, v, 3, o);
				p_noise[v*np_size+u+octavepack*np_size_sq] += mapsample( u, v, 2, o+1);
				p_noise[v*np_size+u+octavepack*np_size_sq] += mapsample( u, v, 1, o+2);				
			}
			octavepack++;

			/*for(int v=0; v<20; v++)
			for(int u=0; u<20; u++)
				p_noise[v*np_size+u] = 1000;*/
			// debug box
			
		}
	}
}

inline int software_noisemaker::mapsample(int u, int v, int upsamplepower, int octave)
{
	int magnitude = 1<<upsamplepower;
	int pu = u >> upsamplepower;
	int pv = v >> upsamplepower;	
	int fu = u & (magnitude-1);
	int fv = v & (magnitude-1);
	int fu_m = magnitude - fu;
	int fv_m = magnitude - fv;

	int o = fu_m*fv_m*o_noise[octave*n_size_sq + ((pv)&n_size_m1)*n_size + ((pu)&n_size_m1)] +
			fu*fv_m*o_noise[octave*n_size_sq + ((pv)&n_size_m1)*n_size + ((pu+1)&n_size_m1)] +
			fu_m*fv*o_noise[octave*n_size_sq + ((pv+1)&n_size_m1)*n_size + ((pu)&n_size_m1)] +
			fu*fv*o_noise[octave*n_size_sq + ((pv+1)&n_size_m1)*n_size + ((pu+1)&n_size_m1)];

	return o >> (upsamplepower+upsamplepower);
}

software_noisemaker::~software_noisemaker()
{
	delete [] vertices;
}

bool software_noisemaker::render_geometry(const D3DXMATRIXA16 *m)
{

	this->calc_noise();

	float magnitude = n_dec_magn * prm->get_float(p_fScale);
	float inv_magnitude_sq = 1.0f/(prm->get_float(p_fScale)*prm->get_float(p_fScale));

	D3DXMATRIXA16 m_inv;
	D3DXMatrixInverse( &m_inv, NULL, m );
	D3DXVec3TransformNormal( &e_u, &D3DXVECTOR3(1,0,0), m);
	D3DXVec3TransformNormal( &e_v, &D3DXVECTOR3(0,1,0), m);
	D3DXVec3Normalize( &e_u, &e_u );
	D3DXVec3Normalize( &e_v, &e_v );


	t_corners0 = this->calc_worldpos(D3DXVECTOR2(0.0f,0.0f),m);
	t_corners1 = this->calc_worldpos(D3DXVECTOR2(+1.0f,0.0f),m);
	t_corners2 = this->calc_worldpos(D3DXVECTOR2(0.0f,+1.0f),m);
	t_corners3 = this->calc_worldpos(D3DXVECTOR2(+1.0f,+1.0f),m);

	D3DXMATRIXA16 surface_to_world;


	float	du = 1.0f/float(sizeX-1),
		dv = 1.0f/float(sizeY-1),
		u,v=0.0f;
	D3DXVECTOR4 result;
	int i=0;
	for(int iv=0; iv<sizeY; iv++)
	{
		u = 0.0f;		
		for(int iu=0; iu<sizeX; iu++)
		{				

			//result = (1.0f-v)*( (1.0f-u)*t_corners0 + u*t_corners1 ) + v*( (1.0f-u)*t_corners2 + u*t_corners3 );				
			result.x = (1.0f-v)*( (1.0f-u)*t_corners0.x + u*t_corners1.x ) + v*( (1.0f-u)*t_corners2.x + u*t_corners3.x );				
			result.z = (1.0f-v)*( (1.0f-u)*t_corners0.z + u*t_corners1.z ) + v*( (1.0f-u)*t_corners2.z + u*t_corners3.z );				
			result.w = (1.0f-v)*( (1.0f-u)*t_corners0.w + u*t_corners1.w ) + v*( (1.0f-u)*t_corners2.w + u*t_corners3.w );				

			float divide = 1.0f/result.w;				
			result.x *= divide;
			result.z *= divide;

			vertices[i].x = result.x;
			vertices[i].z = result.z;
			//vertices[i].y = get_height_at(magnitude*result.x, magnitude*result.z, octaves);
			vertices[i].y = get_height_dual(magnitude*result.x, magnitude*result.z );

			i++;
			u += du;
		}
		v += dv;			
	}

	// smooth the heightdata
	if(prm->params[p_bSmooth].bData)
	{
		//for(int n=0; n<3; n++)
		for(int v=1; v<(sizeY-1); v++)
		{
			for(int u=1; u<(sizeX-1); u++)
			{				
				vertices[v*sizeX + u].y =	0.2f * (vertices[v*sizeX + u].y +
					vertices[v*sizeX + (u+1)].y + 
					vertices[v*sizeX + (u-1)].y + 
					vertices[(v+1)*sizeX + u].y + 
					vertices[(v-1)*sizeX + u].y);															
			}
		}
	}

	if(!prm->params[p_bDisplace].bData)
	{
		// reset height to 0
		for(int u=0; u<(sizeX*sizeY); u++)
		{
			vertices[u].y = 0;
		}

	}


	#ifdef CPU_NORMALS
	calc_normals();	
	#else
	this->upload_noise();
	#endif	

	return true;
}



// check the point of intersection with the plane (0,1,0,0) and return the position in homogenous coordinates 
D3DXVECTOR4 software_noisemaker::calc_worldpos(D3DXVECTOR2 uv, const D3DXMATRIXA16 *m)
{	
	// this is hacky.. this does take care of the homogenous coordinates in a correct way, 
	// but only when the plane lies at y=0
	D3DXVECTOR4	origin(uv.x,uv.y,-1,1);
	D3DXVECTOR4	direction(uv.x,uv.y,1,1);

	D3DXVec4Transform( &origin, &origin, m );
	D3DXVec4Transform( &direction, &direction, m );
	direction -= origin;    

	float	l = -origin.y / direction.y;	// assumes the plane is y=0

	D3DXVECTOR4 worldPos = origin + direction*l;    
	return worldPos;
}

void software_noisemaker::calc_normals()
{
	for(int v=1; v<(sizeY-1); v++)
	{
		for(int u=1; u<(sizeX-1); u++)
		{
			D3DXVECTOR3 vec1(	vertices[v*sizeX + u + 1].x-vertices[v*sizeX + u - 1].x,
				vertices[v*sizeX + u + 1].y-vertices[v*sizeX + u - 1].y, 
				vertices[v*sizeX + u + 1].z-vertices[v*sizeX + u - 1].z);

			D3DXVECTOR3 vec2(	vertices[(v+1)*sizeX + u].x - vertices[(v-1)*sizeX + u].x,
				vertices[(v+1)*sizeX + u].y - vertices[(v-1)*sizeX + u].y,
				vertices[(v+1)*sizeX + u].z - vertices[(v-1)*sizeX + u].z);
			D3DXVECTOR3 normal;
			D3DXVec3Cross( &normal, &vec2, &vec1 );
			vertices[v*sizeX + u].nx = normal.x;
			vertices[v*sizeX + u].ny = normal.y;
			vertices[v*sizeX + u].nz = normal.z;

		}
	}
}

inline int software_noisemaker::readtexel_linear(int u, int v, int offset)
{
	int iu, iup, iv, ivp, fu, fv;
	iu = (u>>n_dec_bits)&n_size_m1;
	iv = ((v>>n_dec_bits)&n_size_m1)*n_size;

	iup = (((u>>n_dec_bits) + 1)&n_size_m1);
	ivp = (((v>>n_dec_bits) + 1)&n_size_m1)*n_size;

	fu = u & n_dec_magn_m1;
	fv = v & n_dec_magn_m1;
	/*float f_fu = (float) fu / n_dec_magn;
	float f_fv = (float) fv / n_dec_magn;*/
	/*float ut01 = ((n_dec_magn_m1-fu)*o_noise[iv + iu + offset] + fu*o_noise[iv + iup + offset]);
	float ut23 = ((n_dec_magn_m1-fu)*o_noise[ivp + iu + offset] + fu*o_noise[ivp + iup + offset]);*/

	int ut01 = ((n_dec_magn-fu)*o_noise[offset + iv + iu] + fu*o_noise[offset + iv + iup]) >> n_dec_bits;
	int ut23 = ((n_dec_magn-fu)*o_noise[offset + ivp + iu] + fu*o_noise[offset + ivp + iup]) >> n_dec_bits ;
	int ut = ((n_dec_magn-fv)*ut01 + fv*ut23) >> n_dec_bits;
	return ut;
}


inline float software_noisemaker::get_height_at(int u, int v, int octaves)
{	
	int value=0;	
	//r_noise = o_noise;	// pointer to the current noise source octave
	for(int i=0; i<octaves; i++)
	{		
		value += readtexel_linear(u,v,i*n_size_sq);
		u = u << 1;
		v = v << 1;
		//r_noise += n_size_sq;
	}		
	return (float)(value)*prm->params[p_fStrength].fData / noise_magnitude;
}

float software_noisemaker::get_height_at(float u, float v)
{
	float magnitude = n_dec_magn * prm->get_float(p_fScale);
	//return get_height_at(magnitude*u, magnitude*v, octaves);
	return get_height_dual(magnitude*u, magnitude*v);
}

void software_noisemaker::init_textures()
{
	// the noise textures. currently two of them (= 8 levels)
	device->CreateTexture(np_size,np_size,0,D3DUSAGE_DYNAMIC, D3DFMT_L16, D3DPOOL_DEFAULT, &(this->packed_noise_texture[0]),NULL);	
	device->CreateTexture(np_size,np_size,0,D3DUSAGE_DYNAMIC, D3DFMT_L16, D3DPOOL_DEFAULT, &(this->packed_noise_texture[1]),NULL);

	device->CreateTexture(nmapsize_x,nmapsize_y,1,D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &(this->heightmap),NULL);	
	device->CreateTexture(nmapsize_x,nmapsize_y,1,D3DUSAGE_AUTOGENMIPMAP|D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &(this->normalmap),NULL);		

	/*device->CreateTexture(nmapsize_x,nmapsize_y,1,D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &(this->heightmap),NULL);	
	device->CreateTexture(nmapsize_x,nmapsize_y,1,D3DUSAGE_AUTOGENMIPMAP|D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &(this->normalmap),NULL);		*/

	// create z/stencil-buffer
	device->CreateDepthStencilSurface( nmapsize_x, nmapsize_y,D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &depthstencil, NULL );
}

void software_noisemaker::upload_noise()
{
	D3DLOCKED_RECT locked;
	unsigned short *data;
	int tempdata[np_size_sq];
	for(int t=0; t<2; t++)
	{
		int offset = np_size_sq*t;
		// upload the first level
		packed_noise_texture[t]->LockRect( 0, &locked, NULL, D3DLOCK_DISCARD );
		data = (unsigned short*)locked.pBits;
		for(int i=0; i<np_size_sq; i++)
			data[i] = 32768+p_noise[i+offset];
		packed_noise_texture[t]->UnlockRect( 0 );

		int c = packed_noise_texture[t]->GetLevelCount();

		// calculate the second level, and upload it
		HRESULT hr = packed_noise_texture[t]->LockRect( 1, &locked, NULL, 0 );
		data = (unsigned short*)locked.pBits;		
		int sz = np_size>>1;
		for(int v=0; v<sz; v++){
			for(int u=0; u<sz; u++)
			{				
				tempdata[v*np_size + u] = (p_noise[((v<<1))*np_size + (u<<1)+offset] + p_noise[((v<<1))*np_size + (u<<1) + 1+offset] +
										   p_noise[((v<<1)+1)*np_size + (u<<1)+offset] + p_noise[((v<<1)+1)*np_size + (u<<1) + 1+offset])>>2;
				data[v*sz+u] = 32768+tempdata[v*np_size + u];
			}
		}

		packed_noise_texture[t]->UnlockRect( 1 );		
		
		for(int j=2; j<c; j++)
		{
			hr = packed_noise_texture[t]->LockRect( j, &locked, NULL, 0 );
			data = (unsigned short*)locked.pBits;
			int pitch = (locked.Pitch)>>1;
			sz = np_size>>j;			
			for(int v=0; v<sz; v++){
				for(int u=0; u<sz; u++)
				{
					tempdata[v*np_size + u] =	(tempdata[((v<<1))*np_size + (u<<1)] + tempdata[((v<<1))*np_size + (u<<1) + 1] +
												tempdata[((v<<1)+1)*np_size + (u<<1)] + tempdata[((v<<1)+1)*np_size + (u<<1) + 1])>>2;
					data[v*pitch+u] = 32768+tempdata[v*np_size + u];
				}
			}		
			packed_noise_texture[t]->UnlockRect( j );
		}
	}
}

void software_noisemaker::generate_normalmap( )
{
#ifndef CPU_NORMALS
	HRESULT hr;
	
	// do the heightmap thingy
	LPDIRECT3DSURFACE9 target,bb,old_depthstencil;
	hr = device->GetRenderTarget(0, &bb );
	hr = heightmap->GetSurfaceLevel( 0,&target );
	device->GetDepthStencilSurface( &old_depthstencil );	
	
	//hr = device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW  );	
	//device->SetStreamSource( 0, surf_software_vertices, 0, sizeof(SOFTWARESURFACEVERTEX) );
	hr = device->SetFVF( D3DFVF_SOFTWARESURFACEVERTEX);			
	//device->SetIndices(surf->surf_indicies);
	hr = hmap_effect->Begin(NULL,NULL);
	hmap_effect->Pass(0);				
	hmap_effect->SetFloat("scale", prm->params[p_fScale].fData);
	
	hmap_effect->SetTexture("noise0",packed_noise_texture[0]);
	hmap_effect->SetTexture("noise1",packed_noise_texture[1]);
	
	hr = device->SetRenderTarget( 0, target );
	device->SetDepthStencilSurface( depthstencil );
	//device->Clear( 0, NULL,D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,128,28), 1.0f, 0 );
	device->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	device->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST, 0,	0, sizeX*sizeY, 0, 2*(sizeX-1)*(sizeY-1) );			
	hmap_effect->End();
	
	// calculate normalmap

	hr = normalmap->GetSurfaceLevel( 0,&target );
	hr = device->SetRenderTarget( 0, target );
	hr = nmap_effect->Begin(NULL,NULL);
	nmap_effect->Pass(0);				
	nmap_effect->SetFloat("inv_mapsize_x", 1.0f/nmapsize_x);
	nmap_effect->SetFloat("inv_mapsize_y", 1.0f/nmapsize_y);
	nmap_effect->SetVector("corner00", &t_corners0 );
	nmap_effect->SetVector("corner01", &t_corners1 );
	nmap_effect->SetVector("corner10", &t_corners2 );
	nmap_effect->SetVector("corner11", &t_corners3 );
	nmap_effect->SetFloat("amplitude", 2*prm->params[p_fStrength].fData);
	nmap_effect->SetTexture("hmap",heightmap);
	device->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST, 0,	0, sizeX*sizeY, 0, 2*(sizeX-1)*(sizeY-1) );			
	nmap_effect->End();

	// restore the device
	device->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	device->SetRenderTarget( 0, bb );
	device->SetDepthStencilSurface( old_depthstencil );
#endif
}

void software_noisemaker::load_effects()
{
	char *errortext;
	LPD3DXBUFFER errors;
	D3DXHANDLE hTechnique;

	// load effect
	D3DXCreateEffectFromFile(device, "v2_heightmapgen.fx", 
		NULL, NULL, 0, NULL, &hmap_effect, &errors );

	if (errors != NULL){
		errortext = (char*) errors->GetBufferPointer();
		MessageBox(NULL, errortext, "hmap_effect", MB_OK);		
	}

	hmap_effect->FindNextValidTechnique(NULL, &hTechnique);    
	hmap_effect->SetTechnique(hTechnique);

	// load effect
	D3DXCreateEffectFromFile(device, "v2_normalmapgen.fx", 
		NULL, NULL, 0, NULL, &nmap_effect, &errors );

	if (errors != NULL){
		errortext = (char*) errors->GetBufferPointer();
		MessageBox(NULL, errortext, "nmap_effect", MB_OK);		
	}

	nmap_effect->FindNextValidTechnique(NULL, &hTechnique);    
	nmap_effect->SetTechnique(hTechnique);
}

