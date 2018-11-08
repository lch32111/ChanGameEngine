// shadertype=glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 viewModel;
uniform mat4 projection;
uniform mat3 MVNormalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
	gl_Position = projection * viewModel * vec4(aPos, 1.0);
	FragPos = vec3(viewModel * vec4(aPos, 1.0));
	Normal = MVNormalMatrix * aNormal;
	TexCoords = aTexCoords;
}