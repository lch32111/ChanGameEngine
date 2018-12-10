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
};

uniform Material material;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    
	gBool.a = material.CMorLM ? 0.9 : 0.1; 
    if(material.CMorLM)
    {
        gBool.rgb = vec3(1.0);
        // Light Map
        if(material.isLMdiffuse)	gAlbedoSpec.rgb = texture(material.LMdiffuse, TexCoords).rgb;
        else    gBool.r = 0;   
        if(material.isLMspecular)    gAlbedoSpec.a = texture(material.LMspecular, TexCoords).r;
        else    gBool.g = 0;
        if(material.isLMemissive)    gEmissive.rgb = texture(material.LMemissive, TexCoords).rgb;
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