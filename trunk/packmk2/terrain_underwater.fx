// oi oi

float4x4	mViewProj;
float4x4	mView;
float4		view_position;
float3		watercolour;
float		sun_shininess, sun_strength;
float3		sun_vec;

texture texDiffuse;

struct VS_INPUT
{
    float3 Pos      : POSITION;
	float3 Normal   : NORMAL;
	float2 tc		: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  Pos			: POSITION;    
	float2	tc			: TEXCOORD0;
	float	fade		: TEXCOORD1;
	float4	colour		: COLOR0;
};

sampler sDiffuse = sampler_state
{  
    Texture = <texDiffuse>; 
    MipFilter = NONE; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = CLAMP;		
	AddressV  = CLAMP;		
};

VS_OUTPUT VShader(VS_INPUT i)
{
    VS_OUTPUT o;	
	o.Pos = mul(float4(i.Pos.x, 0.75*i.Pos.y,i.Pos.z, 1), mViewProj);
	o.tc = i.tc;
	o.colour = dot(sun_vec,i.Normal);
	o.fade = 0.3 + 0.7 * saturate(-i.Pos.y*0.04);
	return o;
}

float4 PShader(VS_OUTPUT i) : COLOR
{	
    return lerp(i.colour*tex2D(sDiffuse, i.tc), float4(watercolour,1), i.fade );
}

technique T0
{
    pass P0
    {        
		pixelshader = compile ps_2_0 PShader();
		vertexshader = compile vs_1_1 VShader();
	}
}
