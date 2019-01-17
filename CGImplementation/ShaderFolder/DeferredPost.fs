// shadertype=glsl

#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D currentScene;
uniform sampler2D bloomedScene;

uniform bool useBloom;
uniform float gamma;
uniform float exposure;

void main()
{
    vec3 hdrColor = texture(currentScene, TexCoords).rgb;
	if(useBloom) 
	{
		vec3 bloomColor = texture(bloomedScene, TexCoords).rgb;
		hdrColor += bloomColor;
	}

	// Reinhard Tone Mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}