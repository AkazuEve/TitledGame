#pragma once

#include <iostream>

#include "../../ext/source/glad/glad.h"

#ifdef _DEBUG
	#define DEBUGPRINT(x) std::cout << x << std::endl;

#else
	#define DEBUGPRINT(x)

#endif