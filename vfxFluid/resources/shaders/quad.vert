#version 450

layout(location = 0) in vec4 in_vertex_position;

void main()
{
	gl_Position = in_vertex_position;
}