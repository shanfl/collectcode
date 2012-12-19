#include "camera.h"

camera::camera(){
	position = D3DXVECTOR3(0,0,0);
	fov = 45;
	aspect = 1.0;
	znear = 0.1;
	zfar = 10;
	rotx = 0;
	roty = 0;
	rotz = 0;
	this->update();
}

camera::camera(D3DXVECTOR3 pos, float rotx, float roty, float rotz, float fov, float aspect, float nearz, float farz){
	this->position = pos;
	this->fov = fov;
	this->aspect = aspect;
	this->znear = nearz;
	this->zfar = farz;
	this->rotx = rotx;
	this->roty = roty;
	this->rotz = rotz;
	this->update();
}

camera::camera(const camera *src){
	this->position	= src->position;
	this->fov		= src->fov;
	this->aspect	= src->aspect;
	this->znear		= src->znear;
	this->zfar		= src->zfar;
	this->rotx		= src->rotx;
	this->roty		= src->roty;
	this->rotz		= src->rotz;
	this->update();
}

camera::~camera(){
	// nothing funny here
}

void camera::update(){
	D3DXMATRIXA16 rotatex,rotatey,rotatez,translation;
	// perspective matrix
	D3DXMatrixPerspectiveFovLH(&proj, fov, aspect, znear, zfar);
	// view matrix
	D3DXMatrixRotationX(&rotatex,rotx);
	D3DXMatrixRotationY(&rotatey,roty);
	D3DXMatrixRotationZ(&rotatez,rotz);
	D3DXMatrixTranslation(&translation,-position.x,-position.y,-position.z);
	view = translation * rotatey*rotatex*rotatez;
	// and finally the combined viewproj
	viewproj = view*proj;
	// and all the inverses
	D3DXMatrixInverse(&invproj,NULL,&proj);
	D3DXMatrixInverse(&invview,NULL,&view);
	D3DXMatrixInverse(&invviewproj,NULL,&viewproj);
	// and the direction vectors
	D3DXVec3TransformNormal(&forward,&D3DXVECTOR3(0,0,1),&invview);
	D3DXVec3TransformNormal(&up,&D3DXVECTOR3(0,1,0),&invview);
	D3DXVec3TransformNormal(&right,&D3DXVECTOR3(1,0,0),&invview);
}

// set all parameters assuming position, forward & all the perspective shit are correct
void camera::update_lookat(){
	// perspective matrix
	D3DXMatrixPerspectiveFovLH(&proj, fov, aspect, znear, zfar);
	// view matrix
	D3DXMatrixLookAtLH( &view, &position,&(position+forward), &D3DXVECTOR3(0,1,0));

	// and finally the combined viewproj
	viewproj = view*proj;
	// and all the inverses
	D3DXMatrixInverse(&invproj,NULL,&proj);
	D3DXMatrixInverse(&invview,NULL,&view);
	D3DXMatrixInverse(&invviewproj,NULL,&viewproj);
	D3DXVec3TransformNormal(&right,&D3DXVECTOR3(1,0,0),&invview);
}