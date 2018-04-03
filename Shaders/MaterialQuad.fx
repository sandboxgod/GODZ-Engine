//-----------------------------------------------------------------------------
// Editor fx, Renders a Material Quad
//-----------------------------------------------------------------------------

float4x4 matWorld : World;
float4x4 matVP : ViewProjection;
texture tDiffuse : Diffuse;		//color map
texture tBump : Normal;			//normal map
float4 worldUp : CameraUp;

//Since we use texture coordinates to translate the quad, we have to perform scale here.
float scaleX = 64;
float scaleY = 64;

sampler2D color_map = sampler_state
{
	Texture = <tDiffuse>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler normal_map = sampler_state
{
	Texture = <tBump>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

//-----------------------------------------------------------------------------
// Vertex Definitions
//-----------------------------------------------------------------------------

struct VS_INPUT
{
    float4 Pos	: POSITION; //xyz
	float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos : POSITION;
	float2 TopLeft : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// Simple Vertex Shader
//-----------------------------------------------------------------------------

VS_OUTPUT VS( VS_INPUT IN )
{
    VS_OUTPUT Out;

	Out.Pos.x = IN.Pos.x;
	Out.Pos.y = IN.Pos.y;
	Out.Pos.z = IN.Pos.z;
	Out.Pos.w = 1.0f;
	

	Out.TopLeft = IN.uv;
	return Out;
}

//-----------------------------------------------------------------------------
// Simple Pixel Shader
//-----------------------------------------------------------------------------

PS_OUTPUT PS( VS_OUTPUT IN )
{
    PS_OUTPUT Out;

	float4 texColor = tex2D(color_map, IN.TopLeft);

	Out.color = float4(texColor.xyz,1);

    return Out;
}

//-----------------------------------------------------------------------------
// Simple Effect (1 technique with 1 pass)
//-----------------------------------------------------------------------------

technique diffuse
{
    pass Pass0
    {
		CullMode = None;

		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS();
    }
}

VS_OUTPUT VS_Billboard( VS_INPUT IN )
{
    VS_OUTPUT Out;
	
	half3 eyeVector = matVP._m02_m12_m22;
	
    float3 center = mul(IN.Pos, matWorld);

    float3 side = normalize(cross(eyeVector.xyz,worldUp.xyz));
    float3 up = normalize(cross(side,eyeVector));

    float3 finalPosition = center;
    finalPosition += (IN.uv.x-0.5f)*side*scaleX;
    finalPosition += (0.5f-IN.uv.y)*up*scaleY;

    float4 finalPosition4 = float4(finalPosition, 1);
    
    Out.Pos = mul(finalPosition4, matVP);
	

	Out.TopLeft = IN.uv;
	return Out;
}


// -------------------------------------------------------------
struct PS_OUTPUT_GBUFFER
{
	float4 material	: COLOR0;
	float4 normal 	: COLOR1;
	float4 depth 	: COLOR2;
};

// -------------------------------------------------------------
PS_OUTPUT_GBUFFER PS_GBuffer(VS_OUTPUT In)
{
	PS_OUTPUT_GBUFFER Out;

	float4 texColor = tex2D(color_map, In.TopLeft);

	//The normal map is stored [0...1] so we have to map it back into the [-1...1] range
	//float3 Normal = (2 * (tex2D(normal_map, In.TopLeft))) - 1.0f;

	//store shadow value
	Out.material = float4(texColor.xyz,0);
	//Out.normal = float4(Normal.xyz,texColor.w);
	Out.normal = float4(1,1,1,1);

	//default z for a quad is 0
	Out.depth = float4(0.1,0,0,1);

	return Out;
} 

// -------------------------------------------------------------
technique diffuse_GBuffer
{

	pass p0 
	{
		CullMode = None;

		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS_GBuffer();
	}
}



//forward rendering
technique billboard
{
    pass Pass0
    {
		CullMode = None;
		AlphaBlendEnable = True;
		AlphaTestEnable = True;
		AlphaRef = 0;
		SrcBlend = One;
		DestBlend = One;

		VertexShader = compile vs_2_0 VS_Billboard();
		PixelShader  = compile ps_2_0 PS();
    }
}

