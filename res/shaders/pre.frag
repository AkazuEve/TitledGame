#version 460 core
layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;

layout(binding = 0) uniform sampler2D ColorTexture;

in vec4 Position;
in vec4 Normal;
in vec2 TextureCoordinate;

void main() {
	gColor = texture(ColorTexture, TextureCoordinate);
	gPosition = Position;
	gNormal = normalize(Normal);
}