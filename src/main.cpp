#include <iostream>

#include "../../ext/source/glad/glad.h"

#include "modules/rendering/Rendering.hpp"
#include "modules/window/Window.hpp"

#include "modules/resources/MemoryManager.hpp"
#include "modules/debug/Console.hpp"

#include <crtdbg.h>
#include <thread>

float getRandomFloat(int max) {
	int random = rand() % max + 1;
	int fraction = rand() % 100 + 1;

	return (float)(random + (float(fraction) / 100.0f));
}

int main() {
	#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);
	#endif

	Window mainWindow(1440, 810, "Game Window");

	MemoryManager::SetupUI();

	Console::SetupUI();

	ResourceManager::PreloadAllResources();

	Rendering::Init();

	Model* monkeModel = ModelManager::CreateModel("res/models/ps1character1.ply", "res/textures/texture.png");

	while (mainWindow.ShouldRun()) {
		monkeModel->rotation.y += 0.5f;
		Rendering::Render();
	}

	Rendering::Terminate();
	ResourceManager::FreeMemory();
	
	return 0;
}