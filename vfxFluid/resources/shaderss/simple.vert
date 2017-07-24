#version 450

uniform mat4 MVP;

layout (location = 0) in vec3 vp;
layout (location = 1) in vec2 vt;
layout (location = 2) in vec3 vn;

out vec2 ft;
out vec3 fn;

void main() {
	// Pass some variables to the fragment shader
	ft = vt;
    
	// Apply all matrix transformations to vert
    gl_Position = MVP  * vec4(vp, 1.0);
}