#include "MemoryManager.hpp"

#include "../rendering/UI/ImGuiUI.hpp"

size_t MemoryManager::totalAllocated{ 0 };
size_t MemoryManager::totalFreed{ 0 };

size_t MemoryManager::allocationCount{ 0 };
size_t MemoryManager::allocationPerFrame{ 0 };

inline void* operator new (size_t size) {
	MemoryManager::totalAllocated += size;
	MemoryManager::allocationCount++;
	//DEBUGPRINT("Allocation: " << size << " Usage: " << MemoryManager::totalAllocated - MemoryManager::totalFreed);

	void* p = malloc(size);
	if (p)
		return p;
	throw std::runtime_error("Failed to allocate");

}

inline void operator delete(void* memory, size_t size) {
	MemoryManager::totalFreed += size;
	//DEBUGPRINT("Deallocation: " << size << " Usage: " << MemoryManager::totalAllocated - MemoryManager::totalFreed);
	free(memory);
}

void MemoryManager::SetupUI() {
	ImGuiUIManager::AddUIFunction(RenderUI);
}

void MemoryManager::RenderUI() {
	ImGui::Begin("Memory Manager");

	static size_t prevAllocation{0};
	
	allocationPerFrame = allocationCount - prevAllocation;

	ImGui::Text("Allocated: %i \nTotal allocated: %i \nTotal freed: %i \nAllocation count: %i \nAllocation count per frame: %i", 
				totalAllocated - totalFreed, totalAllocated, totalFreed, allocationCount, allocationPerFrame);

	prevAllocation = allocationCount;

	ImGui::ProgressBar(((float)totalAllocated - (float)totalFreed) / (float)totalAllocated);

	ImGui::End();
}