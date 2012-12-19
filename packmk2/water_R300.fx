// oi oi

float4x4	mViewProj;
float4x4	mView;
float4		view_position;
float3		watercolour;
float		LODbias;
float		sun_shininess, sun_strength;
float3		sun_vec;
float		reflrefr_offset;
bool		diffuseSkyRef;

texture EnvironmentMap;

texture FresnelMap;
texture Heightmap;
texture Normalmap;
texture Refractionmap;
texture Reflectionmap;
texture noiseXZ;

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
	float3  normal		: TEXCOORD1;
	float3	viewvec		: TEXCOORD2;
	float3	screenPos	: TEXCOORD3;	
	float3  worldPos	: TEXCOORD4;
	float3	screenPosDis	: TEXCOORD5;
};

samplerCUBE sky = sampler_state
{  
    Texture = <EnvironmentMap>; 
    MipFilter = NONE; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = WRAP;		
    AddressV  = WRAP;
    AddressW  = WRAP;
	//MipMapLodBias = (LODbias);
//	SRGBTexture = true;
};

sampler fresnel = sampler_state
{  
    Texture = <FresnelMap>; 
    MipFilter = NONE; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = CLAMP;		
	AddressV  = CLAMP;

};

sampler hmap = sampler_state
{  
    Texture = <Heightmap>; 
    MipFilter = LINEAR; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = CLAMP;		
	AddressV  = CLAMP;		
};

sampler nmap = sampler_state
{  
    Texture = <Normalmap>; 
    MipFilter = LINEAR; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = CLAMP;		
	AddressV  = CLAMP;		
};

sampler refrmap = sampler_state
{  
    Texture = <Refractionmap>; 
    MipFilter = LINEAR; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = CLAMP;		
	AddressV  = CLAMP;	

};

sampler reflmap = sampler_state
{  
    Texture = <Reflectionmap>; 
    MipFilter = LINEAR; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = CLAMP;		
	AddressV  = CLAMP;			
	MipMapLodBias = (LODbias);

};

sampler noise = sampler_state 
{
	texture = <noiseXZ>;
	AddressU  = WRAP;		
	AddressV  = WRAP;
	MIPFILTER = POINT; //LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;	
};

/* DX9 class shaders	*/

VS_OUTPUT VShaderR300(VS_INPUT i)
{
    VS_OUTPUT   o;
	o.worldPos = i.Pos.xyz/4;
	
    o.Pos = mul(float4(i.Pos.xyz,1), mViewProj);    
	o.normal = normalize(i.Normal.xyz);
	o.viewvec = normalize(i.Pos.xyz - view_position.xyz/view_position.w);
	o.tc = i.tc;	
	
	o.screenPosDis = (o.Pos.xyz / o.Pos.w);
	o.screenPosDis.xy = 0.5 + 0.5*o.screenPosDis.xy*float2(1,-1);
	o.screenPosDis.z = reflrefr_offset/o.screenPosDis.z;
	
	// alt screenpos

	float4 tpos = mul(float4(i.Pos.x,0,i.Pos.z,1), mViewProj);
	o.screenPos = tpos.xyz/tpos.w;
	o.screenPos.xy = 0.5 + 0.5*o.screenPos.xy*float2(1,-1);
	o.screenPos.z = reflrefr_offset/o.screenPos.z;	
	
	return o;
}


float4 PShaderR300(VS_OUTPUT i) : COLOR
{
    float4 ut;
	ut.a = 1;
	float3 v = i.viewvec;
	float3 N = 2*tex2D(nmap,i.tc)-1;
		
	float3 R = normalize(reflect(v,N));		
	
	//R.y = max(R.y,0);
	float f = tex1D(fresnel,dot(R,N));		
		
	float3 sunlight = sun_strength*pow(saturate(dot(R, sun_vec)),sun_shininess)*float3(1.2, 0.4, 0.1);
	float4 refl = tex2D(reflmap,i.screenPos.xy-i.screenPos.z*N.xz);
	float3 skyrefl;
	skyrefl = texCUBE(sky,R);
	
	float3 col = lerp(skyrefl+sunlight,refl.rgb,refl.a);
	float3 refr = tex2D(refrmap,i.screenPos.xy-i.screenPos.z*N.xz);
	ut.rgb = lerp( refr, col, f);
			
    return ut;
}


technique T0
{
    pass P0
    {        
		pixelshader = compile ps_2_0 PShaderR300();
		vertexshader = compile vs_1_1 VShaderR300();
	}
}
