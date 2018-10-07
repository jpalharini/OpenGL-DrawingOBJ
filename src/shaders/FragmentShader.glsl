#version 410

in vec2 texCoord;
uniform sampler2D theTexture;
out vec4 color;

void main () {
    color = texture(theTexture, texCoord);
}