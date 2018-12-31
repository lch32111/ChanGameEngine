// shadertype=glsl
#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 TexCoords;
in float ypos;

uniform vec3 cameraPos;

void main()
{
	
	vec3 color = vec3(0.7, 0.4, 0.0);
	
	vec3 lightDirection = normalize(vec3(-0.67, -0.7, 0.14) * -1.0);
	vec3 viewDir = normalize(cameraPos - fragPos);
	
	// ambient
	float ambient = 0.2;

	// diffuse
	float diff = max(dot(normal, lightDirection), 0.0);

	// specular shading
	vec3 halfwayDir = normalize(lightDirection + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);

	color = (ambient + diff + spec) * color;

	FragColor = vec4(color, 1.0);
}