#include <iostream>

#include "../../ext/source/glad/glad.h"

#include "modules/rendering/Rendering.hpp"
#include "modules/window/Window.hpp"

#include <crtdbg.h>

#ifdef _DEBUG
	size_t allocatedMemory{ 0 };

	void* __CRTDECL operator new(size_t size) {
		allocatedMemory += size;
		DEBUGPRINT("Allocation: " << size << " Usage: " << allocatedMemory);

		void* p = malloc(size);
		if(p)
			return p;
		throw std::runtime_error("Failed to allocate");
	}

	void operator delete(void* memory, size_t size) {
		allocatedMemory -= size;
		DEBUGPRINT("Deallocation: " << size << " Usage: " << allocatedMemory);

		free(memory);
	}
#endif

float getRandomFloat(int max) {
	int random = rand() % max + 1;
	int fraction = rand() % 100 + 1;

	return (float)(random + (float(fraction) / 100.0f));
}

int main() {
	#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	Window mainWindow(1440, 810, "Game Window"); 
	Rendering::Init();

	for (unsigned int i{ 0 }; i < 4000; i++) {
		Model* object = Rendering::GetModelManager()->CreateModel("res/models/untitled.ply", "res/textures/brick.png");
		object->position = glm::vec3(getRandomFloat(10) - 5, getRandomFloat(10) - 5, getRandomFloat(10) - 5);
		object->rotation = glm::vec3(getRandomFloat(180), getRandomFloat(180), getRandomFloat(180));
	}

	while (mainWindow.ShouldRun()) {
		Rendering::Render();
	}

	Rendering::Terminate();
	ResourceManager::FreeMemory();

	return 0;
}