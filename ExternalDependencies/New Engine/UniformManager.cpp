#include "UniformManager.h"
#include <Qt\qdebug.h>

UniformManager UniformManager::UniformInstance;


UniformManager::UniformManager()
{
}

UniformManager::~UniformManager()
{
}

void UniformManager::MatchUniform(const GLint& UniformLocation, const UniformInformation& Uniforms, const glm::mat4& ViewPerspective)
{
	switch(Uniforms.Format)
	{
		case UniformType::GLMVEC3:
			glUniform3fv(UniformLocation, 1, reinterpret_cast<const float*>(Uniforms.Data));
			break;
		case UniformType::GLMVEC4:
			glUniform4fv(UniformLocation, 1, reinterpret_cast<const float*>(Uniforms.Data));
			break;
		case UniformType::GLMMAT3:
			glUniformMatrix3fv(UniformLocation, 1, GL_FALSE, reinterpret_cast<const float*>(Uniforms.Data));
			break;
		case UniformType::GLMMAT4:
			glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, reinterpret_cast<const float*>(Uniforms.Data));
			break;
		case UniformType::FLOAT:
			glUniform1fv(UniformLocation, 1, reinterpret_cast<const float*>(Uniforms.Data));
			break;
		case UniformType::INT:
			glUniform1iv(UniformLocation, 1, reinterpret_cast<const int*>(Uniforms.Data));
			break;
		case UniformType::TEXTUREINT:
			glUniform1i(UniformLocation, 0);
			break;
		case UniformType::GLMMAT4_AFFECTEDBYVIEWPERSPECTIVE:
			glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, &(ViewPerspective * *reinterpret_cast<const glm::mat4*>(Uniforms.Data))[0][0]);
			break;
		default:
			qDebug()<< "Invalid Uniform Format";
	}
}

void UniformManager::UniformInformation::Initialize(const UniformType& FotmatIn, const char* NameIn, const uint& DataSize, const void* DataIn)
{
	assert(UNIFORMNAME_MAX_CAPACITY >= strlen(NameIn));
	memset(Name, '\0', UNIFORMDATA_MAX_CAPACITY);
	memcpy(Name, NameIn, strlen(NameIn));

	Format = FotmatIn;
	memcpy(Data, DataIn, DataSize);
}
	