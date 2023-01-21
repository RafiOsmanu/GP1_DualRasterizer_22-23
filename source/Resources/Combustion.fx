//------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;

float4x4 gWorldMatrix : WORLD;
float4x4 gViewInverseMatrix : VIEWINVERSE;

bool gUseNormal = true;


float gPi = 3.14159265359f;

float gLightIntensity = 7.0f;

float gShininess = 25.0f;

float3 gLightDirection = float3(.577f, -.577f, .577f);


SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap; //or mirror. clamp, border
	AddressV = Wrap;//or mirror. clamp, border
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap; //or mirror. clamp, border
	AddressV = Wrap;//or mirror. clamp, border
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Wrap; //or mirror. clamp, border
	AddressV = Wrap;//or mirror. clamp, border
};

RasterizerState gRasterizerState
{
    CullMode = none;
    FrontCounterClockWise = false; //default
};

BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
	blendOp = add;
    srcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;
};

//------------------------------------------------------------------
// Input/Output Structs
//------------------------------------------------------------------

struct VS_INPUT
{
	float3 Position : POSITION;
    float3 WorldPosition : WORLD;
	float2 UV  : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD;
	float2 UV  : TEXCOORD;
    float3 Normal : NORMAL;
};

//------------------------------------------------------------------
// Vertex Shader
//------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.WorldPosition = mul(float4(input.WorldPosition, 1.f), gWorldMatrix);
	output.UV = input.UV;
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
	return output;
}

//------------------------------------------------------------------
// Pixel Shader
//------------------------------------------------------------------

float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(samPoint, input.UV);
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET 
{
    return gDiffuseMap.Sample(samLinear, input.UV);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET 
{
    return gDiffuseMap.Sample(samAnisotropic, input.UV);
}

//------------------------------------------------------------------
// Technique
//------------------------------------------------------------------
technique11 Techniques
{
	pass p0
	{
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Point()));
	}

	pass p1
	{
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
	}

	pass p2
	{
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
	}
}





