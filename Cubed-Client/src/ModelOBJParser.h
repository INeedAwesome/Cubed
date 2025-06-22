#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "Timer.h"

#include <glm/glm.hpp>

namespace Cubed {

	static std::vector<uint32_t> SplitVertexData(const std::string& str, const std::string& delimiter) {
		std::string s = str;
		std::vector<uint32_t> tokens;
		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			tokens.push_back(stoi(token));
			s.erase(0, pos + delimiter.length());
		}
		tokens.push_back(stoi(s));

		return tokens;
	}

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 UV;
	};

	struct OBJModel 
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;

		// Size in bytes
		const uint32_t ModelSize() const { return Vertices.size() * sizeof(Vertex); }

		// Size in bytes
		const uint32_t IndicesSize() const { return Indices.size() * sizeof(uint32_t); }
	};

	OBJModel ReadModelFromDisk(const std::string& filepath);

}

