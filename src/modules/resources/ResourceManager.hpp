#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../rendering/structs/MeshData.hpp"

#include "../Debug.hpp"

MeshData LoadModelFromPLYFile(std::string filePath);

std::string LoadShaderFile(std::string file);

unsigned char* LoadTextureFromFile(const char* path, int* width, int* height);