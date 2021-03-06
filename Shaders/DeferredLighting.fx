#include "Shadows.fxh"

texture GBufferMaterial;
sampler GBufferMaterialSampler = sampler_state
{
   Texture = <GBufferMaterial>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

texture GBufferNormal;
sampler GBufferNormalSampler = sampler_state
{
   Texture = <GBufferNormal>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

texture GBufferDepth;
sampler GBufferDepthSampler = sampler_state
{
   Texture = <GBufferDepth>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

texture LightDiffuse;
sampler LightDiffuseSampler = sampler_state
{
   Texture = <LightDiffuse>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

texture LightSpecular;
sampler LightSpecularSampler = sampler_state
{
   Texture = <LightSpecular>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

texture SSAO;
sampler SSAOSampler = sampler_state
{
   Texture = <SSAO>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

texture BlurSource;
sampler BlurSourceTextureSampler = sampler_state
{
   Texture = <BlurSource>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

texture Random;
sampler RandomSampler = sampler_state
{
   Texture = <Random>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Wrap;
   AddressV  = Wrap;
};


// -------------------------------------------------------------
// shadow map texture stage states
// we try linear filtering here ... in case of fp textures it 
// is not supported on some graphics hardware
// -------------------------------------------------------------
texture ShadowMap1 : ShadowMap;

// no filtering in floating point texture
sampler2D g_samShadowMap = sampler_state
{
   Texture = <ShadowMap1>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = NONE;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

float4 sunlight : LightDirection;
float4 ambient = float4(0.1,0.1,0.1,1.0);
float4x4 matView : View;
float4x4 matViewInverse : ViewInverse;

float3 g_frustumCorners[4];
float4 cameraPos : CameraEyePosition;
float g_farClip : FARPLANE;

float4 g_fSplitPlane : SplitDistances;
float4x4 LightViewProj[4] : LightViewProjectionArray;

float2 g_TexelSize : TexelSize;

//wastes space since the gpu turns this into a vector4 but less instructions this way...
float2 gSunAtlasShift[4] : AtlasSplit;

//specular intensity
float4 SpecularI 
<
	string UIName = "Specular Intensity";
> = {0.8,0.8,0.8,0.8};

float light_vsm_epsilon
<
    string UIWidget = "slider";
    string UIName = "VSM Epsilon";
    float UIMin = 0.0;
    float UIMax = 1.0;
    float UIStep = 0.0001;
> = 0.000001;

// These values are generated by excel, we might want to use a 16 vectors that are more evenly distributed 
#define RAND_COUNT 16
float4 gRandomRotation[RAND_COUNT] = {
{  0.416,	-0.365,	0.832, 1.0 },
{  0.696,	 0.101,	0.710, 1.0 },
{ -0.740,	 0.126,	0.660, 1.0 },
{ -0.329,	 0.097,	0.939, 1.0 },
{ -0.117,	-0.259,	0.958, 1.0 },
{ -0.392,	 0.669,	0.631, 1.0 },
{ -0.435,	 0.107,	0.894, 1.0 },
{ -0.942,	 0.180,	0.283, 1.0 },
{ -0.424,	 0.110,	0.898, 1.0 },
{  0.818,	-0.220,	0.531, 1.0 },
{  0.589,	 0.037,	0.807, 1.0 },
{  0.780,	 0.350,	0.518, 1.0 },
{ -0.115,	 0.910,	0.398, 1.0 },
{ -0.878,	 0.221,	0.424, 1.0 },
{  0.838,	-0.232,	0.493, 1.0 },
{  0.637,	-0.081,	0.766, 1.0 }
};

// -------------------------------------------------------------
// Output channels
// -------------------------------------------------------------
struct VertexShaderInput
{
    float4 Pos : POSITION;
    float3 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Pos					: POSITION;
	float2 TopLeft	    		: TEXCOORD0;
	float3 viewDirection : TEXCOORD1;
};

VS_OUTPUT VS(VertexShaderInput input)
{
	VS_OUTPUT Out;

	Out.Pos.x = input.Pos.x;
	Out.Pos.y = input.Pos.y;
	Out.Pos.z = 0.5f;
	Out.Pos.w = 1.0f;
	

	Out.TopLeft = float2(0.5f, -0.5f) * input.Pos.xy + 0.5f.xx;

	Out.viewDirection = mul( g_frustumCorners[input.TexCoord.z], (float3x3)matViewInverse ).xyz;

    return Out;
}

struct PS_OUTPUT
{
	float4 diffuse	: COLOR0;
	float4 specular	: COLOR1;
};

PS_OUTPUT PS(VS_OUTPUT vin)
{
	PS_OUTPUT Out;

	// Get depth at this pixel
	float4 vpos = tex2D(GBufferDepthSampler, vin.TopLeft);

	float4 normColor = tex2D(GBufferNormalSampler, vin.TopLeft);

	// Getting the materials
	float3 normal = normColor.xyz*2.0 - 1.0;
	float specularIntensity = normColor.a;

	//we do this check for skydomes in particular, it allows for objects to 'bail out'
	//of the lighting pass. Later I would like to use the stencil buffer instead...
	if (specularIntensity > 0)
	{
		float4 worldPos = float4(vpos.x * vin.viewDirection + cameraPos.xyz, 1);

		//if there is no sunlight, then we want for the character to be in shadow...
		//This way, other light sources can light the character...
		float depth = (mul(worldPos, matView)).z;

		
		float3 splitDepth;
		float  split;

		//use depth coordinate to pick the correct quadrant from the atlas
		splitDepth.x = step( g_fSplitPlane[0], depth );
		splitDepth.y = step( g_fSplitPlane[1], depth );
		splitDepth.z = step( g_fSplitPlane[2], depth );
		split = (splitDepth.x + splitDepth.y + splitDepth.z);

		float4 ShadowMapUV = mul(worldPos, LightViewProj[split]);

		//pick the correct shadowmap split from the atlas
		ShadowMapUV.x = (ShadowMapUV.x * 0.5) + gSunAtlasShift[split].x;
		ShadowMapUV.y = (ShadowMapUV.y * 0.5) + gSunAtlasShift[split].y;

		float dist_to_light = 1 - depth / g_fSplitPlane[3];
		float fLightingFactor = compute_lighting_factor_VSM(g_samShadowMap, dist_to_light, ShadowMapUV);

		//0.3 = shadowstrength
		float shadowTerm = max(fLightingFactor, 0.3);

		float4 L = -sunlight;

		float diffuse 			= saturate(dot(L.xyz, normal));
		float3 diffuseColor 	= diffuse * shadowTerm;

		float3 View				= normalize(matViewInverse[3].xyz - worldPos.xyz);
		float3 H				= normalize(View + L.xyz);
		float specular			= pow(saturate(dot(normal, H)), 25);
		float3 specullarColor	= specular * specularIntensity;

		Out.diffuse		= float4(diffuseColor, 1.0);
		Out.specular	= float4(specullarColor, 1.0);
	}
	else
	{
		Out.diffuse		= float4(1,1,1, 1.0);
		Out.specular	= float4(0,0,0, 1.0);
	}

	return Out;
}

PS_OUTPUT PS_DiffuseShadow(VS_OUTPUT vin)
{
	PS_OUTPUT Out;

	// Get depth at this pixel
	float4 vpos = tex2D(GBufferDepthSampler, vin.TopLeft);

	float4 normColor = tex2D(GBufferNormalSampler, vin.TopLeft);

	// Getting the materials
	float3 normal = normColor.xyz*2.0 - 1.0;
	float specularIntensity = normColor.a;

	//we do this check for skydomes in particular, it allows for objects to 'bail out'
	//of the lighting pass. Later I would like to use the stencil buffer instead...
	if (specularIntensity > 0)
	{
		float4 worldPos = float4(vpos.x * vin.viewDirection + cameraPos, 1);

		//if there is no sunlight, then we want for the character to be in shadow...
		//This way, other light sources can light the character...
		float depth = (mul(worldPos, matView)).z;

		
		float3 splitDepth;
		float  split;

		splitDepth.x = step( g_fSplitPlane[0], depth );
		splitDepth.y = step( g_fSplitPlane[1], depth );
		splitDepth.z = step( g_fSplitPlane[2], depth );
		split = (splitDepth.x + splitDepth.y + splitDepth.z);

		float4 ShadowMapUV = mul(worldPos, LightViewProj[split]);

		//pick the correct shadowmap split from the atlas
		ShadowMapUV.x = (ShadowMapUV.x * 0.5) + gSunAtlasShift[split].x;
		ShadowMapUV.y = (ShadowMapUV.y * 0.5) + gSunAtlasShift[split].y;

		//need to compute the uv offsets now :(
		float dist_to_light = 1 - depth / g_fSplitPlane[3];
		float fLightingFactor = compute_lighting_factor_VSM(g_samShadowMap, dist_to_light, ShadowMapUV);

		//0.3 = shadowstrength
		float shadowTerm = max(fLightingFactor, 0.3);

		float4 L = -sunlight;

		float diffuse 			= saturate(dot(L.xyz, normal));
		float3 diffuseColor 	= diffuse * shadowTerm;

		Out.diffuse		= float4(diffuseColor, 1.0);
	}
	else
	{
		Out.diffuse		= float4(1,1,1, 1.0);
	}

	//not enough instructions to compute specular in PS 2.0....
	Out.specular	= float4(0,0,0, 1.0);

	return Out;
}

//for PS 2.0, we have to contribute to specular during this pass (2.0 cards have to use multipass)
PS_OUTPUT PS_Specular(VS_OUTPUT vin)
{
	PS_OUTPUT Out;

	// Get depth at this pixel
	float4 vpos = tex2D(GBufferDepthSampler, vin.TopLeft);

	float4 normColor = tex2D(GBufferNormalSampler, vin.TopLeft);

	// Getting the materials
	float3 normal = normColor.xyz*2.0 - 1.0;
	float specularIntensity = normColor.a;

	float4 worldPos = float4(vpos.x * vin.viewDirection + cameraPos, 1);

	float4 L = -sunlight;

	Out.diffuse		= float4(0,0,0, 1.0);

	float3 View				= normalize(matViewInverse[3].xyz - worldPos.xyz);
	float3 H				= normalize(View + L.xyz);
	float specular			= pow(saturate(dot(normal, H)), 25);
	float3 specullarColor	= specular * specularIntensity;

	Out.specular	= float4(specullarColor, 1.0);

	return Out;
}

//debug the depth render target
PS_OUTPUT DEBUG_FARCLIP_PS(VS_OUTPUT vin)
{
	PS_OUTPUT Out;

	float4 vpos = tex2D(GBufferDepthSampler, vin.TopLeft);
	float3 viewPos = vpos.x * vin.viewDirection.xyz;
	
	//make 0-->1
	float depth = viewPos.z / g_farClip;

	Out.diffuse		= float4(depth,depth,depth, 1.0);
	Out.specular	= float4(0,0,0, 1.0);

	return Out;
}

//debug the depth render target
PS_OUTPUT DEBUG_DEPTH_PS(VS_OUTPUT vin)
{
	PS_OUTPUT Out;

	float4 vpos = tex2D(GBufferDepthSampler, vin.TopLeft);
	Out.diffuse		= float4(vpos.x,vpos.x,vpos.x, 1.0);
	Out.specular	= float4(0,0,0, 1.0);

	return Out;
}


technique RenderDirectionalLight
<
	string shaderDescription = "RenderDirectionalLight";
>
{

	pass p0
	{
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS(); //DEBUG_FARCLIP_PS(); DEBUG_DEPTH_PS(); //PS();
	}
}


//for older cards that can only do PS 2.0 but supports MRT (like ATI Radeon 9800 Pro). It's multipass
//because I couldnt fit specular contribution into the pixel shader
technique RenderDirectionalLight_LowSpec
<
	string shaderDescription = "RenderDirectionalLight_LowSpec";
>
{
	pass p0
	{
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS_DiffuseShadow();
	}
	
	pass p1
	{
		ZWriteEnable = false;
		ZEnable = false;

		AlphaBlendEnable = True;
		AlphaTestEnable = True;
		AlphaRef = 0;
		SrcBlend = One;
		DestBlend = One;

		PixelShader = compile ps_2_0 PS_Specular();
	}
}

float3 getRandom(float2 vpos, float2 screenTC, float2 screenSize)
{
	return normalize(tex2D(RandomSampler, fmod(vpos, float2(64,64))/float2(64,64) ).xyz);
}

float4 PSComposite(VS_OUTPUT In) : COLOR
{
	float2 Tex = In.TopLeft;

	float4 matColor = tex2D(GBufferMaterialSampler, Tex);

	float4 diffuse = tex2D(LightDiffuseSampler, Tex);
	float4 diffuseColor = (matColor * diffuse) + float4(0.1 * matColor.xyz,1);

	float4 specularColor = tex2D(LightSpecularSampler, Tex);

//	float4 ssaoColor = tex2D(SSAOSampler, Tex);
	float4 ssaoColor = tex2D(BlurSourceTextureSampler, Tex);

	return saturate(diffuseColor + specularColor + ambient*ssaoColor);
}

technique RenderComposite
<
	string shaderDescription = "RenderComposite";
>
{

	pass p0 
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PSComposite();
		
	}
}


float ComputeSSAO(float2 vpos, float2 screenTC, float2 screenSize, float farClipDist)
{
	float3 random = getRandom(vpos, screenTC, screenSize);
	float3 vRotation = gRandomRotation[fmod(random.x*screenSize.y + random.y*screenSize.x + abs(cameraPos.x + cameraPos.y + cameraPos.z), RAND_COUNT)].xyz;

	float3x3 rotMat;
	float h = 1 / (1 + vRotation.z);
	rotMat._m00 = h*vRotation.y*vRotation.y+vRotation.z;
	rotMat._m01 = -h*vRotation.y*vRotation.x;
	rotMat._m02 = -vRotation.x;
	rotMat._m10 = -h*vRotation.y*vRotation.x;
	rotMat._m11 = h*vRotation.x*vRotation.x+vRotation.z;
	rotMat._m12 = -vRotation.y;
	rotMat._m20 = vRotation.x;
	rotMat._m21 = vRotation.y;
	rotMat._m22 = vRotation.z;

	float fSceneDepthP = tex2D(GBufferDepthSampler, screenTC).r * farClipDist;

	const int nSamplesNum = 16;
	float offsetScale = 0.01;
	const float offsetScaleStep = 1 + 2.4/nSamplesNum;
	float Accessibility = 0;
	for(int i=0; i<(nSamplesNum/8); i++)
	for(int x=-1; x<=1; x+=2)
	for(int y=-1; y<=1; y+=2)
	for(int z=-1; z<=1; z+=2)
	{
		float3 vOffset = normalize(float3(x,y,z)) * (offsetScale *= offsetScaleStep);
		float3 vRotatedOffset = mul(vOffset, rotMat);
		float3 vSamplePos = float3(screenTC, fSceneDepthP);
		vSamplePos += float3(vRotatedOffset.xy, vRotatedOffset.z * fSceneDepthP * 2);
		float fSceneDepthS = tex2D(GBufferDepthSampler, vSamplePos.xy) * farClipDist;
		float fRangeIsInvalid = saturate( ( ( fSceneDepthP - fSceneDepthS) / fSceneDepthS ) );
		float value = lerp( fSceneDepthS > vSamplePos.z, 0.5, fRangeIsInvalid );
		Accessibility += value;
	}
	Accessibility = Accessibility / nSamplesNum;
	return saturate(Accessibility*Accessibility + Accessibility);
}

float4 PS_SSAO(VS_OUTPUT vin, float2 vpos:VPOS) : COLOR
{
	float ssao = ComputeSSAO(vpos, vin.TopLeft, g_TexelSize, g_farClip);
	return float4(ssao.xxx, 1.0);
/*
	float4 offset = (-1.0f/g_TexelSize.x, -1.0f/g_TexelSize.y, 1.0f/g_TexelSize.x, 1.0f/g_TexelSize.y);

	float depth = tex2D(GBufferDepthSampler, vin.TopLeft + offset.xy).x;
	float depthSmall = tex2D(SSAOSmallSampler, vin.TopLeft + offset.xy).x;

	float4 ao = 0.0f;

	ao += tex2D(SSAOSmallSampler, vin.TopLeft + offset.xy);
	ao += tex2D(SSAOSmallSampler, vin.TopLeft + offset.xw);
	ao += tex2D(SSAOSmallSampler, vin.TopLeft + offset.zy);
	ao += tex2D(SSAOSmallSampler, vin.TopLeft + offset.zw);

	ao *= 0.25f;
	return ao;
*/
}

technique RenderSSAO
<
	string shaderDescription = "RenderSSAO";
>
{

	pass p0
	{
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS_SSAO();
	}
}

float4 PS_Blur(VS_OUTPUT vin, uniform float2 direction) : COLOR
{
	float2 texcoord = vin.TopLeft + (0.5 - direction) / g_TexelSize;
	float4 color = 0;
	color += tex2D(BlurSourceTextureSampler, vin.TopLeft - (4.0 - 9.0 / (9.0 + 1.0) * 0.5) * direction / g_TexelSize) * (1 + 9);
	color += tex2D(BlurSourceTextureSampler, vin.TopLeft - (2.0 - 84.0 / (84.0 + 36.0) * 0.5) * direction / g_TexelSize) * (36 + 84);
	color += tex2D(BlurSourceTextureSampler, vin.TopLeft + 0 * direction / g_TexelSize) * (126 + 126);
	color += tex2D(BlurSourceTextureSampler, vin.TopLeft + (2.0 - 84.0 / (84.0 + 36.0) * 0.5) * direction / g_TexelSize) * (36 + 84);
	color += tex2D(BlurSourceTextureSampler, vin.TopLeft + (4.0 - 9.0 / (9.0 + 1.0) * 0.5) * direction / g_TexelSize) * (1 + 9);
	return color / float(1 + 9 + 36 + 84 + 126) / 2.0;
}

technique BlurX
<
	string shaderDescription = "BlurX";
>
{

	pass p0
	{
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS_Blur(float2(1,0));
	}
}

technique BlurY
<
	string shaderDescription = "BlurY";
>
{

	pass p0
	{
		ZWriteEnable = false;
		ZEnable = false;
		AlphaBlendEnable = false;
		CullMode = NONE;

		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS_Blur(float2(0,1));
	}
}
