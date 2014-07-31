#version 400

layout (location=0) in vec3 position;
layout (location=1) in vec4 vertexColor;

uniform mat4 mvp;

out vec4 Color;

void main() 
{	
	gl_Position = mvp * vec4(position,1);
}