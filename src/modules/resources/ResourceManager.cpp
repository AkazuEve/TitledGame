#include "ResourceManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../ext/source/stb/stbi.h"

std::vector<ResourceManager::LoadedMeshData> ResourceManager::loadedMeshData;
std::vector<ResourceManager::LoadedShaderData> ResourceManager::loadedShaderData;
std::vector<ResourceManager::LoadedTexture> ResourceManager::loadedTextureData;

void ResourceManager::PreloadAllResources() {
	std::string modelsFolder = "res/models/";
	std::string shadersFolder = "res/shaders/";
	std::string texturesFolder = "res/textures/";

	for (const auto& entry : std::filesystem::directory_iterator(modelsFolder)) {
		if (entry.file_size() > 60000) {
			DEBUGPRINT("Preloading: " << entry.path() << " File size: " << entry.file_size());
			LoadPly(entry.path().string());
		}
		else {
			DEBUGPRINT("File too big to preload: " << entry.path() << " File size: " << entry.file_size());
		}
	}
	
	for (const auto& entry : std::filesystem::directory_iterator(shadersFolder)){
		DEBUGPRINT("Preloading: " << entry.path());
		LoadShader(entry.path().string());
	}

	for (const auto& entry : std::filesystem::directory_iterator(texturesFolder)) {
		int x, y;
		DEBUGPRINT("Preloading: " << entry.path());
		LoadTexture(entry.path().string(), &x, &y);
	}
}

MeshData& ResourceManager::LoadPly(std::string filePath) {
	for (LoadedMeshData& loadedData : loadedMeshData) {
		if (loadedData.path == filePath) {
			DEBUGPRINT("Resource Manager:: Loaded ply data from vector");
			return loadedData.meshData;
		}
	}

	DEBUGPRINT("Resource Manager:: Loaded ply data from file");

	MeshData loadedData = LoadModelFromPLYFile(filePath);

	size_t numVertices = loadedData.vertices.size();
	size_t numIndices = loadedData.indices.size();

	std::vector<unsigned int> remap(numIndices);

	size_t optVertexCount = meshopt_generateVertexRemap(
		remap.data(),
		loadedData.indices.data(),
		numIndices,
		loadedData.vertices.data(),
		numVertices,
		sizeof(Vertex));

	DEBUGPRINT("Resource Manager:: Optimizing mesh, original vertex count: " << loadedData.vertices.size() << ", optimized count: " << optVertexCount);

	MeshData optimizedData;

	optimizedData.indices.resize(numIndices);
	optimizedData.vertices.resize(optVertexCount);

	meshopt_remapIndexBuffer(   optimizedData.indices.data(),  loadedData.indices.data(),    numIndices,  remap.data());
	meshopt_remapVertexBuffer(  optimizedData.vertices.data(), loadedData.vertices.data(),   numVertices, sizeof(Vertex), remap.data());
	meshopt_optimizeVertexCache(optimizedData.indices.data(),  optimizedData.indices.data(), numIndices,  optVertexCount);
	meshopt_optimizeOverdraw(   optimizedData.indices.data(),  optimizedData.indices.data(), numIndices,  &(optimizedData.vertices[0].pos.x), optVertexCount, sizeof(Vertex), 1.05f);
	meshopt_optimizeVertexFetch(optimizedData.vertices.data(), optimizedData.indices.data(), numIndices,  optimizedData.vertices.data(),      optVertexCount, sizeof(Vertex));

	LoadedMeshData data;
	data.meshData = optimizedData;
	data.path = filePath;


	loadedMeshData.push_back(data);

	return loadedMeshData[loadedMeshData.size() - 1].meshData;
}

std::string ResourceManager::LoadShader(std::string filePath) {
	for (LoadedShaderData& loadedData : loadedShaderData) {
		if (loadedData.path == filePath) {
			DEBUGPRINT("Resource Manager:: Loaded shader data from vector");
			return loadedData.shader;
		}
	}

	DEBUGPRINT("Resource Manager:: Loaded shader data from file");
	LoadedShaderData newLoadedData;
	newLoadedData.path = filePath;
	newLoadedData.shader = LoadShaderFile(filePath);

	loadedShaderData.push_back(newLoadedData);

	return loadedShaderData[loadedShaderData.size() - 1].shader;
}

