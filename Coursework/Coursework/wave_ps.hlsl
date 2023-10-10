Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[2];
    float4 diffuse[2];
    float4 position[2];
    float4 specularPower[2];
    float4 specular[2];
    float4 direction[2];
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};



float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;

}


float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
    // blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}


float4 main(InputType input) : SV_TARGET
{
    
    float4 lightColourResult = float(0);
    float4 waveTexColour = float(0);
    float4 lightColour = float(0);
    
    waveTexColour = texture0.Sample(sampler0, input.tex);
    
    
    if (specularPower[0].a == 1)
    { 
     // render normals as colour
        
        waveTexColour.xyz = input.normal.xyz * 0.5 + 0.5;
        
        return waveTexColour;
    }
    
    // directional light
    float3 dir = normalize(-direction[0]);     
    // specular lighting
    float4 specularCol = calcSpecular(dir, input.normal, input.viewVector, specular[0], specularPower[0].x);
    lightColourResult +=  ambient[0] + calculateLighting(dir, input.normal, diffuse[0]) +specularCol;
    
   
     
    // point light
    float3 lightVector = normalize(position[1].xyz - input.worldPosition);

    // values for attenuation 
    float constantFactor = 0.3f;
    float linearFactor = 0.075f;
    float quadraticFactor = 0.0001f;
	
    float3 distanceVec = position[1].xyz - input.worldPosition; // distance between light and geometry	
    float distance = length(distanceVec);
    float attenuation = 1 / (constantFactor + (linearFactor * distance) + (quadraticFactor * pow(distance, 2)));
    float4 diffuseVal = diffuse[1] * saturate(attenuation);
    lightColourResult += specularCol + ambient[1] + calculateLighting(lightVector, input.normal, diffuseVal);
    

    
    
        return lightColourResult * waveTexColour;
}