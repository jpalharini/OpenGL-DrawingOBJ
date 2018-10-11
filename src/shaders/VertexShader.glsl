#version 410

layout(location=0) in vec3 vp;
layout(location=1) in vec3 vn;
layout(location=2) in vec2 vt;

uniform mat4 projection, view, model;

out vec2 texCoord;
out vec3 eye_position, eye_normal;

void main () {
    texCoord = vt;

    // Stores the position and normal of the eye to pass to FS
    eye_position = vec3(view * model * vec4(vp, 1.0));
    eye_normal = vec3(view * model * vec4(vn, 0.0));

    gl_Position = projection * view * vec4(vp, 1.0);
}