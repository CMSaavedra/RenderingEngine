#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "ExportHeader.h"

namespace Engine
{
	//ENGINE_SHARED to indicate a function or class that you want to export
	ENGINE_SHARED void Init();

	ENGINE_SHARED bool sampleFunctionThatReturnsTrue();
}

#endif