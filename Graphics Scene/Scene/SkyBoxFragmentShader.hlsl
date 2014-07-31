#version 400

in vec3 tex_coord;

out vec4 color;

uniform samplerCube gCubemapTexture;

void main(void)
{
	color = texture(gCubemapTexture, tex_coord);
}