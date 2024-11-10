#include <iostream>

#include "../../ext/source/glad/glad.h"

#include "modules/rendering/Rendering.hpp"
#include "modules/Window.hpp"

#include <crtdbg.h>

#ifdef _DEBUG
	size_t allocatedMemory{ 0 };

	void* __CRTDECL operator new(size_t size) {
		allocatedMemory += size;
		DEBUGPRINT("Allocation: " << size << " Usage: " << allocatedMemory);

		void* p = malloc(size);
		return p;
	}

	void operator delete(void* memory, size_t size) {
		allocatedMemory -= size;
		DEBUGPRINT("Deallocation: " << size << " Usage: " << allocatedMemory);

		free(memory);
	}
#endif


int main() {
	#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	Window mainWindow(1440, 810, "Game Window"); 
	Rendering::Init();

	Rendering::GetModelManager()->CreateModel("Bonk", "res/models/map.ply", "res/textures/planks.png", GL_UNSIGNED_SHORT);

	Rendering::GetGameraManager()->SetCurrentCameraPosition(glm::vec3(0.0f, -1.0f, -15.0f));

	while (mainWindow.ShouldRun()) {
		Rendering::Render();
	}

	Rendering::Terminate();

	return 0;
}