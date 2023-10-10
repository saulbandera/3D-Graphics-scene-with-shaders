Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
TextureCube depthCubeMapTexture : register(t2);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[2];
    float4 diffuse[2];
    float4 position[2];
    float4 specularPower[2];
    float4 specular[2];
    float4 direction[2];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos : TEXCOORD1;
    float3 worldPosition : TEXCOORD2;
    float3 viewVector : TEXCOORD3;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

// Is the geometry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}


bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}



float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}





float vectorToDepth(float3 vec, float n, float f)
{
    // depth in space of a light source of a certain vector 
    float3 AbsVec = abs(vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));
    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float shadowCalculation(float3 pixelPos)
{
    
    // vector between pixel position and light position
    float3 pixelToLight = pixelPos - position[1].xyz;
    
    // sample from depth map using pixel to light vector 
    float depthValue = depthCubeMapTexture.Sample(shadowSampler, pixelToLight).r; 
    
    // linear depth between pixel and light 
    float depth = vectorToDepth(pixelToLight, 1.f, 100.f);  
    
    // test for shadows
    // if the linear depth between pixel and light is greater than the depth value sampled from the depth cube
    // using the same vector (pixelToLight), it means it is in shadow so 1 is returned 
    return (depth - 0.0001) > depthValue ? 1.0 : 0;
}




float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
    //blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}



float4 main(InputType input) : SV_TARGET
{    
    
 
    
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    float shadowMapBias = 0.001f;    
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
    
    
    if (specularPower[0].a == 1)
    {
        // render colour as normals
        textureColour.xyz = input.normal.xyz * 0.5 + 0.5;
        return textureColour;
    }
    
    
     // directional light
    float3 dir = normalize(-direction[0]);
    colour = ambient[0] + calculateLighting(-direction[0].xyz, input.normal, diffuse[0]);
    
    // directional shadows
    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);   
    float2 texdimensions;
    int levels;
    depthMapTexture.GetDimensions(0.1, texdimensions.x, texdimensions.y, levels);

    // PCF: check the surrounding texels of the depth map to produce a smoother shadow 
    float2 texelSize = 1.0 / texdimensions;
    for (int x = -1; x <= 1; x++){     
        for (int y = -1; y <= 1; y++){        
            if (hasDepthData(pTexCoord))
            {  
                if (isInShadow(depthMapTexture, pTexCoord.xy + float2(x, y) * texelSize.xy, input.lightViewPos, shadowMapBias)){
                    colour -= float4(0.010, 0.003, 0.001, 1);

                }
            
            }
        }
    }
    

    
    // point light
    float3 lightVector = normalize(position[1].xyz - input.worldPosition);

    // values for attenuation 
    float constantFactor = 0.3f;
    float linearFactor = 0.075f;
    float quadraticFactor = 0.001f;
    float3 distanceVec = position[1].xyz - input.worldPosition; // distance between light and geometry
    float distance = length(distanceVec);
    float attenuation = 1 / (constantFactor + (linearFactor * distance) + (quadraticFactor * pow(distance, 2)));
    float4 diffuseVal = diffuse[1] * saturate(attenuation);
    // point light shadows
    float shadow = 0;
    shadow += shadowCalculation(input.worldPosition);
    // if pixel is in shadow then it is set to the ambient colour of the point light
    colour += ambient[1] + (1-shadow)*calculateLighting(lightVector, input.normal, diffuseVal);
    
    
    
    
    return colour * textureColour;
}


