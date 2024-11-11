#pragma once

#include "../debug/Debug.hpp"

#include "../../ext/source/imgui/imgui.h"

class MemoryManager {
public:
	MemoryManager() = delete;
	MemoryManager(const MemoryManager&) = delete;
	~MemoryManager() = delete;

	static void SetupUI();
	static void RenderUI();

	static size_t totalAllocated;
	static size_t totalFreed;

	static size_t allocationCount;
	static size_t allocationPerFrame;
private:
};