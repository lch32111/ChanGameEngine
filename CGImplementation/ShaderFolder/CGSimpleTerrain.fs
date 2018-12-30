// shadertype=glsl
#version 330 core

out vec4 FragColor;
in float ypos;

void main()
{
	float t = (ypos + 0.5);
	vec3 color = (1 - t) * vec3(1.0, 0.0, 0.0) + t * vec3(1.0);
	FragColor = vec4(color, 1.0);
}