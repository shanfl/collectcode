#include "tools.h"

float gaussian(float x, float d)
{
	return exp(-x*x/(2*d*d))/sqrt(2*PI*d);
}

struct DVERTEX
{
	D3DXVECTOR3	position;
	float u,v;
};

#define D3DFVF_DVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)


// render a small texture overlay
void debug_render_quad(const LPDIRECT3DDEVICE9 device, const LPDIRECT3DTEXTURE9 texture, int n)
{
	LPDIRECT3DVERTEXBUFFER9		quad;

	// Create the quad vertex buffer
	
	device->CreateVertexBuffer( 6*sizeof(DVERTEX),
		0, D3DFVF_DVERTEX,
		D3DPOOL_DEFAULT, &quad, NULL );
	
	DVERTEX *qV;

	int ix = n%4,
		iy = n/4;

	float	fx = (ix / 4.0f)*2.0f - 1.0f,
			fy = (iy / 4.0f)*2.0f - 1.0f,
			d = 2.0f / 4.0f;


	if( !FAILED( quad->Lock( 0, 0, (void**)&qV, 0 ) ) ){
	
		qV[0].position = D3DXVECTOR3(fx,	fy,		0);
		qV[1].position = D3DXVECTOR3(fx+d,	fy,		0);
		qV[2].position = D3DXVECTOR3(fx,	fy+d,	0);
		qV[3].position = D3DXVECTOR3(fx,	fy+d,	0);
		qV[4].position = D3DXVECTOR3(fx+d,	fy,		0);
		qV[5].position = D3DXVECTOR3(fx+d,	fy+d,	0);		

		qV[0].u = 0;	qV[0].v = 0;
		qV[1].u = +1;	qV[1].v = 0;
		qV[2].u = 0;	qV[2].v = +1;
		qV[3].u = 0;	qV[3].v = +1;
		qV[4].u = +1;	qV[4].v = 0;
		qV[5].u = +1;	qV[5].v = +1;

		quad->Unlock();
	}
	
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	
	device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	device->SetFVF(D3DFVF_DVERTEX);
	device->SetStreamSource( 0, quad, 1, sizeof(DVERTEX) );
	device->SetTexture( 0, texture);

	D3DXMATRIXA16 id;
	D3DXMatrixIdentity( &id );
	device->SetTransform( D3DTS_VIEW, &id );
	device->SetTransform( D3DTS_PROJECTION, &id );
	
	device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);

	quad->Release();
	device->SetTexture( 0, NULL);
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	
	//device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

}