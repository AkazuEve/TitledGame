#version 460 core
layout (location = 0) out vec4 Color;

layout(binding = 0) uniform sampler2D ColorTexture;

in vec4 Position;
in vec4 Normal;
in vec2 TextureCoordinate;

void main() {
	Color = Position;
}