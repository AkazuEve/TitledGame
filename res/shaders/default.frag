#version 460 core
layout (location = 0) out vec4 Color;

layout(binding = 0) uniform sampler2D ColorTexture;

in vec4 Position;
in vec4 Normal;
in vec2 TextureCoordinate;

void main() {
	//Color = texture(ColorTexture, TextureCoordinate);
	//Color = Normal;
	//Color = Position;

	vec3 lightPos = vec3(1.0);

	vec3 lightDir = normalize(lightPos - Position.xyz);  

	float diff = max(dot(Normal.xyz, lightDir), 0.2);

	Color = diff * texture(ColorTexture, TextureCoordinate);
}