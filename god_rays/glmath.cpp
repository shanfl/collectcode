#include "glmath.h"

// mat4x4 class ---------------------------------------------------------------------------------------------------------------

mat4x4::mat4x4()
{
	M[0] = 1.0f; M[4] = 0.0f; M[8] = 0.0f; M[12] = 0.0f;
	M[1] = 0.0f; M[5] = 1.0f; M[9] = 0.0f; M[13] = 0.0f;
	M[2] = 0.0f; M[6] = 0.0f; M[10] = 1.0f; M[14] = 0.0f;
	M[3] = 0.0f; M[7] = 0.0f; M[11] = 0.0f; M[15] = 1.0f;
}

mat4x4::~mat4x4()
{
}

mat4x4::mat4x4(mat4x4 &Matrix)
{
	for(int i = 0; i < 16; i++)
	{
		M[i] = Matrix[i];
	}
}

mat4x4& mat4x4::operator = (mat4x4 &Matrix)
{
	for(int i = 0; i < 16; i++)
	{
		M[i] = Matrix[i];
	}

	return *this;
}

float& mat4x4::operator [] (int Index)
{
	return M[Index];
}

float* mat4x4::operator & ()
{
	return (float*)this;
}

mat4x4 operator * (mat4x4 &Matrix1, mat4x4 &Matrix2)
{
	mat4x4 Matrix3;

	Matrix3[0] = Matrix1[0] * Matrix2[0] + Matrix1[4] * Matrix2[1] + Matrix1[8] * Matrix2[2] + Matrix1[12] * Matrix2[3];
	Matrix3[1] = Matrix1[1] * Matrix2[0] + Matrix1[5] * Matrix2[1] + Matrix1[9] * Matrix2[2] + Matrix1[13] * Matrix2[3];
	Matrix3[2] = Matrix1[2] * Matrix2[0] + Matrix1[6] * Matrix2[1] + Matrix1[10] * Matrix2[2] + Matrix1[14] * Matrix2[3];
	Matrix3[3] = Matrix1[3] * Matrix2[0] + Matrix1[7] * Matrix2[1] + Matrix1[11] * Matrix2[2] + Matrix1[15] * Matrix2[3];

	Matrix3[4] = Matrix1[0] * Matrix2[4] + Matrix1[4] * Matrix2[5] + Matrix1[8] * Matrix2[6] + Matrix1[12] * Matrix2[7];
	Matrix3[5] = Matrix1[1] * Matrix2[4] + Matrix1[5] * Matrix2[5] + Matrix1[9] * Matrix2[6] + Matrix1[13] * Matrix2[7];
	Matrix3[6] = Matrix1[2] * Matrix2[4] + Matrix1[6] * Matrix2[5] + Matrix1[10] * Matrix2[6] + Matrix1[14] * Matrix2[7];
	Matrix3[7] = Matrix1[3] * Matrix2[4] + Matrix1[7] * Matrix2[5] + Matrix1[11] * Matrix2[6] + Matrix1[15] * Matrix2[7];

	Matrix3[8] = Matrix1[0] * Matrix2[8] + Matrix1[4] * Matrix2[9] + Matrix1[8] * Matrix2[10] + Matrix1[12] * Matrix2[11];
	Matrix3[9] = Matrix1[1] * Matrix2[8] + Matrix1[5] * Matrix2[9] + Matrix1[9] * Matrix2[10] + Matrix1[13] * Matrix2[11];
	Matrix3[10] = Matrix1[2] * Matrix2[8] + Matrix1[6] * Matrix2[9] + Matrix1[10] * Matrix2[10] + Matrix1[14] * Matrix2[11];
	Matrix3[11] = Matrix1[3] * Matrix2[8] + Matrix1[7] * Matrix2[9] + Matrix1[11] * Matrix2[10] + Matrix1[15] * Matrix2[11];

	Matrix3[12] = Matrix1[0] * Matrix2[12] + Matrix1[4] * Matrix2[13] + Matrix1[8] * Matrix2[14] + Matrix1[12] * Matrix2[15];
	Matrix3[13] = Matrix1[1] * Matrix2[12] + Matrix1[5] * Matrix2[13] + Matrix1[9] * Matrix2[14] + Matrix1[13] * Matrix2[15];
	Matrix3[14] = Matrix1[2] * Matrix2[12] + Matrix1[6] * Matrix2[13] + Matrix1[10] * Matrix2[14] + Matrix1[14] * Matrix2[15];
	Matrix3[15] = Matrix1[3] * Matrix2[12] + Matrix1[7] * Matrix2[13] + Matrix1[11] * Matrix2[14] + Matrix1[15] * Matrix2[15];

	return Matrix3;
}

