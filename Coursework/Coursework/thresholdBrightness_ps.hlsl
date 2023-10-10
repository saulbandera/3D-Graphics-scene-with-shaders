Texture2D texture0 : register(t0);
SamplerState Sampler : register(s0);


cbuffer thresholdBuffer : register(b0)
{
    float4 threshold;
    float4 colour;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    float4 pixel = texture0.Sample(Sampler, input.tex);
    if (pixel.x >= threshold.x && pixel.y >= threshold.y && pixel.z >= threshold.z){     
        pixel += (colour / 3); // apply colour that can be changed in user interface
        return pixel;
    }
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}


    // check if the pixel colour is higher than a certain threshold, if it is then the returned value is the coloured pixel, if not then black is returned
