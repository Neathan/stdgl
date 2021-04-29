#version 460

out vec4 fragColor;

in vec3 position;
in vec2 textureCoord;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main() {
	/*
	const float sqrt2 = sqrt(2);

	float r = max(sqrt2-distance(position, vec3( 0, 1, 0)), 0);
	float g = max(sqrt2-distance(position, vec3(-1,-1, 0)), 0);
	float b = max(sqrt2-distance(position, vec3( 1,-1, 0)), 0);
	
	fragColor = vec4(r, g, b, 1);
	*/

	fragColor = texture(texture_diffuse1, textureCoord);
}
