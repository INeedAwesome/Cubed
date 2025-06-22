#include "ModelOBJParser.h"

#include <sstream>

namespace Cubed {

	OBJModel ReadModelFromDisk(const std::string& filepath)
	{
		OBJModel model{};

		std::ifstream stream(filepath);

		if (!stream.is_open())
			return model;

		std::string line;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> textureCoordinates;

		while (getline(stream, line))
		{
			if (line.starts_with("v "))
			{
				std::istringstream ss(line);
				std::string prefix; // v
				glm::vec3 pos{};
				ss >> prefix >> pos.x >> pos.y >> pos.z;

				positions.push_back(pos);
			}
			else if (line.starts_with("vn "))
			{
				std::istringstream ss(line);
				std::string prefix; // vn
				glm::vec3 normal{};
				ss >> prefix >> normal.x >> normal.y >> normal.z;

				normals.push_back(normal);
			}
			else if (line.starts_with("vt "))
			{
				std::istringstream ss(line);
				std::string prefix; // vt
				glm::vec2 tex{};
				ss >> prefix >> tex.x >> tex.y;

				textureCoordinates.push_back(tex);
			}

			// Final stage, "generate" the vertices to be rendered
			else if (line.starts_with("f "))
			{
				std::istringstream ss(line);
				std::string prefix;
				ss >> prefix; // Remove the f 

				std::vector<uint32_t> faceVertexIndices;

				// Runs for every vertex defined 

				while (ss.good()) // still have a 'vX/vtX/vnX' left
				{
					std::string vertexData;
					ss >> vertexData;

					std::vector<uint32_t> vertices = SplitVertexData(vertexData, "/");

					Vertex vert{};
					vert.Position = positions[vertices[0] - 1];
					vert.UV = textureCoordinates[vertices[1] - 1];
					vert.Normal = normals[vertices[2] - 1];

					model.Vertices.push_back(vert);

					// This saves the vertex to be added to the indices later since it resets for every line
					faceVertexIndices.push_back((uint32_t)model.Vertices.size() - 1);
				}

				// Runs once per line

				if (faceVertexIndices.size() == 3)
				{
					model.Indices.push_back(faceVertexIndices[2]); // the obj renders COUNTER clockwise, we render CLOCKWISE, 
					model.Indices.push_back(faceVertexIndices[1]); // to negate that we reverse the order in which we draw by changing the indices
					model.Indices.push_back(faceVertexIndices[0]);
				}
				else if (faceVertexIndices.size() == 4)
				{
					model.Indices.push_back(faceVertexIndices[2]); // Split quad into two triangles, reversed for clockwise
					model.Indices.push_back(faceVertexIndices[1]);
					model.Indices.push_back(faceVertexIndices[0]);

					model.Indices.push_back(faceVertexIndices[0]);
					model.Indices.push_back(faceVertexIndices[3]);
					model.Indices.push_back(faceVertexIndices[2]);
				}
			}
		}
		stream.close();
		return model;
	}
}

