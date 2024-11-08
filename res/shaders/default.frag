#version 460 core
layout (location = 0) out vec4 Color;

layout(binding = 0) uniform sampler2D ColorTexture;

in vec4 Position;
in vec4 Normal;
in vec2 TextureCoordinate;

void main() {
	vec3 lightDir = normalize(vec3(1, 1, 0));  

	float diff = max(dot(Normal.xyz, lightDir), 0.2);

	Color = diff * (texture(ColorTexture, TextureCoordinate) * 1.5);
}