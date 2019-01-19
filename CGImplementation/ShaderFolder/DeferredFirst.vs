// shadertype=glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 instanceModel;
layout (location = 8) in mat4 instanceModelNormalMatrix;

layout (std140) uniform Matrices
{
	mat4 view;			// col0 0 ~ 15
		                // col1 16 ~ 31
						// col2 32 ~ 47
						// col3 48 ~ 63
	mat4 projection;    // col0 64 ~ 79
						// col1 80 ~ 95
						// col2 96 ~ 111
						// col3 112 ~ 128
};

uniform bool IsUseTangentSpace;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBNmat;

void main()
{
	gl_Position = projection * view * instanceModel * vec4(aPos, 1.0);

	// Lighting Calculation in World Space
	FragPos = vec3(instanceModel * vec4(aPos, 1.0));
	TexCoords = aTexCoords;
	Normal = mat3(instanceModelNormalMatrix) * aNormal;

	if(IsUseTangentSpace)
	{
		// Gram-schmidt Process
		vec3 T = normalize(vec3(instanceModel * vec4(aTangent, 0.0)));
		vec3 N = normalize(vec3(instanceModel * vec4(aNormal, 0.0)));
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);
		TBNmat = mat3(T, B, N);

		// and then use the normal/depth map in fragment shader with TBNmat
	}
}