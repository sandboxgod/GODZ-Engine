#include "Shadows.fxh"

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


// 1 over the width and height of the framebuffer (for correct texture sampling)
float fRcpFrameWidth;
float fRcpFrameHeight;

// sky and ground colour of the hemisphere light
float4 lightColor		= float4(0.8,0.8,0.1,1);	// colour of the light
float4 lightPosition		= float4(0,10,10,1);	// point of the light
float radius			= 1.0f;					// the 1.f / the lights radius (distance of total fall off)

float4x4 matWorldViewProjection: WorldViewProjection;
float4x4 matViewInverse : ViewInverse;
float4x4 g_mWorldView : WorldView;

float4 cameraPos : CameraEyePosition;
float g_farClip : FARPLANE;

//diffuse intensity
float4 DiffuseI
<
	string UIName = "Diffuse Intensity";
> = {1.0,1.0,1.0,1.0}; 

//specular intensity
float4 SpecularI
<
	string UIName = "Specular Intensity";
> = {0.8,0.8,0.8,0.8};

//How much shadow to allow (1.0 is max, 0.0f is min)
float shadowStrength
<
	string UIName = "Shadow Threshold";
> = 0.5f;

//control the brightness of the light
float lightIntensity = 1.0f;

//-----------------------------------------------------------
//SpotLight

float4x4 lightVP : LightViewProjection1;
float4x4 matViewProjInv : ViewProjectionInverse;

//shadowmap for spotlights
texture ShadowMap;
sampler2D g_samShadowMap = sampler_state
{
   Texture = <ShadowMap>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = LINEAR;
   AddressU  = Clamp;
   AddressV  = Clamp;
};

float cosInner = 1; //[0.5...1.0]
float cosOuter = 0; //[0.5...1.0]
float4 spotLightDir		= float4(0.8,0.8,0.1,1);	// actual direction of the spotlight
float2 g_shadowMapTexSizeOverOne : ShadowMapTexSizeOverOne;

float3 VSPositionFromDepth(float4 vTexCoord, float3 vPositionVS)
{
    // Calculate the frustum ray using the view-space position.
    // g_fFarCip is the distance to the camera's far clipping plane.
    // Negating the Z component only necessary for right-handed coordinates
    float3 vFrustumRayVS = vPositionVS.xyz * (g_farClip/vPositionVS.z);
    return tex2Dproj(GBufferDepthSampler, vTexCoord).x * vFrustumRayVS;
}

// -------------------------------------------------------------
// Output channels
// -------------------------------------------------------------

struct VS_OUT
{
	float4 Pos				: POSITION;
	float4 ScreenXY			: TEXCOORD0;
	float4 ScreenPosition	: TEXCOORD1;
};

VS_OUT LightPointConvexVS( float4 Pos : POSITION )
{
	VS_OUT Out;
	
	// tranform local space position into HCLIP space
	Out.Pos = mul(Pos, matWorldViewProjection );

	Out.ScreenPosition = mul(Pos, g_mWorldView);

	
	Out.ScreenXY = Out.Pos;
	
	Out.ScreenXY.xy = float2(  Out.ScreenXY.x * 0.5, -Out.ScreenXY.y * 0.5 );

	Out.ScreenXY.xy += (0.5 * Out.ScreenXY.w);
	Out.ScreenXY.x += 0.5f * fRcpFrameWidth * Out.ScreenXY.w;
	Out.ScreenXY.y += 0.5f * fRcpFrameHeight * Out.ScreenXY.w;


	return Out;
}

struct PS_OUTPUT
{
	float4 diffuse	: COLOR0;
	float4 specular	: COLOR1;
};

/*
Regardless of pixel or vertex shader, you'd do this:

lightvec = (vertexpos - lightpos);
lightdist = length(lightvec);
lightvec = normalize(lightvec);

You can then apply attenutation based on the distance, do a directional light calculation based on the lightvector and the normal. 
The pointlight color value is (lightvec dot normal) * attenuation * lightcolor * materialcolor.
*/
PS_OUTPUT PS(VS_OUT vin)
{
	PS_OUTPUT Out;

	float3 viewPos = VSPositionFromDepth(vin.ScreenXY, vin.ScreenPosition.xyz);
	float4 worldPos = mul(float4(viewPos,1), matViewInverse);

	float4 normColor = tex2Dproj(GBufferNormalSampler, vin.ScreenXY);

	// Getting the materials
	float3 normal = normColor.xyz*2.0 - 1.0;

	float4 L = lightPosition - worldPos;

	//Attenuation -- light pixels within our radius; pixels that fall outside of it shouldn't get lit...
	float attenuation = saturate(1.0f - length(L)/radius);

	float diffuse 			= saturate(dot(L, normal));
	float3 diffuseColor 	= diffuse * lightColor.rgb;

	float3 View				= normalize(matViewInverse[3].xyz - worldPos.xyz);
	float3 H				= normalize(View + L.xyz);
	float specular			= min(pow(saturate(dot(normal, H)), 25), normColor.w);
	float3 specullarColor	= specular * SpecularI ;

	Out.diffuse		= attenuation * float4(diffuseColor, 1.0);
	Out.specular	= attenuation * float4(specullarColor, 1.0);

	return Out;
}

//useful for debugging
half4 ShowVolume0( float4 uv : TEXCOORD0 ) : COLOR0
{
	return half4(lightColor.x,lightColor.y,lightColor.z,1);
}


technique RenderPointLight
<
	string shaderDescription = "RenderPointLight";
