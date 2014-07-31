#version 400

layout (location=0) in vec3 in_Position;
layout (location=1) in vec4 vertexColor;
layout (location=2) in vec2 vertexTexCoord;
layout (location=3) in vec3 in_Normal;
//layout (location=4) in vec3 in_Tangent;
//layout (location=5) in vec3 in_BiTangent;

uniform mat4 mvp;
uniform mat4 DepthBiasMVP;
uniform mat4 ModelToWorldMatrix;

out vec2 TextureCoord;
out vec4 Position;
out vec3 Normal;
out vec4 ShadowCoord;

void main() 
{	
	TextureCoord = vertexTexCoord;
	ShadowCoord = DepthBiasMVP * vec4(in_Position,1);
	Normal = normalize(mat3(ModelToWorldMatrix) * in_Normal);
	Position = ModelToWorldMatrix * vec4(in_Position,1);
	gl_Position = mvp * vec4(in_Position,1);
}