Texture2D scene : register(t0);
Texture2D bloomedObject : register(t1);
SamplerState Sampler : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    float4 mainColour = scene.Sample(Sampler, input.tex);
    float4 bloomColour = bloomedObject.Sample(Sampler, input.tex);
    
    // add the scene texture with the bloomed object
    return saturate(mainColour + bloomColour);
}