vec4 operator * (mat4x4 &Matrix, vec4 &Vector)
{
	vec4 v;

	v.x = Matrix[0] * Vector.x + Matrix[4] * Vector.y + Matrix[8] * Vector.z + Matrix[12] * Vector.w;
	v.y = Matrix[1] * Vector.x + Matrix[5] * Vector.y + Matrix[9] * Vector.z + Matrix[13] * Vector.w;
	v.z = Matrix[2] * Vector.x + Matrix[6] * Vector.y + Matrix[10] * Vector.z + Matrix[14] * Vector.w;
	v.w = Matrix[3] * Vector.x + Matrix[7] * Vector.y + Matrix[11] * Vector.z + Matrix[15] * Vector.w;
	
	return v;
}

// 2D vector functions --------------------------------------------------------------------------------------------------------

float dot(vec2 &u, vec2 &v)
{
	return u.x * v.x + u.y * v.y;
}

float length(vec2 &u)
{
	return sqrt(u.x * u.x + u.y * u.y);
}

vec2 normalize(vec2 &u)
{
	return u / sqrt(u.x * u.x + u.y * u.y);
}

vec2 reflect(vec2 &i, vec2 &n)
{
	return i - 2.0f * dot(n, i) * n;
}

vec2 refract(vec2 &i, vec2 &n, float eta)
{
	vec2 r;

	float ndoti = dot(n, i), k = 1.0f - eta * eta * (1.0f - ndoti * ndoti);

	if(k >= 0.0f)
	{
		r = eta * i - n * (eta * ndoti + sqrt(k));
	}

	return r;
}

vec2 rotate(vec2 &u, float angle)
{
	return *(vec2*)&(RotationMatrix(vec3(0.0f, 0.0f, 1.0f), angle) * vec4(u, 0.0f, 1.0f));
}

// 3D vector functions --------------------------------------------------------------------------------------------------------

