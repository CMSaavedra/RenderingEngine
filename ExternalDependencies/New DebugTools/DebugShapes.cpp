#include "DebugShapes.h"
#include <ShapeData.h>
#include <ShapeGenerator.h>
#include <Qt\qfile.h>
#include "FbxLoader.h"
#include <fstream>
#include <glm\gtx\transform.hpp>
#include <iostream>

DebugShapes DebugShapes::theInstance;

DebugShapes::DebugShapes()
{
}

DebugShapes::~DebugShapes()
{
}

void DebugShapes::CreateRender()
{
	renderer = new Renderer;
}

void DebugShapes::initialize()
{
	Neumont::ShapeData cube = Neumont::ShapeGenerator::makeCube();		
	Neumont::ShapeData arrow = Neumont::ShapeGenerator::makeArrow();	
	Neumont::ShapeData plane = Neumont::ShapeGenerator::makePlane(15,15);
	Neumont::ShapeData sphere = Neumont::ShapeGenerator::makeSphere(20);
	Neumont::ShapeData teapot = Neumont::ShapeGenerator::makeTeapot(15, glm::mat4());

	Neumont::ShapeData* table;
	unsigned int numberShapes;
	Neumont::FbxLoader::loadFbxFile("table.fbx", table, numberShapes);

	cubeGeometryIndex = renderer->AddGeometry(cube.verts,cube.vertexBufferSize(),
		cube.indices,cube.numIndices,GL_TRIANGLES);	

	arrowGeometryIndex = renderer->AddGeometry(arrow.verts, arrow.vertexBufferSize(),
		arrow.indices, arrow.numIndices, GL_TRIANGLES);

	planeGeometryIndex = renderer->AddGeometry(plane.verts, plane.vertexBufferSize(),
		plane.indices, plane.numIndices, GL_TRIANGLES);

	sphereGeometryIndex = renderer->AddGeometry(sphere.verts, sphere.vertexBufferSize(),
		sphere.indices, sphere.numIndices, GL_TRIANGLES);

	teapotGeometryIndex = renderer->AddGeometry(teapot.verts, teapot.vertexBufferSize(),
		teapot.indices, teapot.numIndices, GL_TRIANGLES);

	for(unsigned int i = 0; i < numberShapes; i++)
	{
		tableGeometryIndex = renderer->AddGeometry(table[i].verts, table[i].vertexBufferSize(),
			table[i].indices, table[i].numIndices, GL_TRIANGLES);
	}
		
	shaderProgramID = renderer->AddShader("VertexShader.hlsl","FragmentShader.hlsl");
	NormalShaderProgramID = renderer->AddShader("NormalVertexShader.hlsl","NormalFragmentShader.hlsl");
	ReflectionProgramID = renderer->AddShader("VertexShader.hlsl","ReflectionFragmentShader.hlsl");
	VertexDisplacementShaderProgramID = renderer->AddShader("VertexDisplacementShader.hlsl","FragmentShader.hlsl");
	
	cube.cleanUp();
	arrow.cleanUp();
	plane.cleanUp();
	sphere.cleanUp();
	teapot.cleanUp();
}

void DebugShapes::LoadShadows()
{	
	shadowshaderProgramID = renderer->AddShader("ShadowVertexShader.hlsl","ShadowFragmentShader.hlsl");
	SkyBoxProgramID = renderer->AddShader("SkyBoxVertexShader.hlsl","SkyBoxFragmentShader.hlsl");
	renderer->AddTexture("Normal.png");
	renderer->AddTexture("brick_texture.png");
	renderer->AddTexture("texture.png");
	renderer->AddTexture("textureTeapot.png");
	renderer->AddTexture("textureVertex.png");
	renderer->AddTexture("wood.png");
	renderer->AddTexture("Flat.png");
}

void DebugShapes::AddFBX(const glm::mat4& position, const bool& ignoreDepth, const bool& useBlend)
{
	std::ifstream in("c:\\lulu.bin", std::ios::in | std::ios::binary);
	in.seekg(0, std::ios::end);
	const unsigned int BUFFER_SIZE = in.tellg(); 
	char* buf = new char[BUFFER_SIZE];
	in.seekg(0, std::ios::beg);
	in.read(buf, BUFFER_SIZE);
	in.close();

	uint NUM_SHAPE =  *reinterpret_cast<unsigned int*>(buf);

	Neumont::ShapeData* shapeDatas = reinterpret_cast<Neumont::ShapeData*>(buf + sizeof(unsigned int));	
	
	for(unsigned int i = 0; i < NUM_SHAPE; i++)
	{
		Neumont::ShapeData& d = shapeDatas[i];
		d.verts = reinterpret_cast<Neumont::Vertex*>( buf + (unsigned int)(d.verts));
		d.indices = reinterpret_cast<ushort*>( buf + (unsigned int)(d.indices));

		Neumont::ShapeData fbxObject = shapeDatas[i];
		unsigned int fbxObjectGeometryIndex = renderer->AddGeometry(fbxObject.verts,fbxObject.vertexBufferSize(),
		fbxObject.indices,fbxObject.numIndices, GL_TRIANGLES);	

		Renderer::Renderable* renderableToTrack = renderer->AddRenderable(fbxObjectGeometryIndex, position * glm::translate(glm::vec3(0.0f,1.0f*(i),0.0f)),shaderProgramID,ignoreDepth, useBlend);
		Object = renderableToTrack;
	}		
	
	delete[] buf;
}

