#pragma once

#include <afxwin.h>

#define _USE_MATH_DEFINES
#include <math.h>

// vec2 class -----------------------------------------------------------------------------------------------------------------

class vec2
{
public:
	union{
		struct{float x, y;};
		struct{float s, t;};
		struct{float r, g;};
	};
	vec2() : x(0.0f), y(0.0f){}
	~vec2(){}
	vec2(float num) : x(num), y(num){}
	vec2(float x, float y) : x(x), y(y){}
	vec2(vec2 &u) : x(u.x), y(u.y){}
	vec2& operator = (vec2 &u){x = u.x; y = u.y; return *this;}
	vec2 operator - (){return vec2(-x, -y);}
	float* operator & (){return (float*)this;};
	vec2& operator += (float num){x += num; y += num; return *this;}
	vec2& operator += (vec2 &u){x += u.x; y += u.y; return *this;}
	vec2& operator -= (float num){x -= num; y -= num; return *this;}
	vec2& operator -= (vec2 &u){x -= u.x; y -= u.y; return *this;}
	vec2& operator *= (float num){x *= num; y *= num; return *this;}
	vec2& operator *= (vec2 &u){x *= u.x; y *= u.y; return *this;}
	vec2& operator /= (float num){x /= num; y /= num; return *this;}
	vec2& operator /= (vec2 &u){x /= u.x; y /= u.y; return *this;}
	friend vec2 operator + (vec2 &u, float num){return vec2(u.x + num, u.y + num);}
	friend vec2 operator + (float num, vec2 &u){return vec2(num + u.x, num + u.y);}
	friend vec2 operator + (vec2 &u, vec2 &v){return vec2(u.x + v.x, u.y + v.y);}
	friend vec2 operator - (vec2 &u, float num){return vec2(u.x - num, u.y - num);}
	friend vec2 operator - (float num, vec2 &u){return vec2(num - u.x, num - u.y);}
	friend vec2 operator - (vec2 &u, vec2 &v){return vec2(u.x - v.x, u.y - v.y);}
	friend vec2 operator * (vec2 &u, float num){return vec2(u.x * num, u.y * num);}
	friend vec2 operator * (float num, vec2 &u){return vec2(num * u.x, num * u.y);}
	friend vec2 operator * (vec2 &u, vec2 &v){return vec2(u.x * v.x, u.y * v.y);}
	friend vec2 operator / (vec2 &u, float num){return vec2(u.x / num, u.y / num);}
	friend vec2 operator / (float num, vec2 &u){return vec2(num / u.x, num / u.y);}
	friend vec2 operator / (vec2 &u, vec2 &v){return vec2(u.x / v.x, u.y / v.y);}
};

// vec3 class -----------------------------------------------------------------------------------------------------------------

class vec3
{
public:
	union{
		struct{float x, y, z;};
		struct{float s, t, p;};
		struct{float r, g, b;};
	};
	vec3() : x(0.0f), y(0.0f), z(0.0f){}
	~vec3(){}
	vec3(float num) : x(num), y(num), z(num){}
	vec3(float x, float y, float z) : x(x), y(y), z(z){}
	vec3(vec2 &u, float z) : x(u.x), y(u.y), z(z){}
	vec3(vec3 &u) : x(u.x), y(u.y), z(u.z){}
	vec3& operator = (vec3 &u){x = u.x; y = u.y; z = u.z; return *this;}
	vec3 operator - (){return vec3(-x, -y, -z);}
	float* operator & (){return (float*)this;}
	vec3& operator += (float num){x += num; y += num; z += num; return *this;}
	vec3& operator += (vec3 &u){x += u.x; y += u.y; z += u.z; return *this;}
	vec3& operator -= (float num){x -= num; y -= num; z -= num; return *this;}
	vec3& operator -= (vec3 &u){x -= u.x; y -= u.y; z -= u.z; return *this;}
	vec3& operator *= (float num){x *= num; y *= num; z *= num; return *this;}
	vec3& operator *= (vec3 &u){x *= u.x; y *= u.y; z *= u.z; return *this;}
	vec3& operator /= (float num){x /= num; y /= num; z /= num; return *this;}
	vec3& operator /= (vec3 &u){x /= u.x; y /= u.y; z /= u.z; return *this;}
	friend vec3 operator + (vec3 &u, float num){return vec3(u.x + num, u.y + num, u.z + num);}
	friend vec3 operator + (float num, vec3 &u){return vec3(num + u.x, num + u.y, num + u.z);}
	friend vec3 operator + (vec3 &u, vec3 &v){return vec3(u.x + v.x, u.y + v.y, u.z + v.z);}
	friend vec3 operator - (vec3 &u, float num){return vec3(u.x - num, u.y - num, u.z + num);}
	friend vec3 operator - (float num, vec3 &u){return vec3(num - u.x, num - u.y, num - u.z);}
	friend vec3 operator - (vec3 &u, vec3 &v){return vec3(u.x - v.x, u.y - v.y, u.z - v.z);}
	friend vec3 operator * (vec3 &u, float num){return vec3(u.x * num, u.y * num, u.z * num);}
	friend vec3 operator * (float num, vec3 &u){return vec3(num * u.x, num * u.y, num * u.z);}
	friend vec3 operator * (vec3 &u, vec3 &v){return vec3(u.x * v.x, u.y * v.y, u.z * v.z);}
	friend vec3 operator / (vec3 &u, float num){return vec3(u.x / num, u.y / num, u.z / num);}
	friend vec3 operator / (float num, vec3 &u){return vec3(num / u.x, num / u.y, num / u.z);}
	friend vec3 operator / (vec3 &u, vec3 &v){return vec3(u.x / v.x, u.y / v.y, u.z / v.z);}
};

