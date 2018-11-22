// shadertype=glsl
#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;

const float width = 5;

in VS_OUT
{
	vec4 normal;
	float dist;
}gs_in[];

void main()
{
	gl_Position =  gl_in[0].gl_Position;
	EmitVertex();

	gl_Position =  gl_in[0].gl_Position + gs_in[0].normal * gs_in[0].dist;
	EmitVertex();
	
	EndPrimitive();
}