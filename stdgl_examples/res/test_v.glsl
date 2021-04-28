#version 460

in vec3 in_position;

uniform vec3 offsetPosition;

void main() {
	
	gl_Position = vec4(in_position + offsetPosition, 1.0f);
	
}
