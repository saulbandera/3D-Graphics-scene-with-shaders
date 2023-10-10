cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
};

struct InputType
{
    float4 position : POSITION;
};

struct OutputType
{
    float4 position : SV_POSITION;
};

OutputType main(InputType input)
{
    OutputType output;

    // transform vertices to world space
    output.position = mul(input.position, worldMatrix);	
    return output;
}