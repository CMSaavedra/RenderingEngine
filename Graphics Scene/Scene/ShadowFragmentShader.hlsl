#version 400

in vec3 position;
in vec4 Color;

out vec4 FragColor;
out vec4 fragmentdepth;

void main() 
{
	fragmentdepth = vec4(gl_FragCoord.z);
}