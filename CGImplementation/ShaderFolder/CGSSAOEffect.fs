// shadertype=glsl

// CG SSAO Effect Shader

#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;    // World Space 
uniform sampler2D gNormal;      // World Space
uniform sampler2D texNoise; 

#define NR_SSAO_KERNEL 64
uniform vec3 samples[NR_SSAO_KERNEL];
uniform mat4 view;
uniform mat4 projection;

uniform int screenWidth;
uniform int screenHeight;
uniform int noiseTexDimension;

uniform float radius;
uniform float bias;
uniform float power;

void main()
{
    vec2 noiseScale = vec2(screenWidth, screenHeight) / noiseTexDimension;

    vec3 fragPos = vec3(view * vec4(texture(gPosition, TexCoords).xyz, 1.0));
    vec3 normal = normalize(vec3(view * vec4(texture(gNormal, TexCoords).xyz, 1.0)));
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < NR_SSAO_KERNEL; ++i)
    {
        // get sample position
        vec3 f_sample = TBN * samples[i];
        f_sample = fragPos + f_sample * radius;
        
        vec4 offset = vec4(f_sample, 1.0);

        offset = projection * offset;  // from view to clip-space
		offset.xy /= offset.w; // perspective divide
		offset.xy = offset.xy * 0.5 + 0.5; // transform to range 0.0 - 1.0
    
        // World Space -> View Space Transformation
        // But We only need the z position. So I just use dot product.
        vec4 WorldPosition = vec4(texture(gPosition, offset.xy).xyz, 1.0);
        float sampleDepth = 0;
        for(int i = 0; i < 4; ++i)
            sampleDepth += view[i][2] * WorldPosition[i];
		
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= f_sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / NR_SSAO_KERNEL);

	FragColor = pow(occlusion, power);
}
