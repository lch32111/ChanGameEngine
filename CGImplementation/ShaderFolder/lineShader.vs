// shadertype=glsl
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 toPoint;

out VS_OUT
{
	vec4 normal;
	float dist;
}vs_out;

void main()
{
	vec3 WorldPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(WorldPos, 1.0);

	vs_out.normal = normalize(projection * view * vec4(normalize(vec3(toPoint - WorldPos)), 0.0));
	vs_out.dist = length(vec3(projection * view * vec4(toPoint, 0.0))
					 -vec3(projection * view * vec4(WorldPos, 0.0)));
}