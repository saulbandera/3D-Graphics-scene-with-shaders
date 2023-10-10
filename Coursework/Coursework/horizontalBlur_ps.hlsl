Texture2D brightAreasTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer WidthBuffer : register(b0)
{
    float width;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    float weight, weight1, weight2, weight3, weight4;
    float4 colour;

	// create the weights that each neighbor pixel will contribute to the blur
    weight =  0.3000f;
    weight1 = 0.2500f;
    weight2 = 0.2000f;
    weight3 = 0.1500f;
    weight4 = 0.1000f;  
   
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float texelSize = 1.0f / width;
    
	// add the horizontal pixels to the colour by the specific weight of each
    colour += brightAreasTexture.Sample(Sampler, input.tex + float2(texelSize * -4.0f, 0.0f)) * weight4;
    colour += brightAreasTexture.Sample(Sampler, input.tex + float2(texelSize * -3.0f, 0.0f)) * weight3;
    colour += brightAreasTexture.Sample(Sampler, input.tex + float2(texelSize * -2.0f, 0.0f)) * weight2;
    colour += brightAreasTexture.Sample(Sampler, input.tex + float2(texelSize * -1.0f, 0.0f)) * weight1;
    colour += brightAreasTexture.Sample(Sampler, input.tex) * weight;
    colour += brightAreasTexture.Sample(Sampler, input.tex + float2(texelSize * 1.0f, 0.0f)) * weight1;
    colour += brightAreasTexture.Sample(Sampler, input.tex + float2(texelSize * 2.0f, 0.0f)) * weight2;
    colour += brightAreasTexture.Sample(Sampler, input.tex + float2(texelSize * 3.0f, 0.0f)) * weight3;
    colour += brightAreasTexture.Sample(Sampler, input.tex + float2(texelSize * 4.0f, 0.0f)) * weight4;

    colour.a = 1.0f;

    return saturate(colour);
}
