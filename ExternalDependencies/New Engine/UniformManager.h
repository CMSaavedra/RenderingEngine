#ifndef UNIFORM_MANAGER_H
#define UNIFORM_MANAGER_H

#include "ExportHeader.h"
#include <GL\glew.h>
#include <glm\glm.hpp>

typedef unsigned int uint;

#ifndef UNIFORMS_MAX_CAPACITY
#define UNIFORMS_MAX_CAPACITY 100
#endif

#ifndef UNIFORMNAME_MAX_CAPACITY
#define UNIFORMNAME_MAX_CAPACITY 64
#endif

#ifndef UNIFORMDATA_MAX_CAPACITY
#define UNIFORMDATA_MAX_CAPACITY 64
#endif

class ENGINE_SHARED UniformManager
{

public:
	UniformManager();
	UniformManager(const UniformManager&);
	UniformManager& operator=(const UniformManager&);
	~UniformManager();

	static UniformManager& getInstance() { return UniformInstance; }

	enum ENGINE_SHARED UniformType
	{
		GLMVEC3,
		GLMVEC4,
		GLMMAT3,
		GLMMAT4,
		GLMMAT4_AFFECTEDBYVIEWPERSPECTIVE,
		FLOAT,
		INT,
		TEXTUREINT
	};

	struct ENGINE_SHARED UniformInformation
	{
		UniformType Format;
		char Name[UNIFORMNAME_MAX_CAPACITY];
		char Data[UNIFORMDATA_MAX_CAPACITY];

		void UpdateInformation(const uint& DataSize, const void* data);
		void Initialize(const UniformType& FotmatIn, const char* NameIn, const uint& DataSize, const void* DataIn);
	};

	void MatchUniform(const GLint& UniformLocation, const UniformInformation& Uniforms, const glm::mat4& ViewPerspective);

private:
	static UniformManager UniformInstance;
};

#define Uniform UniformManager::getInstance()

#endif

