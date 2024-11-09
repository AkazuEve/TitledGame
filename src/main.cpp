#include <iostream>

#include "modules/rendering/Rendering.hpp"
#include "modules/Window.hpp"

#include <crtdbg.h>

int main() {
	#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	Window mainWindow(1440, 810, "Game Window");
	Rendering::Init();

	Rendering::CreateModel("Bonk", "res/models/Monke.ply", "res/textures/planks.png", GL_UNSIGNED_SHORT);

	while (mainWindow.ShouldRun()) {
		Rendering::Render();
	}

	Rendering::Terminate();

	return 0;
}