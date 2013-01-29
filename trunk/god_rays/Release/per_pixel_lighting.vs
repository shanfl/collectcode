#version 120

varying vec3 LD, Normal, LDR;

void main()
{
	LD = -(gl_ModelViewMatrix * gl_Vertex).xyz;
	Normal = gl_NormalMatrix * gl_Normal;
	LDR = reflect(-LD, Normal);
	gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
