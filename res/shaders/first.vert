#version 460 core
layout (location = 0) in vec2 aPos;

out vec2 screenPos;

void main() {
	screenPos = (aPos + 1) / 2;
	gl_Position = vec4(aPos, 0.0, 1.0);
}