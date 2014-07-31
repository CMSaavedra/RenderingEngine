#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "ExportHeader.h"

namespace DebugTools
{
	// your stuff goes here...
	// use DEBUG_SHARED to indicate a function or class that you want to export
	DEBUG_SHARED void Init();

	DEBUG_SHARED bool sampleFunctionThatReturnsTrue();
}

#endif // _DEBUG_H_