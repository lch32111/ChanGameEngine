// shadertype=glsl

// Deferred First Pass Fragment Shader
// The lighting will be calculated in World Space.

#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gEmissive;
layout (location = 4) out vec4 gBool;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBNmat;

struct Material
{
    bool CMorLM; // CM == false, LM == true
    bool isLMdiffuse;
    bool isLMspecular;
    bool isLMemissive;

    // Color Material Component
    vec3 CMambient;
    vec3 CMdiffuse;
    vec3 CMspecular;
    float CMshininess;

    // Light Map Material Component
    sampler2D LMdiffuse;
    sampler2D LMspecular;
    sampler2D LMemissive;

    // Normal/Parallax Mapping
    bool isNormalMap;
    bool isDepthMap;
    sampler2D NormalMap;
    sampler2D DepthMap;
};

uniform Material material;
uniform vec3 cameraPos;

vec2 ParallaxOcclusionMapping(in vec2 texCoords, in vec3 viewDir);

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;

    vec2 real_TexCoords = TexCoords;
    vec3 real_Normal = Normal;

    if(material.isDepthMap)
    {
        // Parallax Mapping ->get new TexCoords;
        real_TexCoords = ParallaxOcclusionMapping(TexCoords, transpose(TBNmat) * (cameraPos - FragPos));
    }

    if(material.isNormalMap)
    {
        // Normal Mapping -> get new normal, and then transform it into world space.
        real_Normal = texture(material.NormalMap, real_TexCoords).xyz;
        real_Normal = normalize(real_Normal * 2.0 - 1.0);
        real_Normal = TBNmat * real_Normal;
    }

    // also store the per-fragmentnormals into the gBuffer
    gNormal = normalize(real_Normal);
    
	gBool.a = material.CMorLM ? 0.9 : 0.1; 
    if(material.CMorLM)
    {
        gBool.rgb = vec3(1.0);
        // Light Map
        if(material.isLMdiffuse)	gAlbedoSpec.rgb = texture(material.LMdiffuse, real_TexCoords).rgb;
        else    gBool.r = 0;   
        if(material.isLMspecular)    gAlbedoSpec.a = texture(material.LMspecular, real_TexCoords).r;
        else    gBool.g = 0;
        if(material.isLMemissive)    gEmissive.rgb = texture(material.LMemissive, real_TexCoords).rgb;
        else    gBool.b = 0;
    }
    else
    {
        // Color Material
        gBool.rgb = material.CMambient;
        gAlbedoSpec.rgb = material.CMdiffuse;
        gAlbedoSpec.a = material.CMshininess;
        gEmissive.rgb = material.CMspecular;
    }
}

vec2 ParallaxOcclusionMapping(in vec2 texCoords, vec3 viewDir)
{
    // viewDir is in the tangent space

    // number of depth layers
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (From vector P)

    // Manual Setting for adjustment of Parallax Mapping
    // TODO : convert this height_scale as a uniform variable to control it.
    float height_scale = 0.1; 
    vec2 P = viewDir.xy * height_scale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(material.DepthMap, currentTexCoords).r;
 
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.DepthMap, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // Parallax Occlusion Mapping
 
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
 
    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.DepthMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}