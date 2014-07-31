#version 400

layout (location = 0) in vec3 in_position;

out vec3 tex_coord;
uniform mat4 mvp;
uniform vec3 CameraPosition;

void main(void)
{
	tex_coord = -in_position;
	gl_Position = mvp * vec4(in_position, 1.0);
}