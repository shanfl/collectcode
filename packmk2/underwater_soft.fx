// oi oi

float4x4	mViewProj;
float4		view_position;
float		LODbias;
float		sun_alfa, sun_theta, sun_shininess, sun_strength;
float3		watercolour;

texture EnvironmentMap;
texture Normalmap;
texture FresnelMap;

struct VS_INPUT
{
    float3 Pos      : POSITION;
	float3 Normal    : NORMAL;
	float2 tc		: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;    
	float2 tc		: TEXCOORD0;
	float3  normal	: TEXCOORD1;
	float3	viewvec	: TEXCOORD2;
	float3	sun		: TEXCOORD3;
};

samplerCUBE sky = sampler_state
{  
    Texture = <EnvironmentMap>; 
    MipFilter = LINEAR; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = WRAP;		
    AddressV  = WRAP;
    AddressW  = WRAP;
	MipMapLodBias = (LODbias);
	//SRGBTexture = true;
};

sampler fresnel = sampler_state
{  
    Texture = <FresnelMap>; 
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

/* DX9 class shaders	*/

VS_OUTPUT VShaderR300(VS_INPUT i)
{
    VS_OUTPUT   o;
   		
    o.Pos = mul(float4(i.Pos.xyz,1), mViewProj);    
	o.normal = normalize(i.Normal.xyz);
	o.viewvec = i.Pos.xyz - view_position.xyz/view_position.w;
	o.tc = i.tc;
	o.sun.x = cos(sun_theta)*sin(sun_alfa);
	o.sun.y = sin(sun_theta);
	o.sun.z = cos(sun_theta)*cos(sun_alfa);
	return o;
}


float4 PShaderR300(VS_OUTPUT i) : COLOR
{
    //return float4(1,0,0,1);
    float4 ut;
	ut.a = 1;
	float3 v = normalize(i.viewvec);
	float3 N = 2*tex2D(nmap,i.tc)-1;
	float3 R = refract(v,N,1.33);			
	R.y = - R.y;
	float f = tex1D(fresnel,dot(R,N));	
	float sunlight = sun_strength*pow(saturate(dot(R, i.sun)),sun_shininess);
    float3 col = texCUBE(sky,R) + sunlight*float3(1.2, 0.7, 0.3);	
	float3 reflcol = watercolour + saturate(30*float3(0.4,0.6,0.8)*(1-dot(float3(0,1,0),N)));
	ut.rgb = lerp(col, reflcol, f) ;

	return ut;
}

/*	DX8 class shaders	*/

struct VS_NV20
{
    float4  Pos			: POSITION;    
	float2  dot			: TEXCOORD1;
	float3	R			: TEXCOORD2;
	float3	sun			: TEXCOORD3;
	float3  sunlight	: COLOR0;
};

VS_NV20 VShaderNV20(VS_INPUT i)
{
    VS_NV20   o;
   		
    o.Pos = mul(float4(i.Pos.xyz,1), mViewProj);    
	float3 normal = normalize(i.Normal.xyz);
	float3 v = normalize(i.Pos.xyz - view_position.xyz/view_position.w);

	// what am I doing here? ;)
	o.sun.x = cos(sun_theta)*sin(sun_alfa);
	o.sun.y = sin(sun_theta);
	o.sun.z = cos(sun_theta)*cos(sun_alfa);
	
	o.R = reflect(v,normal);		
	o.dot = dot(o.R,normal);
	
	float sunlight = sun_strength*pow(saturate(dot(o.R, o.sun)),sun_shininess);
	o.sunlight = sunlight*float3(1.2, 0.6, 0.1);
	return o;
}

float4 PShaderNV20(VS_NV20 i) : COLOR
{
    float4 ut;
	ut.a = 1;
	
	float f = tex1D(fresnel,i.dot);			
    float3 col = texCUBE(sky,i.R) + i.sunlight;	
	ut.rgb = lerp(float3(0.05,0.15,0.20), col, f) ;

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

/*technique T1
{
    pass P0
    {        
		pixelshader = compile ps_1_1 PShaderNV20();
		vertexshader = compile vs_1_1 VShaderNV20();        
    }
}*/