void DebugShapes::AddCube(const glm::mat4& position, const float& textureId, const float& lifetime, const bool& ignoreDepth, const bool& useBlend)
{
	assert(numDebugInfo < MAX_DEBUG_INFOS);
	DebugInformation& debug = debugInfos[numDebugInfo++];
	
	if(lifetime > 0)
	{
		debug.MAX_LIFETIME = lifetime;
		debug.lifetime = 0;
	}

	debug.ignoreDepth = ignoreDepth;

	Renderer::Renderable* renderableToTrack = renderer->AddRenderable(cubeGeometryIndex, position, NormalShaderProgramID, ignoreDepth, useBlend);
	renderableToTrack->Texture = textureId;
	debug.renderable = renderableToTrack;
}

void DebugShapes::AddSkyBox(const glm::mat4& position)
{
	Renderer::Renderable* renderableToTrack = renderer->AddRenderable(cubeGeometryIndex, position, SkyBoxProgramID, true, false);
	renderableToTrack;
}

void DebugShapes::AddArrow(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend)
{
	assert(numDebugInfo < MAX_DEBUG_INFOS);
	DebugInformation& debug = debugInfos[numDebugInfo++];
	
	if(lifetime > 0)
	{
		debug.MAX_LIFETIME = lifetime;
		debug.lifetime = 0;
	}

	debug.ignoreDepth = ignoreDepth;

	Renderer::Renderable* renderableToTrack = renderer->AddRenderable(arrowGeometryIndex, position, shaderProgramID, ignoreDepth, useBlend);	
	glm::vec4 Color(0.2f,0.8f,0.2f,1);
	renderableToTrack->AddUniform(Uniform.UniformType::GLMVEC4, "color", SIZE_32, reinterpret_cast<const void*>(&Color));	
	renderableToTrack->Texture = 1;
	debug.renderable = renderableToTrack;
}

void DebugShapes::AddPlane(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend)
{
	assert(numDebugInfo < MAX_DEBUG_INFOS);
	DebugInformation& debug = debugInfos[numDebugInfo++];
	
	if(lifetime > 0)
	{
		debug.MAX_LIFETIME = lifetime;
		debug.lifetime = 0;
	}

	debug.ignoreDepth = ignoreDepth;

	Renderer::Renderable* renderableToTrack = renderer->AddRenderable(planeGeometryIndex, position, shaderProgramID, ignoreDepth, useBlend);
	glm::vec4 Color(1,1,1,1);
	renderableToTrack->AddUniform(Uniform.UniformType::GLMVEC4, "color", SIZE_32, reinterpret_cast<const void*>(&Color));		
	renderableToTrack->Texture = 6;
	debug.renderable = renderableToTrack;
}

void DebugShapes::AddVetexPlane(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend)
{
	lifetime;
	Renderer::Renderable* renderableToTrack = renderer->AddRenderable(planeGeometryIndex, position, VertexDisplacementShaderProgramID, ignoreDepth, useBlend);
	glm::vec4 Color(1,1,1,1);
	renderableToTrack->Texture = 8;
	renderableToTrack->AddUniform(Uniform.UniformType::GLMVEC4, "color", SIZE_32, reinterpret_cast<const void*>(&Color));		
}

void DebugShapes::AddLightSource(const glm::mat4& position)
{
	LightSource = renderer->AddRenderable(sphereGeometryIndex, position, shadowshaderProgramID, true, false);	
}

void DebugShapes::AddSphere(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend)
{
	assert(numDebugInfo < MAX_DEBUG_INFOS);
	DebugInformation& debug = debugInfos[numDebugInfo++];
	
	if(lifetime > 0)
	{
		debug.MAX_LIFETIME = lifetime;
		debug.lifetime = 0;
	}

	debug.ignoreDepth = ignoreDepth;

	Renderer::Renderable* renderableToTrack = renderer->AddRenderable(sphereGeometryIndex, position,  NormalShaderProgramID, ignoreDepth, useBlend);	
	debug.renderable = renderableToTrack;
}

void DebugShapes::AddTeapot(const glm::mat4& position, const float& lifetime, const bool& ignoreDepth, const bool& useBlend)
{
	assert(numDebugInfo < MAX_DEBUG_INFOS);
	DebugInformation& debug = debugInfos[numDebugInfo++];
	
	if(lifetime > 0)
	{
		debug.MAX_LIFETIME = lifetime;
		debug.lifetime = 0;
	}

	debug.ignoreDepth = ignoreDepth;

	Renderer::Renderable* renderableToTrack = renderer->AddRenderable(teapotGeometryIndex, position, shaderProgramID, ignoreDepth, useBlend);	
	Object = renderableToTrack;
	renderableToTrack->Texture = 7;
	debug.renderable = renderableToTrack;
}

void DebugShapes::AddTable(const glm::mat4& position, const bool& ignoreDepth, const bool& useBlend)
{
	Renderer::Renderable* renderableToTrack = renderer->AddRenderable(tableGeometryIndex, position, shaderProgramID, ignoreDepth, useBlend);
	renderableToTrack->Texture = 9;
}

DebugShapes& DebugShapes::getInstance()
{
	return theInstance;
}
