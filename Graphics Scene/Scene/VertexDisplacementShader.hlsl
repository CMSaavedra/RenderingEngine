#version 400

layout (location=0) in vec3 in_Position;
layout (location=1) in vec4 vertexColor;
layout (location=2) in vec2 vertexTexCoord;
layout (location=3) in vec3 in_Normal;

uniform mat4 mvp;
uniform mat4 DepthBiasMVP;
uniform mat4 ModelToWorldMatrix;

uniform float Time;
uniform float K;
uniform float Velocity;
uniform float Amplitude;

out vec2 TextureCoord;
out vec4 Position;
out vec3 Normal;
out vec4 ShadowCoord;

void main() 
{	
	TextureCoord = vertexTexCoord;

	vec4 pos = vec4(in_Position,1.0);
	float u = K * (pos.x - Velocity * Time);
	pos.y = Amplitude *  sin(u);
	
	vec3 n = vec3(0.0);
	n.xy = normalize(vec2(-K * Amplitude * cos(u), 1.0));
	
	ShadowCoord = DepthBiasMVP * pos;
	Position = ModelToWorldMatrix * pos;

	Normal = normalize(mat3(ModelToWorldMatrix) * n); 
	gl_Position = mvp * pos;
}