#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTextureCoordinate;

out vec2 TextureCoordinate;

uniform mat4 model;
uniform mat4 camera;

void main() {
	TextureCoordinate = inTextureCoordinate;
	gl_Position = camera * model * vec4(inPos, 1.0);
}