#ifndef _c_camera_
#define _c_camera_

#include <d3dx9.h>

class camera{
public:
	camera();
	camera(const camera *src);
	camera(D3DXVECTOR3 pos, float rotx, float roty, float rotz, float fov, float aspect, float nearz, float farz);
	~camera();
	void update();
	void update_lookat();
	D3DXVECTOR3 position;
	D3DXVECTOR3 forward,up,right;
	float fov, aspect,znear, zfar, rotx, roty, rotz;
	D3DXMATRIXA16 view, invview, proj, invproj, viewproj, invviewproj;
};

#endif