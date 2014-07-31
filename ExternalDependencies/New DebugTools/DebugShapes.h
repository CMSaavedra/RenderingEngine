#pragma once
#ifndef DEBUG_SHAPES_H
#define DEBUG_SHAPES_H

#include "ExportHeader.h"
#include "Renderer.h"
#include "Clock.h"
#include <Qt\qdebug.h>
#include <ShapeData.h>

typedef unsigned int uint;

#ifndef MAX_DEBUG_INFOS
#define MAX_DEBUG_INFOS 100
#endif

class DEBUG_SHARED DebugShapes
{
public:
	DebugShapes();
	~DebugShapes();

	void initialize();
	void CreateRender();
	void LoadShadows();
	static DebugShapes& getInstance();
	
	Renderer* renderer;
	Renderer::Renderable* LightSource;
	Renderer::Renderable* Object;

	void AddFBX(const glm::mat4& position, const bool& ignoreDepth, const bool& useBlend);
	void AddArrow(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend);
	void AddCube(const glm::mat4& position, const float& tetureID, const float& lifetime, const bool& ignoreDepth, const bool& useBlend);
	void AddVetexPlane(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend);
	void AddPlane(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend);
	void AddSphere(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend);
	void AddTeapot(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend);
	void AddTable(const glm::mat4& position, const bool& ignoreDepth, const bool& useBlend);
	void AddLightSource(const glm::mat4& position);
	void AddSkyBox(const glm::mat4& position);

private:

	struct DebugInformation
	{
		Renderer::Renderable* renderable;
		
		float MAX_LIFETIME; 
		float lifetime;
		bool ignoreDepth;

		DebugInformation(float maxLifetime = 0) : 
			MAX_LIFETIME(maxLifetime) { lifetime = 0; }

		void update()
		{ 
			lifetime += clockManager.timeElapsedLastFrame();	
			qDebug()<<lifetime;
			renderable->Visible = lifetime < MAX_LIFETIME;
		}
	};

	DebugInformation debugInfos[MAX_DEBUG_INFOS];
	uint numDebugInfo;
		
	uint shaderProgramID;
	uint VertexDisplacementShaderProgramID;
	uint SkyBoxProgramID;
	uint NormalShaderProgramID;
	uint shadowshaderProgramID;
	uint ReflectionProgramID;
	uint cubeGeometryIndex;
	uint arrowGeometryIndex;
	uint planeGeometryIndex;
	uint sphereGeometryIndex;
	uint teapotGeometryIndex;
	uint tableGeometryIndex;
	
	GLuint textureID;
		
	inline void DebugUpdate();

	DebugShapes(const DebugShapes&);
	DebugShapes operator=(const DebugShapes&); 

	static DebugShapes theInstance;
};

#define DebugShapeManager DebugShapes::getInstance()

#endif