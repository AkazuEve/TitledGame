#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../rendering/structs/MeshData.hpp"

#include "../debug/Debug.hpp"

unsigned char* LoadTextureFromFile(const char* path, int* width, int* height);

class ResourceManager {
public:
	ResourceManager() = delete;
	~ResourceManager() = delete;

	static MeshData& LoadPly(std::string filePath);

	static std::string LoadShader(std::string filePath);

	static unsigned char* LoadTexture(std::string, int* width, int* height);

	static void FreeMemory();

private:

	static MeshData LoadModelFromPLYFile(std::string filePath);
	static std::string LoadShaderFile(std::string file);
	static unsigned char* LoadTextureFromFile(const char* path, int* width, int* height);

	struct LoadedMeshData {
		MeshData data{};
		std::string path{};
	};

	struct LoadedShaderData {
		std::string shader{};
		std::string path{};
	};

	struct LoadedTexture {
		unsigned char* data{};
		std::string path{};
		int width{}, height{};
	};

	static std::vector<LoadedMeshData> loadedMeshData;
	static std::vector<LoadedShaderData> loadedShaderData;
	static std::vector<LoadedTexture> loadedTextureData;
};