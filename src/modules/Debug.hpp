#pragma once

#include <iostream>

#include "../../ext/source/glad/glad.h"

#ifdef _DEBUG
	#define DEBUGPRINT(x) std::cout << x << "\n";

#else
	#define DEBUGPRINT(x)
#endif
namespace Debug {
	void __stdcall glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);	
}