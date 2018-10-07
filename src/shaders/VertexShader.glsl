#version 410

layout(location=0) in vec3 vp;
layout(location=1) in vec3 vn;
layout(location=2) in vec2 vt;

uniform mat4 projection, view;

out vec2 texCoord;

void main () {
   texCoord = vt;
	gl_Position = projection * view * vec4 (vp, 1.0);
}