>
{

	pass p0
	{
		VertexShader = compile vs_2_0 LightPointConvexVS();
		PixelShader = compile ps_2_0 PS();

		
		AlphaBlendEnable = True;
		AlphaTestEnable = True;
		AlphaRef = 0;
		SrcBlend = One;
		DestBlend = One;

	
		// render back faces only, with an infinate far plane and no z-testing
		// guarentees a convex object only touchs each pixel once and once only
		//CullMode = CCW; //CCW;

		//turned off culling for now until I apply cullmodes properly; see http://forums.xna.com/forums/p/32573/270969.aspx
		CullMode = None;
		ZEnable = false;
		ZWriteEnable = false;
	}
}

float lerpstep(float lower, float upper, float s)
{
	return saturate( (s - lower) / (upper - lower) );
}

struct PS_SPOTLIGHT_OUTPUT
{
	float4 diffuse	: COLOR0;
};

PS_SPOTLIGHT_OUTPUT PS_ShadowSpotLight(VS_OUT vin)
{
	PS_SPOTLIGHT_OUTPUT Out;

	float4 normColor = tex2Dproj(GBufferNormalSampler, vin.ScreenXY);

	float3 viewPos = VSPositionFromDepth(vin.ScreenXY, vin.ScreenPosition.xyz);
	float4 worldPos = mul(float4(viewPos,1), matViewInverse);

	// Getting the materials
	float3 normal = normColor.xyz*2.0 - 1.0;

	float4 L = lightPosition - worldPos;

	float dist_to_light = length(L);

	//Attenuation -- light pixels within our radius; pixels that fall outside of it shouldn't get lit...
	float attenuation = saturate(1.0f - dist_to_light/radius); //radius

	float3 LightDir = normalize(L.xyz);

	float4 ShadowMapUV = mul(worldPos, lightVP);
	ShadowMapUV /= ShadowMapUV.w;
	
	float rescaled_dist = dist_to_light / radius; //scaled between 0...1, we want dist_to_light / light_atten_end
	float fLightingFactor = compute_lighting_factor_VSM(g_samShadowMap, rescaled_dist, ShadowMapUV);

	float shadowTerm = max(fLightingFactor, shadowStrength);

	// Compute the spot light cut-off angle
	float cosAngle = dot( spotLightDir, LightDir );
	//float fSpotTerm = lerpstep( cosOuter, cosInner, cosAngle );
	float fSpotTerm = smoothstep( cosOuter, cosInner, cosAngle );



	//float diffuse 			= saturate(dot(L, normal));
	float diff = saturate(dot(normal, LightDir));
	float4 diffuseColor 	= float4(diff * lightColor.rgb * shadowTerm, 1);

	Out.diffuse = diffuseColor * attenuation * fSpotTerm ;

	return Out;
}

technique RenderShadowSpotLight
<
	string shaderDescription = "RenderSpotLight";
>
{
	pass p0
	{
		VertexShader = compile vs_2_0 LightPointConvexVS();
		PixelShader = compile ps_2_0 PS_ShadowSpotLight(); //ShowVolume0(); //PS_ShadowSpotLight();

		
		AlphaBlendEnable = True;
		AlphaTestEnable = True;
		AlphaRef = 0;
		SrcBlend = One;
		DestBlend = One;
		

	
		// render back faces only, with an infinate far plane and no z-testing
		// guarentees a convex object only touchs each pixel once and once only
		//CullMode = CCW; //CCW;

		//turned off culling for now until I apply cullmodes properly; see http://forums.xna.com/forums/p/32573/270969.aspx
		CullMode = None;
		ZEnable = false;
		ZWriteEnable = false;
		
	}
}

PS_SPOTLIGHT_OUTPUT PS_SpotLight(VS_OUT vin)
{
	PS_SPOTLIGHT_OUTPUT Out;

	float4 normColor = tex2Dproj(GBufferNormalSampler, vin.ScreenXY);

	float3 viewPos = VSPositionFromDepth(vin.ScreenXY, vin.ScreenPosition.xyz);
	float4 worldPos = mul(float4(viewPos,1), matViewInverse);

	// Getting the materials
	float3 normal = normColor.xyz*2.0 - 1.0;

	float4 L = lightPosition - worldPos;
	float3 LightDir = normalize(L.xyz);

	// Compute the spot light cut-off angle
	float cosAngle = dot( spotLightDir, LightDir );
	float fSpotTerm = smoothstep( cosOuter, cosInner, cosAngle );

	//Attenuation -- light pixels within our radius; pixels that fall outside of it shouldn't get lit...
	float attenuation = saturate(1.0f - length(L)/radius); //radius

	//float diffuse 			= saturate(dot(L, normal));
	float diff = saturate(dot(normal, LightDir));
	float4 diffuseColor 	= float4(diff * lightColor.rgb, 1);

	Out.diffuse = diffuseColor * attenuation * fSpotTerm;

	return Out;
}

technique RenderSpotLight
<
	string shaderDescription = "RenderSpotLight";
>
{
	pass p0
	{
		VertexShader = compile vs_2_0 LightPointConvexVS();
		PixelShader = compile ps_2_0 PS_SpotLight(); //ShowVolume0(); //PS_SpotLight();

		
		AlphaBlendEnable = True;
		AlphaTestEnable = True;
		AlphaRef = 0;
		SrcBlend = One;
		DestBlend = One;
		

	
		// render back faces only, with an infinate far plane and no z-testing
		// guarentees a convex object only touchs each pixel once and once only
		//CullMode = CCW; //CCW;

		//turned off culling for now until I apply cullmodes properly; see http://forums.xna.com/forums/p/32573/270969.aspx
		CullMode = None;
		ZEnable = false;
		ZWriteEnable = false;
		
	}
}
