	// oi oi

float	inv_mapsize_x,inv_mapsize_y;
float4	corner00, corner01, corner10, corner11;
float	amplitude;

struct VS_INPUT
{
    float3  Pos     : POSITION;
    float2  tc     : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4  Pos			: POSITION;
	float2	tc			: TEXCOORD0;
	float3  tc_p_dx		: TEXCOORD1;
	float3  tc_p_dy		: TEXCOORD2;
	float3  tc_m_dx		: TEXCOORD3;
	float3  tc_m_dy		: TEXCOORD4;
};

texture hmap;

// samplers
sampler hsampler = sampler_state 
{
	texture = <hmap>;
	AddressU  = WRAP;		
	AddressV  = WRAP;
	MIPFILTER = NONE;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
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
	float scale = 1;
	float2 tc = i.tc + float2(-inv_mapsize_x*scale,0);
	float4 meh = lerp(lerp(corner00,corner01,tc.x),lerp(corner10,corner11,tc.x),tc.y);
	o.tc_m_dx = meh.xyz/meh.w;
	
	tc = i.tc + float2(+inv_mapsize_x*scale,0);
	meh = lerp(lerp(corner00,corner01,tc.x),lerp(corner10,corner11,tc.x),tc.y);
	o.tc_p_dx = meh.xyz/meh.w;
	
	tc = i.tc + float2(0,-inv_mapsize_y*scale);
	meh = lerp(lerp(corner00,corner01,tc.x),lerp(corner10,corner11,tc.x),tc.y);
	o.tc_m_dy = meh.xyz/meh.w;
	
	tc = i.tc + float2(0,inv_mapsize_y*scale);
	//meh = (1-tc.y)*(corner00*(1-tc.x) + corner01*tc.x) + tc.y*(corner10*(1-tc.x) + corner11*tc.x);
	meh = lerp(lerp(corner00,corner01,tc.x),lerp(corner10,corner11,tc.x),tc.y);
	o.tc_p_dy = meh.xyz/meh.w;

	o.tc = i.tc;
	
	return o;
}


float4 PShader(VS_OUTPUT i) : COLOR
{	
	float2	dx = {inv_mapsize_x,0},
			dy = {0,inv_mapsize_y};
	i.tc_p_dx.y = amplitude*tex2D(hsampler, i.tc+dx);
	i.tc_m_dx.y = amplitude*tex2D(hsampler, i.tc-dx);
	i.tc_p_dy.y = amplitude*tex2D(hsampler, i.tc+dy);
	i.tc_m_dy.y = amplitude*tex2D(hsampler, i.tc-dy);
	//return float4(0.5+0.5*i.tc_p_dx,1);
	float3 normal = normalize(-cross(i.tc_p_dx-i.tc_m_dx, i.tc_p_dy-i.tc_m_dy));
	return float4(0.5+0.5*normal,1);
}

technique T0
{
	pass P0
	{
		vertexshader = compile vs_2_0 VShader();
		pixelshader = compile ps_2_0 PShader();  
	}
}

