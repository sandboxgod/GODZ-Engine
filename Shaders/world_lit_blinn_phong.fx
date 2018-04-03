//Blinn-Phong shader by Richard Osborne
//Uses Jim Blinn Half Vector
//Written 01/2006
//References: Programming Vertex & Pixel Shaders by Wolfgang Engel
//Useful on world geometry
#include "Shadows.fxh"

float4x4 matWorldVP : WorldViewProjection;
float4x4 matWorld : World;
float4x4 matViewInverse : ViewInverse;
texture tDiffuse : Diffuse;	//color map
float4x4 g_mWorldView : WorldView;

// transform projected textures
// == mLightViewProj * texture adjustment matrix
// mScale * mTransform * mRotate * mView * mProj * mScaleBias
float4x4 mLightViewProjTexAdj  : LightViewProjectionAdj;
float4x4 mLightViewProjTexAdj2 : LightViewProjectionAdj2;
float4x4 mLightViewProjTexAdj3 : LightViewProjectionAdj3;
float4x4 mLightViewProjTexAdj4 : LightViewProjectionAdj4;

float4x4 mLightViewProj : LightViewProjection;

float2 g_shadowMapTexSizeOverOne : ShadowMapTexSizeOverOne;

static const int g_iNumSplits = 4;
float4 g_fSplitPlane : SplitDistances;
float g_shadowMapTexSize : ShadowMapTexSize;

float g_farClip : FARPLANE;

//wastes space since the gpu turns this into a vector4...
float2 gSunAtlasShift[4] : AtlasSplit;

//How much shadow to allow (1.0 is max, 0.0f is min)
float shadowStrength
<
	string UIName = "Shadow Threshold";
> = 0.3f;

float4   lightDir : LIGHTDIRECTION;

//ambient intensity
float4 AmbientI 
<
	string UIName = "Ambient Intensity";
> = {1.0, 1.0, 1.0, 1.0}; //{0.5, 0.0, 0.0, 1.0f}; 

float4 AmbColor
<
	string UIName = "Ambient Color";
> = {0.0, 0.0, 0.0, 0.0f}; 

//diffuse intensity
float4 DiffuseI
<
	string UIName = "Diffuse Intensity";
> = {1.0,1.0,1.0,1.0}; 

//specular intensity
float SpecularIntensity
<
	string UIName = "Specular Intensity";
> = 0.8;

struct v2in
{
	float4 Pos : POSITION;
	float3 Norm  : NORMAL;
	float2 uv : TEXCOORD0;
	float3 T 		: TANGENT; //in object space
	float3 B 		: BINORMAL; //in object space	
};

sampler2D DiffuseSampler = sampler_state
{
	Texture = <tDiffuse>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};


// -------------------------------------------------------------
// shadow map texture stage states
// we try linear filtering here ... in case of fp textures it 
// is not supported on some graphics hardware
// -------------------------------------------------------------
texture ShadowMap1 : ShadowMap;
sampler2D g_samShadowMap = sampler_state
{
   Texture = <ShadowMap1>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = NONE;
   AddressU  = Clamp;
   AddressV  = Clamp;

};
texture tBlurVMap : BlurTexture;


sampler BlurVSampler = sampler_state
{
	texture = (tBlurVMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 N_Depth  : TEXCOORD0;		//Normal
	float4 uv_ClipPosZW : TEXCOORD1;
	float3 View : TEXCOORD2;
	float4 ShadowMapUV[4]	: TEXCOORD3;
};
	
// -------------------------------------------------------------
VS_OUTPUT VS(v2in v)
{
	VS_OUTPUT Out;
	Out.Pos = mul(v.Pos, matWorldVP);
	Out.uv_ClipPosZW.zw = Out.Pos.zw;
	
	
	Out.uv_ClipPosZW.xy = v.uv;
	Out.N_Depth.xyz = (mul(v.Norm, (float3x3)matWorld)); //transform the normal
	float4 Pw = mul(v.Pos, matWorld);

	Out.View = (matViewInverse[3].xyz - Pw); //vecEye
	
	// transform projected textures
	// == mLightViewProj * texture adjustment matrix
	// mScale * mTransform * mRotate * mView * mProj * mScaleBias
	Out.ShadowMapUV[0] = mul(v.Pos, mLightViewProjTexAdj);
	Out.ShadowMapUV[1] = mul(v.Pos, mLightViewProjTexAdj2);
	Out.ShadowMapUV[2] = mul(v.Pos, mLightViewProjTexAdj3);
	Out.ShadowMapUV[3] = mul(v.Pos, mLightViewProjTexAdj4);

	// measure distance between light and point
	// transform light source
	// mScale * mTransform * mRotate * mView * mProj
	Out.N_Depth.w = (mul(v.Pos, g_mWorldView)).z;

	return Out;
}

// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{
	float2 uv = In.uv_ClipPosZW.xy;
	float4 texColor = tex2D(DiffuseSampler, uv);

	float depth = In.N_Depth.w;
	float3 splitDepth;
	float  split;

	splitDepth.x = step( g_fSplitPlane[0], depth );
	splitDepth.y = step( g_fSplitPlane[1], depth );
	splitDepth.z = step( g_fSplitPlane[2], depth );
	split = (splitDepth.x + splitDepth.y + splitDepth.z);

	float4 ShadowMapUV = In.ShadowMapUV[split];

	//pick the correct shadowmap split from the atlas
	ShadowMapUV.x = (ShadowMapUV.x * 0.5) + gSunAtlasShift[split].x;
	ShadowMapUV.y = (ShadowMapUV.y * 0.5) + gSunAtlasShift[split].y;

	float dist_to_light = 1 - depth / g_fSplitPlane[3];
	float fLightingFactor = compute_lighting_factor_VSM(g_samShadowMap, dist_to_light, ShadowMapUV);

    //compute specular reflection
	float4 L = normalize(-lightDir); //normalized light direction
	float3 Half = normalize(L + normalize(In.View)); //saves us a normalize call when we use the half vector	
	float3 N = normalize(In.N_Depth.xyz);

	//N.H^n - specular
	float specular = pow(saturate(dot(N, Half)), 25);

	float diffLight = saturate(dot(L, N.xyz)); //Diff
	diffLight = max(diffLight, shadowStrength); //get rid of shadow
	
	//self shadowing term
	float selfShadow = saturate(4 * dot(N.xyz, L));
	selfShadow = max(selfShadow, shadowStrength); //get rid of shadow

	// multiply diffuse with shadowmap lookup value
	float shadowTerm = max(fLightingFactor, shadowStrength);

	float4 finalColor = AmbColor + (selfShadow * (texColor * shadowTerm) * diffLight + (SpecularIntensity * specular));

	return float4(finalColor.xyz, shadowTerm);
}

