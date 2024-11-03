#version 460 core
layout(binding = 0) uniform sampler2D gColor;
layout(binding = 1) uniform sampler2D gPosition;
layout(binding = 3) uniform sampler2D gNormal;

layout (location = 0) out vec4 finalColor;

in vec2 screenPos;

void main() {
   vec4 color = texture(gColor, screenPos);
   vec4 position = texture(gPosition, screenPos);
   vec4 normal = texture(gNormal, screenPos);

	finalColor = color;
}