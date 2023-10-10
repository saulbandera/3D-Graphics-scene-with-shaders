cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix shadowMatrices[6];
    matrix projectionMatrix;
};

struct InputType
{
    float4 position : SV_POSITION;
};

struct OutputType
{
    float4 position : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void main(triangle InputType input[3], inout TriangleStream<OutputType> triStream)
{
    // receive one triangle and output six, one for each face of the depth cube 
    for (int face = 0; face < 6; ++face){
         
            OutputType output;
            output.RTIndex = face; // the face of the texture cube that is going to be output to
            for (int i = 0; i < 3; ++i){         
                float4 worldPosition = input[i].position;
            
                // transform world space input vertices into light space
                float4 viewPosition = mul(worldPosition, shadowMatrices[face]); 
                output.position = mul(viewPosition, projectionMatrix);
                triStream.Append(output);
            }
        triStream.RestartStrip();
       
    }	
}

