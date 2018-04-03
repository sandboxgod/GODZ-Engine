//
//Matrix Palette Skinning & Tangent Space Bump Mapping shader by Richard Osborne
//Uses Jim Blinn Half Vector / traditional Reflection techniques
//Includes a Self Shadowing Term
//Gloss maps can be used
//Written Jan 26, 2006
//References: Programming Vertex & Pixel Shaders by Wolfgang Engel
#include "Shadows.fxh"

static const int MAX_MATRICES = 55;
float4x4 blendMatrices[MAX_MATRICES] : WORLDMATRIXARRAY;
float4x4 world_view_proj_matrix : WorldViewProjection;

float4x4 matWorld : World;
float4x4 g_mWorldView : WorldView;  //here for Fog

float4 sunlight : LightDirection;
float4x4 matView : ViewInverse;
texture tDiffuse : Diffuse;	//color map
texture tBump : NORMAL;
texture tBlurVMap : BlurTexture;

//wastes space since the gpu turns this into a vector4...
float2 gSunAtlasShift[4] : AtlasSplit;

int MaxNumBones=4; //max # of bones
static const int g_iNumSplits = 4;
float4 g_fSplitPlane : SplitDistances;

float g_shadowMapTexSize : ShadowMapTexSize;
int minNumOfShadowmaps : NUM_OF_SHADOWMAPS;
float2 g_shadowMapTexSizeOverOne : ShadowMapTexSizeOverOne;

float g_farClip : FARPLANE;

// transform light source
// mScale * mTransform * mRotate * mView * mProj
float4x4 mLightViewProj : LightViewProjection;

// transform projected textures
// == mLightViewProj * texture adjustment matrix
// mScale * mTransform * mRotate * mView * mProj * mScaleBias
float4x4 mLightViewProjTexAdj  : LightViewProjectionAdj;
float4x4 mLightViewProjTexAdj2 : LightViewProjectionAdj2;
float4x4 mLightViewProjTexAdj3 : LightViewProjectionAdj3;
float4x4 mLightViewProjTexAdj4 : LightViewProjectionAdj4;

//How much shadow to allow (1.0 is max, 0.0f is min)
float shadowStrength
<
	string UIName = "Shadow Threshold";
> = 0.3f;

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

///////////////////////////////////////////////////////
struct VS_INPUT
{
    float4  Pos		   : POSITION;	//make sure to use float4 here for the bone_matrix4x4.mul(pos) * weight
    float3  Normal     : NORMAL;
    float2  uv		   : TEXCOORD0;    
    float4  Weights    : TEXCOORD1;
    float4  Indices    : TEXCOORD2; 
    float3  Tangent    : TANGENT;
    float3  Binormal   : BINORMAL;        
};

sampler2D cmap = sampler_state
{
	Texture = <tDiffuse>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler NormSampler = sampler_state
{
	Texture = <tBump>;
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
	float3 uv_depth : TEXCOORD0;
	float3 ViewDir : TEXCOORD1;	//lightDir
	float3 L : TEXCOORD2;	//lightDir
	float4 ShadowMapUV[4]	: TEXCOORD3;
};

// -------------------------------------------------------------
VS_OUTPUT VS_Skin(VS_INPUT v)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	
    float3 tempPos={0,0,0};
    float3 tempNormal={0,0,0};
    float3 tempTangent={0,0,0};
    
    int index = v.Indices[0];
	tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[0];
	tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[0];
	tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[0];
    
    index = v.Indices[1];
	tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[1];
	tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[1];
	tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[1];
	
    index = v.Indices[2];
	tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[2];
	tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[2];
	tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[2];
	
    index = v.Indices[3];
	tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[3];
	tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[3];
	tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[3];
	
	Out.uv_depth.xy = v.uv;
	
    float4 finalPos = {tempPos.xyz, 1};
    
    // Transform the final skinned position. Here I use ViewProjection because the bone transform 
    // already contains the world matrix
    Out.Pos = mul(finalPos, world_view_proj_matrix );

	//Normal is taken directly from normal map - so no need to output the Normal
	float4 WorldPos = mul(finalPos, matWorld);
	
	tempTangent = normalize(tempTangent);
	tempNormal = normalize(tempNormal);
	
	float3x3 worldToTS; //world to tangent space
	worldToTS[0] = mul(tempTangent, matWorld);
	worldToTS[1] = mul(cross(tempTangent, tempNormal), matWorld);
	worldToTS[2] = mul(tempNormal, matWorld);
	
	Out.L= (mul(worldToTS, -sunlight)); //light dir
	
	Out.ViewDir = (mul(worldToTS, (matView[3].xyz - WorldPos)));

	// transform projected textures
	// == mLightViewProj * texture adjustment matrix
	// mScale * mTransform * mRotate * mView * mProj * mScaleBias
	Out.ShadowMapUV[0] = mul(finalPos, mLightViewProjTexAdj);
	Out.ShadowMapUV[1] = mul(finalPos, mLightViewProjTexAdj2);
	Out.ShadowMapUV[2] = mul(finalPos, mLightViewProjTexAdj3);
	Out.ShadowMapUV[3] = mul(finalPos, mLightViewProjTexAdj4);

	Out.uv_depth.z = mul(finalPos, g_mWorldView).z;

	return Out;
}

// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{
	float2 uv = In.uv_depth.xy;
    float4 texColor = tex2D(cmap, uv);

	float3 splitDepth;
	float  split;
	float depth = In.uv_depth.z;

	//worldPos --- not sure which space they expecting here... Hopefully world-space
	splitDepth.x = step( g_fSplitPlane[0], depth );
	splitDepth.y = step( g_fSplitPlane[1], depth );
	splitDepth.z = step( g_fSplitPlane[2], depth );
	split = (splitDepth.x + splitDepth.y + splitDepth.z);

	float4 ShadowMapUV = In.ShadowMapUV[split];

	//pick the correct shadowmap split from the atlas
	ShadowMapUV.x = (ShadowMapUV.x * 0.5) + gSunAtlasShift[split].x;
	ShadowMapUV.y = (ShadowMapUV.y * 0.5) + gSunAtlasShift[split].y;

	//need to compute the uv offsets now :(
	float dist_to_light = 1 - depth / g_fSplitPlane[3];
	float fLightingFactor = compute_lighting_factor_VSM(g_samShadowMap, dist_to_light, ShadowMapUV);


	//The normal map is stored [0...1] so we have to map it back into the [-1...1] range
	float3 Normal = (2 * (tex2D(NormSampler, uv))) - 1.0f;
	
	float3 L = normalize(In.L); //light dir
	float3 H = normalize(normalize(In.ViewDir) + L.xyz);


	//N.H^n
	float Specular = min(pow(saturate(dot(Normal, H)), 25), texColor.w);
	

	float Diff = saturate(dot(L, Normal));
	// multiply diffuse with shadowmap lookup value
	float shadowTerm = max(fLightingFactor, shadowStrength);
	//finalColor*= shadowTerm;
	float4 finalColor =  ( (texColor * Diff * shadowTerm) + (SpecularIntensity * Specular));

	return float4(finalColor.xyz, 1); //shadowTerm
}