vec3 cross(vec3 &u, vec3 &v)
{
	return vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

float dot(vec3 &u, vec3 &v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

float length(vec3 &u)
{
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

vec3 normalize(vec3 &u)
{
	return u / sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

vec3 reflect(vec3 &i, vec3 &n)
{
	return i - 2.0f * dot(n, i) * n;
}

vec3 refract(vec3 &i, vec3 &n, float eta)
{
	vec3 r;

	float ndoti = dot(n, i), k = 1.0f - eta * eta * (1.0f - ndoti * ndoti);

	if(k >= 0.0f)
	{
		r = eta * i - n * (eta * ndoti + sqrt(k));
	}

	return r;
}

vec3 rotate(vec3 &u, vec3 &v, float angle)
{
	return *(vec3*)&(RotationMatrix(v, angle) * vec4(u, 1.0f));
}

// matrix functions -----------------------------------------------------------------------------------------------------------

mat4x4 BiasMatrix()
{
	mat4x4 BM;

	BM[0] = 0.5f; BM[4] = 0.0f; BM[8] = 0.0f; BM[12] = 0.5f;
	BM[1] = 0.0f; BM[5] = 0.5f; BM[9] = 0.0f; BM[13] = 0.5f;
	BM[2] = 0.0f; BM[6] = 0.0f; BM[10] = 0.5f; BM[14] = 0.5f;
	BM[3] = 0.0f; BM[7] = 0.0f; BM[11] = 0.0f; BM[15] = 1.0f;

	return BM;
}

mat4x4 BiasMatrixInverse()
{
	mat4x4 BMI;

	BMI[0] = 2.0f; BMI[4] = 0.0f; BMI[8] = 0.0f; BMI[12] = -1.0f;
	BMI[1] = 0.0f; BMI[5] = 2.0f; BMI[9] = 0.0f; BMI[13] = -1.0f;
	BMI[2] = 0.0f; BMI[6] = 0.0f; BMI[10] = 2.0f; BMI[14] = -1.0f;
	BMI[3] = 0.0f; BMI[7] = 0.0f; BMI[11] = 0.0f; BMI[15] = 1.0f;

	return BMI;
}

mat4x4 ViewMatrix(vec3 &X, vec3 &Y, vec3 &Z, vec3 &Position)
{
	mat4x4 VM;

	VM[0] = X.x;
	VM[1] = Y.x;
	VM[2] = Z.x;
	VM[3] = 0.0f;

	VM[4] = X.y;
	VM[5] = Y.y;
	VM[6] = Z.y;
	VM[7] = 0.0f;

	VM[8] = X.z;
	VM[9] = Y.z;
	VM[10] = Z.z;
	VM[11] = 0.0f;

	VM[12] = - (Position.x * X.x + Position.y * X.y + Position.z * X.z);
	VM[13] = - (Position.x * Y.x + Position.y * Y.y + Position.z * Y.z);
	VM[14] = - (Position.x * Z.x + Position.y * Z.y + Position.z * Z.z);
	VM[15] = 1.0f;

	return VM;
}

mat4x4 ViewMatrixInverse(mat4x4 &VM)
{
	mat4x4 VMI;

	VMI[0] = VM[0];
	VMI[1] = VM[4];
	VMI[2] = VM[8];
	VMI[3] = 0.0f;

	VMI[4] = VM[1];
	VMI[5] = VM[5];
	VMI[6] = VM[9];
	VMI[7] = 0.0f;

	VMI[8] = VM[2];
	VMI[9] = VM[6];
	VMI[10] = VM[10];
	VMI[11] = 0.0f;

	VMI[12] = - (VMI[0] * VM[12] + VMI[4] * VM[13] + VMI[8] * VM[14]);
	VMI[13] = - (VMI[1] * VM[12] + VMI[5] * VM[13] + VMI[9] * VM[14]);
	VMI[14] = - (VMI[2] * VM[12] + VMI[6] * VM[13] + VMI[10] * VM[14]);
	VMI[15] = 1.0f;

	return VMI;
}

mat4x4 OrthogonalProjectionMatrix(float left, float right, float bottom, float top, float n, float f)
{
	mat4x4 OPM;

	OPM[0] = 2.0f / (right - left);
	OPM[1] = 0.0f;
	OPM[2] = 0.0f;
	OPM[3] = 0.0f;

	OPM[4] = 0.0f;
	OPM[5] = 2.0f / (top - bottom);
	OPM[6] = 0.0f;
	OPM[7] = 0.0f;

	OPM[8] = 0.0f;
	OPM[9] = 0.0f;
	OPM[10] = -2.0f / (f - n);
	OPM[11] = 0.0f;

	OPM[12] = - (right + left) / (right - left);
	OPM[13] = - (top + bottom) / (top - bottom);
	OPM[14] = - (f + n) / (f - n);
	OPM[15] = 1.0f;

	return OPM;
}

mat4x4 PerspectiveProjectionMatrix(float fovy, float x, float y, float n, float f)
{
	mat4x4 PPM;

	float coty = 1.0f / tan(fovy * M_PI / 360.0f);
	float aspect = x / (y > 0.0f ? y : 1.0f);

	PPM[0] = coty / aspect;
	PPM[1] = 0.0f;
	PPM[2] = 0.0f;
	PPM[3] = 0.0f;

	PPM[4] = 0.0f;
	PPM[5] = coty;
	PPM[6] = 0.0f;
	PPM[7] = 0.0f;

	PPM[8] = 0.0f;
	PPM[9] = 0.0f;
	PPM[10] = (n + f) / (n - f);
	PPM[11] = -1.0f;

	PPM[12] = 0.0f;
	PPM[13] = 0.0f;
	PPM[14] = 2.0f * n * f / (n - f);
	PPM[15] = 0.0f;

	return PPM;
}

mat4x4 PerspectiveProjectionMatrixInverse(mat4x4 &PPM)
{
	mat4x4 PPMI;

	PPMI[0] = 1.0f / PPM[0];
	PPMI[1] = 0.0f;
	PPMI[2] = 0.0f;
	PPMI[3] = 0.0f;

	PPMI[4] = 0.0f;
	PPMI[5] = 1.0f / PPM[5];
	PPMI[6] = 0.0f;
	PPMI[7] = 0.0f;

	PPMI[8] = 0.0f;
	PPMI[9] = 0.0f;
	PPMI[10] = 0.0f;
	PPMI[11] = 1.0f / PPM[14];

	PPMI[12] = 0.0f;
	PPMI[13] = 0.0f;
	PPMI[14] = 1.0f / PPM[11];
	PPMI[15] = - PPM[10] / (PPM[11] * PPM[14]);

	return PPMI;
}

mat4x4 RotationMatrix(vec3 &Vector, float angle)
{
	mat4x4 RM;

	vec3 v = normalize(Vector);

	float c = cos(angle), s = sin(angle);

	RM[0] = 1.0f + (1.0f - c) * (v.x * v.x - 1.0f);
	RM[1] = (1.0f - c) * v.x * v.y + v.z * s;
	RM[2] = (1.0f - c) * v.x * v.z - v.y * s;
	RM[3] = 0.0f;

	RM[4] = (1.0f - c) * v.x * v.y - v.z * s;
	RM[5] = 1.0f + (1.0f - c) * (v.y * v.y - 1.0f);
	RM[6] = (1.0f - c) * v.y * v.z + v.x * s;
	RM[7] = 0.0f;

	RM[8] = (1.0f - c) * v.x * v.z + v.y * s;
	RM[9] = (1.0f - c) * v.y * v.z - v.x * s;
	RM[10] = 1.0f + (1.0f - c) * (v.z * v.z - 1.0f);
	RM[11] = 0.0f;

	RM[12] = 0.0f;
	RM[13] = 0.0f;
	RM[14] = 0.0f;
	RM[15] = 1.0f;

	return RM;
}

mat4x4 ScaleMatrix(float x, float y, float z)
{
	mat4x4 SM;

	SM[0] = x;
	SM[1] = 0.0f;
	SM[2] = 0.0f;
	SM[3] = 0.0f;

	SM[4] = 0.0f;
	SM[5] = y;
	SM[6] = 0.0f;
	SM[7] = 0.0f;

	SM[8] = 0.0f;
	SM[9] = 0.0f;
	SM[10] = z;
	SM[11] = 0.0f;

	SM[12] = 0.0f;
	SM[13] = 0.0f;
	SM[14] = 0.0f;
	SM[15] = 1.0f;

	return SM;
}

mat4x4 TranslationMatrix(float x, float y, float z)
{
	mat4x4 TM;

	TM[0] = 1.0f;
	TM[1] = 0.0f;
	TM[2] = 0.0f;
	TM[3] = 0.0f;

	TM[4] = 0.0f;
	TM[5] = 1.0f;
	TM[6] = 0.0f;
	TM[7] = 0.0f;

	TM[8] = 0.0f;
	TM[9] = 0.0f;
	TM[10] = 1.0f;
	TM[11] = 0.0f;

	TM[12] = x;
	TM[13] = y;
	TM[14] = z;
	TM[15] = 1.0f;

	return TM;
}