// -------------------------------------------------------------
technique diffuseBump
<
	string shaderDescription = "shadow";
>
{

	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
		
	}
}


// -------------------------------------------------------------
struct VS_OUTPUTCREATESHADOWMAP
{
	float4 Pos		: POSITION;
	float2 Depth	: TEXCOORD0;
};

// -------------------------------------------------------------
VS_OUTPUTCREATESHADOWMAP VSCreateShadowMap(v2in v)
{
	VS_OUTPUTCREATESHADOWMAP Out = (VS_OUTPUTCREATESHADOWMAP)0;

	Out.Pos = mul( v.Pos, mLightViewProj);
	Out.Depth = Out.Pos.zw;

	return Out;
}

// -------------------------------------------------------------
// Pixel shader that creates shadow map
// -------------------------------------------------------------
float4 PSCreateShadowMap(float2 depth : TEXCOORD0) : COLOR
{
	float2 xdepth;
	xdepth.x = depth.x / depth.y;
	xdepth.y = xdepth.x * xdepth.x;

	return xdepth.xyxy;
}

// -------------------------------------------------------------
technique diffuseBump_Shadow
<
	string shaderDescription = "shadow";
>
{

	pass p0 
	{
		// render back faces to hide artifacts - rno it makes the depth map harder to debug though
		CullMode = CW;

		VertexShader = compile vs_2_0 VSCreateShadowMap();
		PixelShader = compile ps_2_0 PSCreateShadowMap();
		
	}
}


// -------------------------------------------------------------
struct VS_OUTPUT_GS_BUFFER
{
	float4 Pos		: POSITION;
	float3 N  : TEXCOORD0;
	float4 VPos		: TEXCOORD1;
	float2 uv		: TEXCOORD2;
};

// -------------------------------------------------------------
VS_OUTPUT_GS_BUFFER VS_GBUFFER(v2in v)
{
	VS_OUTPUT_GS_BUFFER Out;
	Out.Pos = mul(v.Pos, matWorldVP);
	Out.uv = v.uv;

	Out.N = mul(v.Norm, (float3x3)matWorld); //transform the normal

	float4 Pw = mul(v.Pos, matWorld);
	Out.VPos = mul(v.Pos, g_mWorldView);

	return Out;
}

// -------------------------------------------------------------
struct PS_OUTPUT_GBUFFER
{
	float4 material	: COLOR0;
	float4 normal	: COLOR1;
	float4 depth 	: COLOR2;
};

// -------------------------------------------------------------
PS_OUTPUT_GBUFFER PS_GBuffer(VS_OUTPUT_GS_BUFFER In)
{
	PS_OUTPUT_GBUFFER Out;

	float2 uv = In.uv;
    float4 texColor = tex2D(DiffuseSampler, uv);
    
	//store shadow value
	Out.material = texColor;
	Out.normal = float4( (normalize(In.N) * 0.5)+0.5, SpecularIntensity);

    // Negate and divide by distance to far-clip plane
    // (so that depth is in range [0,1])
    // This is for right-handed coordinate system,
    // for left-handed negating is not necessary.
	Out.depth = float4(In.VPos.z / g_farClip,0,0,1);


	return Out;
}

// -------------------------------------------------------------
technique diffuseBump_GBuffer
<
	string shaderDescription = "shadow";
>
{

	pass p0 
	{
		VertexShader = compile vs_2_0 VS_GBUFFER();
		PixelShader = compile ps_2_0 PS_GBuffer();
		
	}
}