// vec4 class -----------------------------------------------------------------------------------------------------------------

class vec4
{
public:
	union{
		struct{float x, y, z, w;};
		struct{float s, t, p, q;};
		struct{float r, g, b, a;};
	};
	vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f){}
	~vec4(){}
	vec4(float num) : x(num), y(num), z(num), w(num){}
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w){}
	vec4(vec2 &u, float z, float w) : x(u.x), y(u.y), z(z), w(w){}
	vec4(vec3 &u, float w) : x(u.x), y(u.y), z(u.z), w(w){}
	vec4(vec4 &u) : x(u.x), y(u.y), z(u.z), w(u.w){}
	vec4& operator = (vec4 &u){x = u.x; y = u.y; z = u.z; w = u.w; return *this;}
	vec4 operator - (){return vec4(-x, -y, -z, -w);}
	float* operator & (){return (float*)this;}
	vec4& operator += (float num){x += num; y += num; z += num; w += num; return *this;}
	vec4& operator += (vec4 &u){x += u.x; y += u.y; z += u.z; w += u.w; return *this;}
	vec4& operator -= (float num){x -= num; y -= num; z -= num; w -= num; return *this;}
	vec4& operator -= (vec4 &u){x -= u.x; y -= u.y; z -= u.z; w -= u.w; return *this;}
	vec4& operator *= (float num){x *= num; y *= num; z *= num; w *= num; return *this;}
	vec4& operator *= (vec4 &u){x *= u.x; y *= u.y; z *= u.z; w *= u.w; return *this;}
	vec4& operator /= (float num){x /= num; y /= num; z /= num; w /= num; return *this;}
	vec4& operator /= (vec4 &u){x /= u.x; y /= u.y; z /= u.z; w /= u.w; return *this;}
	friend vec4 operator + (vec4 &u, float num){return vec4(u.x + num, u.y + num, u.z + num, u.w + num);}
	friend vec4 operator + (float num, vec4 &u){return vec4(num + u.x, num + u.y, num + u.z, num + u.w);}
	friend vec4 operator + (vec4 &u, vec4 &v){return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);}
	friend vec4 operator - (vec4 &u, float num){return vec4(u.x - num, u.y - num, u.z - num, u.w - num);}
	friend vec4 operator - (float num, vec4 &u){return vec4(num - u.x, num - u.y, num - u.z, num - u.w);}
	friend vec4 operator - (vec4 &u, vec4 &v){return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);}
	friend vec4 operator * (vec4 &u, float num){return vec4(u.x * num, u.y * num, u.z * num, u.w * num);}
	friend vec4 operator * (float num, vec4 &u){return vec4(num * u.x, num * u.y, num * u.z, num * u.w);}
	friend vec4 operator * (vec4 &u, vec4 &v){return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);}
	friend vec4 operator / (vec4 &u, float num){return vec4(u.x / num, u.y / num, u.z / num, u.w / num);}
	friend vec4 operator / (float num, vec4 &u){return vec4(num / u.x, num / u.y, num / u.z, num / u.w);}
	friend vec4 operator / (vec4 &u, vec4 &v){return vec4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w);}
};

// mat4x4 class ---------------------------------------------------------------------------------------------------------------

class mat4x4
{
public:
	float M[16];
	mat4x4();
	~mat4x4();
	mat4x4(mat4x4 &Matrix);
	mat4x4& operator = (mat4x4 &Matrix);
	float& operator [] (int Index);
	float* operator & ();
	friend mat4x4 operator * (mat4x4 &Matrix1, mat4x4 &Matrix2);
	friend vec4 operator * (mat4x4 &Matrix, vec4 &Vector);
};

// 2D vector functions --------------------------------------------------------------------------------------------------------

float dot(vec2 &u, vec2 &v);
float length(vec2 &u);
vec2 normalize(vec2 &u);
vec2 reflect(vec2 &i, vec2 &n);
vec2 refract(vec2 &i, vec2 &n, float eta);
vec2 rotate(vec2 &u, float angle);

// 3D vector functions --------------------------------------------------------------------------------------------------------

vec3 cross(vec3 &u, vec3 &v);
float dot(vec3 &u, vec3 &v);
float length(vec3 &u);
vec3 normalize(vec3 &u);
vec3 reflect(vec3 &i, vec3 &n);
vec3 refract(vec3 &i, vec3 &n, float eta);
vec3 rotate(vec3 &u, vec3 &v, float angle);

// matrix functions -----------------------------------------------------------------------------------------------------------

mat4x4 BiasMatrix();
mat4x4 BiasMatrixInverse();
mat4x4 ViewMatrix(vec3 &X, vec3 &Y, vec3 &Z, vec3 &Position);
mat4x4 ViewMatrixInverse(mat4x4 &VM);
mat4x4 OrthogonalProjectionMatrix(float left, float right, float bottom, float top, float n, float f);
mat4x4 PerspectiveProjectionMatrix(float fovy, float x, float y, float n, float f);
mat4x4 PerspectiveProjectionMatrixInverse(mat4x4 &PPM);
mat4x4 RotationMatrix(vec3 &Vector, float angle);
mat4x4 ScaleMatrix(float x, float y, float z);
mat4x4 TranslationMatrix(float x, float y, float z);
