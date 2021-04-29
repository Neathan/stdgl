#version 460

in vec3 in_position;
in vec3 in_normal;
in vec2 in_textureCoord;

out vec3 position;
out vec3 normal;
out vec2 textureCoord;

uniform mat4 projection;
uniform mat4 view;


void main() {
	position = in_position;
	normal = in_normal;
	textureCoord = in_textureCoord;
	
	gl_Position = projection * view * vec4(position, 1.0f);
}
