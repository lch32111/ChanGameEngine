// shadertype=glsl
#version 330 core

out vec4 FragColor;

in vec4 lineColor;

void main()
{
	FragColor = lineColor;
}