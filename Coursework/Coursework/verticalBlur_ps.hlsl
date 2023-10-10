Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer HeightBuffer : register(b0)
{
    float height;
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

    float texelSize = 1.0f / height;
	// add the vertical pixels to the colour by the specific weight of each
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -4.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -3.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -2.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -1.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex) * weight;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 1.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 2.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 3.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 4.0f)) * weight4;
 
    colour.a = 1.0f;
    
    return saturate(colour);
}