//-----------------------------------------------------------------------------
// Editor fx, Renders an XYZ gizmo
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Effect File Variables
//-----------------------------------------------------------------------------

float4x4 worldViewProj : WorldViewProjection; // This matrix will be loaded by the application


//-----------------------------------------------------------------------------
// Vertex Definitions
//-----------------------------------------------------------------------------

// Our sample application will send vertices 
// down the pipeline laid-out like this...

struct VS_INPUT
{
    float3 position	: POSITION;
	float4 color : COLOR0;
};

// Once the vertex shader is finished, it will 
// pass the vertices on to the pixel shader like this...

struct VS_OUTPUT
{
    float4 hposition : POSITION;
    float4 color	 : COLOR0;
};

// And finally, the pixel shader will send a single 
// color value to the frame buffer like this...

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// Simple Vertex Shader
//-----------------------------------------------------------------------------

VS_OUTPUT myvs( VS_INPUT IN )
{
    VS_OUTPUT OUT;

	//OUT.hposition = mul( worldViewProj, float4(IN.position, 1) );

    float4 finalPos = {IN.position, 1};
    
    OUT.hposition = mul(finalPos, worldViewProj );

	OUT.color = IN.color; // Pass white as a default color

	return OUT;
}

//-----------------------------------------------------------------------------
// Simple Pixel Shader
//-----------------------------------------------------------------------------

PS_OUTPUT myps( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;

	OUT.color = IN.color;

    return OUT;
}

//-----------------------------------------------------------------------------
// Simple Effect (1 technique with 1 pass)
//-----------------------------------------------------------------------------

technique diffuse
{
    pass Pass0
    {
		Lighting = FALSE;
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		VertexShader = compile vs_2_0 myvs();
		PixelShader  = compile ps_2_0 myps();
    }
}



// -------------------------------------------------------------
struct PS_OUTPUT_GBUFFER
{
	float4 material	: COLOR0;
};

// -------------------------------------------------------------
PS_OUTPUT_GBUFFER PS_GBuffer(VS_OUTPUT In)
{
	PS_OUTPUT_GBUFFER Out;

	//store shadow value
	Out.material = In.color;

	return Out;
} 

// -------------------------------------------------------------
technique diffuse_GBuffer
{

	pass p0 
	{
		VertexShader = compile vs_2_0 myvs();
		PixelShader = compile ps_2_0 PS_GBuffer();
		
	}
}
