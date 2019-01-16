// shadertype=glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 ModelNormalMatrix;

uniform bool IsUseTangentSpace;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBNmat;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	// Lighting Calculation in World Space
	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoords = aTexCoords;
	Normal = ModelNormalMatrix * aNormal;

	if(IsUseTangentSpace)
	{
		// Gram-schmidt Process
		vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
		vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);
		TBNmat = mat3(T, B, N);

		// and then use the normal/depth map in fragment shader with TBNmat
	}
}