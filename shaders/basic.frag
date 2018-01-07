#version 400 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_2d;

void main() {
  FragColor = texture(texture_2d, TexCoord);
}
