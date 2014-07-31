#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "ExportHeader.h"
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <QtOpenGL\qglwidget>
#include "Camera.h"
#include "UniformManager.h"

typedef unsigned int uint;

#ifndef MAX_NUM_RENDERABLES
#define MAX_NUM_RENDERABLES 100
#endif

#ifndef MAX_NUM_GEOMETRIES
#define MAX_NUM_GEOMETRIES 100
#endif

#ifndef MAX_NUM_SHADERS
#define MAX_NUM_SHADERS 100
#endif

#ifndef MAX_NUM_VERTEXBUFFER_IDS
#define MAX_NUM_VERTEXBUFFER_IDS 100
#endif

#ifndef MAX_NUM_INDEXBUFFER_IDS
#define MAX_NUM_INDEXBUFFER_IDS 100
#endif

#ifndef MAX_NUM_UNIFORMS
#define MAX_NUM_UNIFORMS 100
#endif

#ifndef MAX_NUM_TEXTURES
#define MAX_NUM_TEXTURES 100
#endif


#ifndef SIZE_64
#define SIZE_64 64
#endif

#ifndef SIZE_32
#define SIZE_32 32
#endif 

#ifndef SIZE_4
#define SIZE_4 4
#endif 

#ifndef DEPTH_TEXTURE_SIZE
#define DEPTH_TEXTURE_SIZE 1024
#endif 

class ENGINE_SHARED Renderer : public QGLWidget
{

private:
	Q_OBJECT

	glm::mat4 Projection;
	Camera MainCamera;
	GLuint Framebuffer;
	GLuint depthTexture;	
	GLuint CubeMapTexture;
	GLuint CubeMapTexture2;
	GLuint NoiseTexture;
	
public:
	struct ENGINE_SHARED Renderable
	{
		uint GeometryID;
		glm::mat4 where;
		uint ShaderProgramID;
		uint NumUniforms;
		bool Visible;
		bool Depth;
		bool Blend;
		int Texture;

		UniformManager::UniformInformation Uniforms[UNIFORMS_MAX_CAPACITY];
		Renderable() : Visible(true) { NumUniforms = 0; Texture = 0; }

		void Initialize(const uint& GeometryID, const uint& ProgramIndex, const bool& Visible, const bool& UseDepthtest, const bool& UseBlend);

		UniformManager::UniformInformation* AddUniform(const UniformManager::UniformType& Format, const char* Name, const uint& DataSize, const void* Data);
	};
	
	Renderable Renderables[MAX_NUM_RENDERABLES];
	uint NextRenderableIndex;
	glm::vec3 LightPosition;
	
	float K; 
	float Velocity;
	
	float HighThreshold; 
	float LowThreshold;
	float dt;

	bool LightsOn;
	bool NormalsOn;
	bool ShadowsOn;

private:

	struct ShaderInformation
	{
		uint GlProgramID;
		uint VertexShaderID;
		uint FragShaderID;
	};

	struct GeometryInformation
	{
		uint VertexBufferSize;
		uint VertexBufferOffset;
		uint IndexBufferSize;
		uint IndexBufferOffset;
		uint NumIndices;
		uint IndexMode;
	};

	struct TextureInformation
	{
		GLuint TextureID;
		QImage image;
	};

	GeometryInformation Geometries[MAX_NUM_GEOMETRIES];
	ShaderInformation ShadersInfos[MAX_NUM_SHADERS];
	TextureInformation TextureInfos[MAX_NUM_TEXTURES];
	uint VertexBufferIDs[MAX_NUM_VERTEXBUFFER_IDS];
	uint IndexBufferIDs[MAX_NUM_INDEXBUFFER_IDS];
	uint Uniforms[MAX_NUM_UNIFORMS];

	uint NextGeometryIndex;
	uint NextShaderInfoIndex;
	uint NextVertexBufferIndex;
	uint NextIndexBufferIndex;
	uint NextUniformIndex;
	uint NextTextureIndex;

	uint CurrentVertexOffset;
	uint CurrentIndexOffset;

	uint VertexBufferID;
	uint IndexBufferID;

	uint BufferSize;

	glm::vec2 lastMousePos;
			
public:
	Renderer();
	~ Renderer();		

	const uint AddGeometry(const void* VerticeData, const uint& VertexBufferSize, const unsigned short* indices, const uint& NumIndices, const uint& IndexMode);
	const uint AddShader(const char* VertexShaderFileName, const char* FragShaderFileName);
	Renderable* AddRenderable(const uint& GeomertryId, const glm::mat4& where, const uint& ShaderProgramID, const bool& IgnoreDepth, const bool& UseBlend);
	const uint AddTexture(const char* TextureName);

	void AddToOpenVertexBuffer(const void* VerticeData, uint& VertexBufferSize, const GeometryInformation& GeometryInfo);
	void AddToOpenIndexBuffer(const unsigned short* Indices, const uint& IndexBufferSize, const GeometryInformation& GeometryInfo);
	void CompileShader(const char* FileName, GLuint ShaderID);
	void EnableAttribs(const uint& Index, const uint& Size, const void* Pointer);
	void ShutDown();

protected:
	void initializeGL();
	void GenerateBuffers();
	void paintGL();
	void FillBuffers();
	void CreateSkyBox();
	void CreateShadowMap();
	void CreateNoiseTexture();
	void ScreenCapture();
	void DrawObjects(const glm::mat4& Camera, const glm::mat4& Projection);
	void DrawShadows(const glm::mat4& Camera, const glm::mat4& Projection);
	void resizeEvent();
	void mouseMoveEvent(QMouseEvent* e);
	void keyPressEvent(QKeyEvent* e);
	
signals:
	void renderInitialized();

	public slots:
		void Update();
};

#define RendererManager Renderer::getInstance()

#endif