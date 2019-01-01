// shadertype=glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 mvpMatrix;

out vec3 fragPos;
out vec3 normal;
out vec2 TexCoords;

void main()
{
	gl_Position = mvpMatrix * vec4(aPos, 1.0);
	fragPos = aPos;
	normal = aNormal;
	TexCoords = aTexCoords;
}