// -------------------------------------------------------------
technique bumpBlinn
{
	pass p0
	{
		ZEnable = true;
		ZWriteEnable = true;
		CullMode = None;
		VertexShader = compile vs_2_0 VS_Skin();
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
VS_OUTPUTCREATESHADOWMAP VSCreateShadowMap(VS_INPUT v)
{
	VS_OUTPUTCREATESHADOWMAP Out;
	
    float3 tempPos={0,0,0};
    float3 tempNormal={0,0,0};
    float3 tempTangent={0,0,0};

    // bone transformation
	for(int i=0;i<MaxNumBones;i++)
	{
	    // First transformed position and normal
	    int index = v.Indices[i];
		tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[i];
	    tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[i];
	    tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[i];
    }    	
	
    float4 finalPos;
    finalPos.xyz = tempPos;
    finalPos.w = 1;
    
    // Transform the final skinned position. Here I use ViewProjection because the bone transform 
    // already contains the world matrix
    Out.Pos = mul(finalPos, mLightViewProj );
    
    //return linearized depth
    //Out.Depth = Out.Pos.z / Out.Pos.w;
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
technique bumpBlinn_Shadow
{
	pass p0
	{
		ZEnable = true;
		ZWriteEnable = true;
		
		// render back faces to hide artifacts - rno it makes the depth map harder to debug though
		CullMode = CW;

		VertexShader = compile vs_2_0 VSCreateShadowMap();
		PixelShader = compile ps_2_0 PSCreateShadowMap();
	}
}

// -------------------------------------------------------------
struct VS_OUTPUT_GBUFFER
{
	float4 Pos				: POSITION;
	float4 uv_ClipPosZW		: TEXCOORD0;
	float3 WorldTangent		: TEXCOORD1;
	float3 WorldBinormal	: TEXCOORD2;
	float3 WorldNormal		: TEXCOORD3;
	float4 VPos		: TEXCOORD4;
};

// -------------------------------------------------------------
VS_OUTPUT_GBUFFER VS_Skin_Gbuffer(VS_INPUT v)
{
	VS_OUTPUT_GBUFFER Out;
	
    float3 tempPos={0,0,0};
    float3 tempNormal={0,0,0};
    float3 tempTangent={0,0,0};
    
    int index = v.Indices[0];
	tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[0];
	tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[0];
	tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[0];
    
    index = v.Indices[1];
	tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[1];
	tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[1];
	tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[1];
	
    index = v.Indices[2];
	tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[2];
	tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[2];
	tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[2];
	
    index = v.Indices[3];
	tempPos += mul(v.Pos, blendMatrices[index]) * v.Weights[3];
	tempNormal += mul(v.Normal, (float3x3)blendMatrices[index]) * v.Weights[3];
	tempTangent += mul(v.Tangent, (float3x3)blendMatrices[index]) * v.Weights[3];
	
	Out.uv_ClipPosZW.xy = v.uv;
	
    float4 finalPos = {tempPos.xyz, 1};
    
    // Transform the final skinned position. Here I use ViewProjection because the bone transform 
    // already contains the world matrix
    Out.Pos = mul(finalPos, world_view_proj_matrix );

	Out.VPos = mul(finalPos, g_mWorldView );

	Out.uv_ClipPosZW.zw = Out.Pos.zw;
	
	tempTangent = normalize(tempTangent);
	tempNormal = normalize(tempNormal);

	float3x3 worldToTS; //world to tangent space
	Out.WorldTangent = mul(tempTangent, matWorld);
	Out.WorldBinormal = mul(cross(tempTangent, tempNormal), matWorld);
	Out.WorldNormal = mul(tempNormal, matWorld);
	
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
// Pixel shader for GBuffer render pass
// -------------------------------------------------------------
PS_OUTPUT_GBUFFER PS_Gbuffer(VS_OUTPUT_GBUFFER In) : COLOR
{
	PS_OUTPUT_GBUFFER Out;

	float2 uv = In.uv_ClipPosZW.xy;
    float4 texColor = tex2D(cmap, uv);
    
	//The normal map is stored [0...1] so we have to map it back into the [-1...1] range
	float3 Normal = (2 * (tex2D(NormSampler, uv))) - 1.0f;

	// Bring the normal to the world space
	float3x3 TangentToViewSpace = float3x3(In.WorldTangent, In.WorldBinormal, In.WorldNormal);
	Normal = mul(Normal, TangentToViewSpace);
	Normal = normalize(Normal);

	Out.material = texColor;
	Out.normal = float4( (Normal * 0.5)+0.5, SpecularIntensity);

    // Negate and divide by distance to far-clip plane
    // (so that depth is in range [0,1])
    // This is for right-handed coordinate system,
    // for left-handed negating is not necessary.
	Out.depth = float4(In.VPos.z / g_farClip,0,0,1);

	return Out;
}

// -------------------------------------------------------------
technique bumpBlinn_GBuffer
{
	pass p0
	{
		ZEnable = true;
		ZWriteEnable = true;
		CullMode = None;
		VertexShader = compile vs_2_0 VS_Skin_Gbuffer();
		PixelShader = compile ps_2_0 PS_Gbuffer();
	}
}




