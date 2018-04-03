/*-----------------------------------------------------------------------------
	Name	: Soft Shadows.fx
	Desc	: Soft shadows effect file.
	Author	: Anirudh S Shastry. Copyright (c) 2004.
	Date	: 22nd June, 2004.
-----------------------------------------------------------------------------*/

//--------------------------------------------
// Global variables
//--------------------------------------------
float2  g_vSampleOffsets[15];
float   g_fSampleWeights[15];

texture tScreenMap;
texture tBlurHMap;

sampler ScreenSampler = sampler_state
{
	texture = (tScreenMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler BlurHSampler = sampler_state
{
	texture = (tBlurHMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};



//--------------------------------------------
// Vertex shaders
//--------------------------------------------
struct VSOUTPUT_BLUR
{
	float4 vPosition	: POSITION;
	float2 vTexCoord	: TEXCOORD0;
};

// Gaussian filter vertex shader
VSOUTPUT_BLUR VS_Blur( float4 inPosition : POSITION, float2 inTexCoord : TEXCOORD0 )
{
	// Output struct
	VSOUTPUT_BLUR OUT = (VSOUTPUT_BLUR)0;

	// Output the position
	OUT.vPosition = inPosition;

	// Output the texture coordinates
	OUT.vTexCoord = inTexCoord;

	return OUT;
}

//--------------------------------------------
// Pixel shaders
//--------------------------------------------

// Horizontal blur pixel shader
float4 PS_BlurH15x15( VSOUTPUT_BLUR IN ) : COLOR0
{
	// Accumulated color
	float4 vAccum = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Sample the taps (g_vSampleOffsets holds the texel offsets and g_fSampleWeights holds the texel weights)
	for( int i = 0; i < 15; i++ )
	{
		vAccum += tex2D( ScreenSampler, IN.vTexCoord + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
	}

	return vAccum;
}

// Vertical blur pixel shader
float4 PS_BlurV15x15( VSOUTPUT_BLUR IN ) : COLOR0
{
	// Accumulated color
	float4 vAccum = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Sample the taps (g_vSampleOffsets holds the texel offsets and g_fSampleWeights holds the texel weights)
	for( int i = 0; i < 15; i++ )
	{
		vAccum += tex2D( BlurHSampler, IN.vTexCoord + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
	}

	return vAccum;
}

// Horizontal blur pixel shader
float4 PS_BlurH5x5( VSOUTPUT_BLUR IN ) : COLOR0
{
	// Accumulated color
	float4 vAccum = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Sample the taps (g_vSampleOffsets holds the texel offsets and g_fSampleWeights holds the texel weights)
	for( int i = 0; i < 5; i++ )
	{
		vAccum += tex2D( ScreenSampler, IN.vTexCoord + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
	}

	return vAccum;
}

// Vertical blur pixel shader
float4 PS_BlurV5x5( VSOUTPUT_BLUR IN ) : COLOR0
{
	// Accumulated color
	float4 vAccum = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Sample the taps (g_vSampleOffsets holds the texel offsets and g_fSampleWeights holds the texel weights)
	for( int i = 0; i < 5; i++ )
	{
		vAccum += tex2D( BlurHSampler, IN.vTexCoord + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
	}

	return vAccum;
}

//--------------------------------------------
// Techniques
//--------------------------------------------
technique techBlurH
{
	pass p0
	{
		Lighting	= False;
		CullMode	= None;
		
		VertexShader = compile vs_2_0 VS_Blur();
		PixelShader  = compile ps_2_0 PS_BlurH15x15();
	}
}



technique techBlurV
{
	pass p0
	{
		Lighting	= False;
		CullMode	= None;
		
		VertexShader = compile vs_2_0 VS_Blur();
		PixelShader  = compile ps_2_0 PS_BlurV15x15();
	}
}

technique techBlurH5x5
{
	pass p0
	{
		Lighting	= False;
		CullMode	= None;
		
		VertexShader = compile vs_2_0 VS_Blur();
		PixelShader  = compile ps_2_0 PS_BlurH5x5();
	}
}



technique techBlurV5x5
{
	pass p0
	{
		Lighting	= False;
		CullMode	= None;
		
		VertexShader = compile vs_2_0 VS_Blur();
		PixelShader  = compile ps_2_0 PS_BlurV5x5();
	}
}
