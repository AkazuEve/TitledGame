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

vec4 to_low_precision(vec4 position,vec2 resolution)
{
	//Perform perspective divide
	vec3 perspective_divide = position.xyz / vec3(position.w);
	
	//Convert to screenspace coordinates
	vec2 screen_coords = (perspective_divide.xy 
							+ vec2(1.0,1.0)) 
							* vec2(resolution.x,resolution.y) 
							* 0.5;

	//Truncate to integer
	vec2 screen_coords_truncated = vec2(int(screen_coords.x),
										int(screen_coords.y));
	
	//Convert back to clip range -1 to 1
	vec2 reconverted_xy = ((screen_coords_truncated * vec2(2,2))
							 / vec2(resolution.x,resolution.y)) 
							- vec2(1,1);

	//Construct return value
	vec4 ps1_pos = vec4(reconverted_xy.x,
						reconverted_xy.y,
						perspective_divide.z,
						position.w);

	ps1_pos.xyz = ps1_pos.xyz * vec3(	position.w,
										position.w,
										position.w);

	return ps1_pos;

}

void main() {
	gl_Position = camera * model * vec4(inPos, 1.0);

	Position = model * vec4(inPos, 1.0);

	Normal = transpose(inverse(modelNormal)) * vec4(normalize(inNormal), 1.0);

	TextureCoordinate = inTextureCoordinate;
}