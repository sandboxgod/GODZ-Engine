//-----------------------------------------------------------------------------
// skydome.fx
//-----------------------------------------------------------------------------
float g_farClip : FARPLANE;
float4x4 g_mWorldView : WorldView;
float4x4 matWorldVP : WorldViewProjection;
float4 skyColor
<
	string UIName = "Sky Color";
> = {0.1,0.5,0.8,1};

texture tDiffuse : Diffuse;	//color map
sampler2D color_map = sampler_state
{
	Texture = <tDiffuse>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

struct v2in
{
	float4 Pos : POSITION;
	float3 Norm  : NORMAL;
	float2 uv : TEXCOORD0;
	float3 T 	: TANGENT; //in object space
	float3 B 	: BINORMAL; //in object space
};

// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float2 Tex  : TEXCOORD0;
	float4 VPos	: TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

// -------------------------------------------------------------
VS_OUTPUT VS(v2in v)
{
	VS_OUTPUT Out;
	Out.Pos = mul(v.Pos, matWorldVP);
	Out.Tex = v.uv;
	Out.VPos = mul(v.Pos, g_mWorldView);

	return Out;
}

//-----------------------------------------------------------------------------
// Simple Pixel Shader
//-----------------------------------------------------------------------------

PS_OUTPUT PS( VS_OUTPUT IN )
{
    PS_OUTPUT Out;

	float4 texColor = tex2D(color_map, IN.Tex);
	Out.color = float4(texColor.xyz,1);
	//Out.color = skyColor;

    return Out;
}

// -------------------------------------------------------------
technique diffuse
{
	pass p0
	{
		CullMode = None;

		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;
	}
}

// -------------------------------------------------------------
struct PS_OUTPUT_GBUFFER
{
	float4 material	: COLOR0;
	//float4 normal	: COLOR1;
	//float4 depth 	: COLOR2;
};

// -------------------------------------------------------------
PS_OUTPUT_GBUFFER PS_GBuffer(VS_OUTPUT In)
{
	PS_OUTPUT_GBUFFER Out;

	float4 texColor = tex2D(color_map, In.Tex);

	//bend sky color with luminosity map
	Out.material = (0.1 * float4(0.0,0.1,0.7,1) ) + (0.9 * float4(texColor.xyz,1));
	//Out.material = float4(0.0,0.1,0.7,1);

	return Out;
}

// -------------------------------------------------------------
technique diffuse_GBuffer
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS_GBuffer();

		//ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;
	}
}

