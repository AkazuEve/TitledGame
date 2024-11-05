#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoordinate;

uniform mat4 model;
uniform mat4 modelNormal;
uniform mat4 camera;

out vec4 Position;
out vec4 Normal;
out vec2 TextureCoordinate;

void main() {
	gl_Position = camera * model * vec4(inPos, 1.0);

	Position = model * vec4(inPos, 1.0);

	Normal = transpose(inverse(modelNormal)) * vec4(normalize(inNormal), 1.0);

	TextureCoordinate = inTextureCoordinate;
}