#version 450

uniform sampler2D diff_tex;
uniform vec3 Kd;

in vec2 ft;
in vec3 fn;

layout (location = 0) out vec4 resultColor;

void main()
{
	vec3 diff = texture(diff_tex, ft).rgb * Kd;
	resultColor = vec4(diff, 1.0);
}
