//Blinn-Phong shader by Richard Osborne
//Uses Jim Blinn Half Vector
//Written 01/2006
//References: Programming Vertex & Pixel Shaders by Wolfgang Engel
//Useful on world geometry

//from Shadowmap Antiliasing article in GPU Gems
float3 offset_lookup(sampler2D map, float4 loc, float2 offset, float2 texmapscale)
{
	return tex2Dproj(map, float4(loc.xy + offset * texmapscale * loc.w, loc.z, loc.w));
}

float compute_lighting_factor16(sampler2D map, float4 shadowCoord, float2 texmapscale)
{
	float sum = 0;
	float x,y;
	
	for(y=-1.5;y<=1.5;y+=1.0)
		for(x=-1.5;x<=1.5;x+=1.0)
			sum += offset_lookup(map, shadowCoord, float2(x,y), texmapscale);
			
	return sum / 16.0;   
}

float compute_lighting_factor4(sampler2D map, float4 shadowCoord, float2 texmapscale, float4 position)
{
	float2 offset = (float)(frac(position.xy * 0.5) > 0.25);
	offset.y += offset.x;
	if (offset.y > 1.1)
	{
		offset.y = 0;
	}
	
	return (offset_lookup(map, shadowCoord, offset + float2(-1.5,0.5), texmapscale)  +
	offset_lookup(map, shadowCoord, offset + float2(0.5,0.5), texmapscale) +
	offset_lookup(map, shadowCoord, offset + float2(-1.5,-1.5), texmapscale) +
	offset_lookup(map, shadowCoord, offset + float2(0.5,-1.5), texmapscale) ) * 0.25;
}

//Note: if passing in perspective projection, dont forget to divide by w before calling this
float compute_lighting_factor_VSM(sampler2D ShadowMapSampler, float dist_to_light, float4 ShadowMapUV)
{
	// TODO: Emulate bilinear filtering on unsupporting hardware
	float4 moments = tex2D(ShadowMapSampler, ShadowMapUV.xy);

	//float dist_to_light = 1 - depth / g_fSplitPlane[minNumOfShadowmaps-1];
	float lit_factor = (dist_to_light <= moments.x);

	// Variance shadow mapping
	//see http://www.gamedev.net/community/forums/topic.asp?topic_id=374989 - VSM
	float E_x2 = moments.y;
	float Ex_2 = moments.x * moments.x;
	const float light_vsm_epsilon = 0.0001;
	float variance = min(max(E_x2 - Ex_2, 0.0) + light_vsm_epsilon, 1.0);
	float m_d = (moments.x - dist_to_light);
	float p = variance / (variance + m_d * m_d);

	return max(lit_factor, p);
}