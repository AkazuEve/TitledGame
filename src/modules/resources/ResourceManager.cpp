#include "ResourceManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../ext/source/stb/stbi.h"

MeshData LoadModelFromPLYFile(std::string filePath) {
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

std::string LoadShaderFile(std::string file) {
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

unsigned char* LoadTextureFromFile(const char* path, int* width, int* height) {
	// Flip horizontally so OpenGL images are not flipped
	stbi_set_flip_vertically_on_load(true);

	int n{ 0 };

	// Read data from file
	unsigned char* bytes = stbi_load(path, width, height, &n, 0);

	return bytes;
}