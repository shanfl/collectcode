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
texture Reflectionmap;
texture Refractionmap;
texture noiseXZ;

struct VS_INPUT
{
    float3 Pos      : POSITION;
	float3 Normal   : NORMAL;
	float2 tc		: TEXCOORD0;
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
	//SRGBTexture = true;
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

sampler refrmap = sampler_state
{  
    Texture = <Refractionmap>; 
    MipFilter = LINEAR; 
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
	AddressU  = CLAMP;		
	AddressV  = CLAMP;			
	MipMapLodBias = (LODbias);
};


struct VS_NV20
{
    float4  Pos			: POSITION;    
	float2  dot			: TEXCOORD0;
	float3	R			: TEXCOORD1;
	float2  refr_tc  	: TEXCOORD2;
	float2  refl_tc  	: TEXCOORD3;
	float3  sunlight	: COLOR0;
	
};

VS_NV20 VShaderNV20(VS_INPUT i)
{
    VS_NV20   o;
   		
    o.Pos = mul(float4(i.Pos.xyz,1), mViewProj);    
	float3 normal = normalize(i.Normal.xyz);
	float3 v = normalize(i.Pos.xyz - view_position.xyz/view_position.w);

	o.R = reflect(v,normal);		
	o.dot = dot(o.R,normal);	
	float sunlight = pow(saturate(dot(o.R, sun_vec)),sun_shininess);
	o.sunlight = sunlight*float3(1.2, 0.8, 0.6);
		
	float4 tpos = mul(float4(i.Pos.x,0,i.Pos.z,1), mViewProj);
	tpos.xyz = tpos.xyz/tpos.w;
	tpos.xy = 0.5 + 0.5*tpos.xy*float2(1,-1);
	tpos.z = reflrefr_offset/tpos.z;	
	
	o.refr_tc = tpos.xy - tpos.z*normal.xz;
	o.refl_tc = tpos.xy - tpos.z*normal.xz;
	
	return o;
}

VS_NV20 VShaderNV30(VS_INPUT i)
{
    VS_NV20   o;
   		
    o.Pos = mul(float4(i.Pos.xyz,1), mViewProj);    
	float3 normal = normalize(i.Normal.xyz);
	float3 v = normalize(i.Pos.xyz - view_position.xyz/view_position.w);

	o.R = reflect(v,normal);		
	o.dot = dot(o.R,normal);	
	float sunlight = pow(saturate(dot(o.R, sun_vec)),sun_shininess);
	o.sunlight = sunlight*float3(1.2, 0.4, 0.1);
		
	float4 tpos = mul(float4(i.Pos.x,0,i.Pos.z,1), mViewProj);
	tpos.xyz = tpos.xyz/tpos.w;
	tpos.xy = 0.5 + 0.5*tpos.xy*float2(1,-1);
	tpos.z = reflrefr_offset/tpos.z;	
	
	o.refr_tc = tpos.xy - tpos.z*normal.xz;
	o.refl_tc = tpos.xy - tpos.z*normal.xz;
	
	return o;
}


float4 PShaderNV20(VS_NV20 i) : COLOR
{
    float4 ut;
	ut.a = 1;
	
	float f = tex1D(fresnel,i.dot);			
    float3 global_refl = texCUBE(sky,i.R);	
    float4 local_refl = tex2D(reflmap, i.refl_tc);
    float3 refr = tex2D(refrmap, i.refr_tc);
	float3 refl = lerp( global_refl, local_refl.rgb, local_refl.a);
	ut.rgb = lerp( refr, refl, f) + i.sunlight*sun_strength;

    return ut;
}

float4 PShaderNV30(VS_NV20 i) : COLOR
{
    float4 ut;
	ut.a = 1;
	
	float f = tex1D(fresnel,i.dot);			
    float3 global_refl = texCUBE(sky,i.R)  + i.sunlight.rgb*sun_strength;	
    float4 local_refl = tex2D(reflmap, i.refl_tc);
    float3 refr = tex2D(refrmap, i.refr_tc);
	float3 refl = lerp( global_refl, local_refl.rgb, local_refl.a);
	ut.rgb = lerp( refr, refl, f);

    return ut;
}

technique T0
{
    pass P0
    {        
		pixelshader = compile ps_2_0 PShaderNV30();
		vertexshader = compile vs_2_0 VShaderNV30();        
    }
}

technique T1
{
    pass P0
    {        
		pixelshader = compile ps_1_4 PShaderNV20();
		vertexshader = compile vs_1_1 VShaderNV20();        
    }
}

technique T2
{
    pass P0
    {        
		pixelshader = compile ps_1_1 PShaderNV20();
		vertexshader = compile vs_1_1 VShaderNV20();        
    }
}

