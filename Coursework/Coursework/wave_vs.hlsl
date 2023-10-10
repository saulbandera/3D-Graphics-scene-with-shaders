cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}


cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
};


cbuffer TimeBuffer : register(b2)
{
    float time;
    float defSpeed;
    float defAmplitude;
    float defSteepness;
    float2 PI_wavelength;
}

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};

OutputType main(InputType input)
{
    OutputType output;
    
    // defining three waves to add complexity to the wave pattern in water
    
    // WAVE 1 
    float wavenumber = (2 * PI_wavelength.x) / PI_wavelength.y; // 2*pi/wavelength - frequency of waves
    float amplitude = defAmplitude; // height of waves
    float WA = wavenumber * amplitude; // reduce the number of calculations 
    float phaseSpeed = defSpeed * wavenumber; // speed of wave movement
    float2 direction; 
    direction = normalize(float2(0.5, 1)); // direction in which the waves travel 
    float var = wavenumber * dot(direction, input.position.xz) + phaseSpeed * time; // reduce num calculations
    float steepness = defSteepness / (wavenumber * amplitude * 3); // horizontal amplitude(steepness)
    
    // WAVE 2
    float wavenumber2 = (2 * PI_wavelength.x) / (PI_wavelength.y * 1.4);
    float amplitude2 = defAmplitude * 0.9;
    float WA2 = wavenumber2 * amplitude2;
    float phaseSpeed2 = defSpeed * 0.6 * wavenumber2;
    float2 direction2;
    direction2 = normalize(float2(1, 0.5));
    float var2 = wavenumber2 * dot(direction2, input.position.xz) + phaseSpeed2 * time;
    float steepness2 = defSteepness / (wavenumber2 * amplitude2 * 3);
           
    // WAVE 3
    float wavenumber3 = (2 * PI_wavelength.x) / (PI_wavelength.y * 2.7);
    float amplitude3 = defAmplitude * 0.6;
    float WA3 = wavenumber3 * amplitude3;
    float phaseSpeed3 = defSpeed * 1.2 * wavenumber3;
    float2 direction3;
    direction3 = normalize(float2(1, 0));
    float var3 = wavenumber3 * dot(direction3, input.position.xz) + phaseSpeed3 * time;
    float steepness3 = defSteepness / (wavenumber3 * amplitude3 * 3);
    
    
   
    float3 position; // input position
    position.x = input.position.x;
    position.y = input.position.y;
    position.z = input.position.z;
   
   
    // gerstner wave formula
    input.position.x += steepness * amplitude * direction.x * cos(var) + steepness2 * amplitude2 * direction2.x * cos(var2) + steepness3 * amplitude3 * direction3.x * cos(var3);
    input.position.y = amplitude * sin(var) + amplitude2 * sin(var2) + amplitude3 * sin(var3);
    input.position.z += steepness * amplitude * direction.y * cos(var) + steepness2 * amplitude2 * direction2.y * cos(var2) + steepness3 * amplitude3 * direction3.y * cos(var3);
      
    // calculate normals using the cross product of the tangent and the bitangent of the gerstner wave
    input.normal.x = -(direction.x * WA * cos(wavenumber * dot(direction, input.position.xz) + phaseSpeed * time)     + direction2.x * WA2 * cos(wavenumber2 * dot(direction2, input.position.xz) + phaseSpeed2 * time) + direction3.x * WA3 * cos(wavenumber3 * dot(direction3, input.position.xz) + phaseSpeed3 * time));
    input.normal.y = 1 - (steepness * WA * sin(wavenumber * dot(direction, input.position.xz) + phaseSpeed * time)    + steepness2 * WA2 * sin(wavenumber2 * dot(direction2, input.position.xz) + phaseSpeed2 * time) + steepness3 * WA3 * sin(wavenumber3 * dot(direction3, input.position.xz) + phaseSpeed3 * time));
    input.normal.z = -(direction.y * WA * cos(wavenumber * dot(direction, input.position.xz) + phaseSpeed * time)     + direction2.y * WA2 * cos(wavenumber2 * dot(direction2, input.position.xz) + phaseSpeed2 * time) + direction3.y * WA3 * cos(wavenumber3 * dot(direction3, input.position.xz) + phaseSpeed3 * time));
  
    
    
    

    
	// calculate the position of the vertex against the worldirection, view, and projection matrices
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // store the texture coordinates for the pixel shader
    output.tex = input.tex; 
  
    //lighting calculations
    float4 worldPosition = mul(input.position, worldMatrix);
    output.viewVector = cameraPosition.xyz - worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);
    output.worldPosition = mul(input.position, worldMatrix).xyz;

    // calculate the normal vector against the world matrix only and normalize
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    return output;


}