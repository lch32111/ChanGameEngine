// shadertype=glsl
#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 TexCoords;
in float ypos;

uniform vec3 cameraPos;

uniform sampler2D terrTex;

void main()
{
	
	vec3 color = vec3(0.7, 0.4, 0.0);
	vec3 lightDirection = normalize(vec3(-0.67, -0.7, 0.14) * -1.0);
	vec3 viewDir = normalize(cameraPos - fragPos);
	
	// ambient
	float ambient = 0.3;

	// diffuse
	float diff = max(dot(normal, lightDirection), 0.0);

	// specular shading
	vec3 halfwayDir = normalize(lightDirection + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);

	color = ambient * color + diff * color + spec * vec3(1.0);
	
	// vec3 texColor = texture(terrTex, TexCoords).xyz;
	FragColor = vec4(color, 1.0);
}