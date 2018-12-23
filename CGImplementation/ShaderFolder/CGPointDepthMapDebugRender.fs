// shadertype=glsl
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube depthMap;

void main()
{
    FragColor = vec4(vec3(texture(depthMap, TexCoords).r), 1.0);
}