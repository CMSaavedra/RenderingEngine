#version 400

in vec3 Normal;
in vec4 Position;

out vec4 Color;

uniform samplerCube gCubemapTexture;

void main() 
{	
	vec3 TexCoord = reflect(Position.xyz, Normal); 

	Color = texture(gCubemapTexture, TexCoord);
}