#include <iostream>

#include "../subprojects/NamedLiblary/Rendering.hpp"
#include "modules/Window.hpp"

#include <crtdbg.h>

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Window mainWindow(1440, 810, "Game Window");
	RenderingInit();

	while (mainWindow.ShouldRun()) {
		Render();
	}

	RenderingTerminate();

	return 0;
}