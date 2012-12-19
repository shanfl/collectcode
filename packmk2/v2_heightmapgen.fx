	// oi oi

//float		amplitude;
float		scale;
//float		mapsize_x,mapsize_y;

struct VS_INPUT
{
    float3  Pos     : POSITION;
    float2  tc     : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4  Pos			: POSITION;
	float2  tc0			: TEXCOORD0;	
	float2  tc1			: TEXCOORD1;
};

texture noise0;
texture noise1;

// samplers
sampler N0 = sampler_state 
{
	texture = <noise0>;
	AddressU  = WRAP;		
	AddressV  = WRAP;
	MIPFILTER = LINEAR; //LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MipMapLodBias = 0;
};
sampler N1 = sampler_state 
{
	texture = <noise1>;
	AddressU  = WRAP;		
	AddressV  = WRAP;
	MIPFILTER = LINEAR; //LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MipMapLodBias = 0;
};

/*float4 calc_worldpos(float2 uv)
{	
	float4	origin = float4(uv.xy,-1,1);
    float4	direction = float4(uv.xy,1,1);
    
    origin	  = mul(origin, mProjector);
    direction = mul(direction, mProjector) - origin; 
    
    float	l = -origin.y / direction.y;
    
    float4 worldPos = origin + direction*l;
    worldPos /= worldPos.w;
	return worldPos;
}*/


VS_OUTPUT VShader(VS_INPUT i)
{
	VS_OUTPUT o;
	o.Pos = float4( i.tc.x*2-1,1-i.tc.y*2, 0, 1 );
	o.tc0 = scale*i.Pos.xz*0.007813;
	o.tc1 = scale*i.Pos.xz*0.125;
//	o.tc0 = i.tc;
//	o.tc1 = 16*i.tc;
	return o;
}


float4 PShader(VS_OUTPUT i) : COLOR
{	
	//return float4(1,0,0,1);
	//return tex2D(N0, i.tc0);
	//return tex2D(N1, i.tc1);
	return tex2D(N0, i.tc0) + tex2D(N1, i.tc1) - 0.5;
}

technique T0
{
	pass P0
	{
		vertexshader = compile vs_2_0 VShader();
		pixelshader = compile ps_2_0 PShader();  
	}
}