unsigned char* ResourceManager::LoadTexture(std::string filePath, int* width, int* height) {
	for (LoadedTexture& loadedData : loadedTextureData) {
		if (loadedData.path == filePath) {
			DEBUGPRINT("Resource Manager:: Loaded texture data from vector");
			*width = loadedData.width;
			*height = loadedData.width;
			return loadedData.data;
		}
	}

	DEBUGPRINT("Resource Manager:: Loaded texture data from file");
	LoadedTexture newLoadedData;
	newLoadedData.path = filePath;
	newLoadedData.data = LoadTextureFromFile(filePath.c_str(), width, height);
	newLoadedData.width = *width;
	newLoadedData.height = *height;

	loadedTextureData.push_back(newLoadedData);

	return loadedTextureData[loadedTextureData.size() - 1].data;
}

void ResourceManager::FreeMemory() {
	for (LoadedTexture& loadedData : loadedTextureData) {
		stbi_image_free(loadedData.data);
	}
}


MeshData ResourceManager::LoadModelFromPLYFile(std::string filePath) {
	std::ifstream file{ filePath };
	if (!file.is_open())
		throw std::exception("Failed to open file");

	DEBUGPRINT("Loading ply file: " << filePath);

	MeshData data;
	Vertex veretx;
	unsigned int tmpInt;

	std::string line;
	std::string word;
	std::stringstream stream;

	unsigned int vertexCount = 0;
	unsigned int triangleCount = 0;


	while (std::getline(file, line)) {
		stream.clear();
		stream.str(line);

		stream >> word;

		//Here I read how many vertices and indices the model has
		//For now I don't care for the layout just want it to work
		if (word == "element") {

			stream >> word;

			if (word == "vertex") {
				stream >> vertexCount;
				data.vertices.reserve(vertexCount);
				DEBUGPRINT("Vertex count: " << vertexCount);
			}

			if (word == "face") {
				stream >> triangleCount;
				data.indices.reserve(3 * triangleCount);
				DEBUGPRINT("Triangle count: " << triangleCount);
			}
		}

		//End of header time for data parsing
		if (word == "end_header") {
			//This just contains vertex data
			for (unsigned int i{ 0 }; i < vertexCount; i++) {
				std::getline(file, line);
				stream.clear();
				stream.str(line);

				stream >> veretx.pos.x >> veretx.pos.y >> veretx.pos.z;
				stream >> veretx.nor.x >> veretx.nor.y >> veretx.nor.z;
				stream >> veretx.tex.x >> veretx.tex.y;

				data.vertices.emplace_back(veretx);

			}

			//This line will have a int for vectex amount per face then data >:|
			for (unsigned int i{ 0 }; i < triangleCount; i++) {
				std::getline(file, line);
				stream.clear();
				stream.str(line);

				stream >> tmpInt;

				for (unsigned int i{ 0 }; i < 3; i++) {
					stream >> tmpInt;
					data.indices.emplace_back(tmpInt);
				}

			}
		}
	}
	return data;
}

std::string ResourceManager::LoadShaderFile(std::string file) {
	std::string shaderString;
	std::ifstream inFile(file.c_str(), std::ios::ate);

	//Just open text and dunk into string
	if (inFile.is_open()) {
		shaderString.reserve(inFile.tellg());
		inFile.seekg(0, std::ios::beg);
		shaderString.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
	}
	else {
		throw std::runtime_error("Failed to load shader file");
	}

	return shaderString;
}

unsigned char* ResourceManager::LoadTextureFromFile(const char* path, int* width, int* height) {
	// Flip horizontally so OpenGL images are not flipped
	stbi_set_flip_vertically_on_load(true);

	int n{ 0 };

	// Read data from file
	unsigned char* bytes = stbi_load(path, width, height, &n, 0);

	return bytes;
}