#version 460 core
layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gNormal;

layout(binding = 0) uniform sampler2D ColorTexture;

in vec2 TextureCoordinate;

void main() {
   gColor = texture(ColorTexture, TextureCoordinate);
   gNormal = vec4(1.0, 0.5 ,0.5, 1.0);
}