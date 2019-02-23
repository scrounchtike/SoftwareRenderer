
#include "MeshLoader.hpp"

#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include <functional>

namespace MeshLoader {

static std::vector<std::string> splitByRegex(const std::string& string, char regex) {
	std::stringstream ss;
	ss.str(string);
	std::string item;

	std::vector<std::string> result;
	while (std::getline(ss, item, regex)) {
		result.push_back(item);
	}
	return result;	
}
template <typename T>
static std::vector<T> split(const std::string& string, int start, char regex, std::function<double(const char*)> Convert)
{
	std::vector<std::string> strs = splitByRegex(string, regex);
	std::vector<T> result;
	for (int i = start; i < strs.size(); ++i)
	{
		result.push_back(Convert(strs[i].c_str()));
	}
	return result;
}

std::vector<float> LoadMesh(const std::string& filename, int& num_vertices)
{
	std::ifstream file;
	file.open(filename);
	if (!file.good())
		assert(!"Failed to load mesh input file!");

	// Store vertex data
	std::vector<float> vertices_;
	std::vector<float> texCoords_;
	std::vector<float> normals_;

	// Final vertex values
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> normals;

	std::string line;
	while (std::getline(file, line))
	{
		// Ignore empty lines
		if (line == "")
			continue;
		// Ignore comments
		if (line[0] == '#')
			continue;

		if (line.length() >= 2 && line.substr(0, 2) == "v ")
		{
			std::vector<float> dvertices = split<float>(line, 1, ' ', std::atof);
			vertices_.push_back(dvertices[0]);
			vertices_.push_back(dvertices[1]);
			vertices_.push_back(dvertices[2]);
		}
		else if (line.length() >= 3 && line.substr(0, 3) == "vt ")
		{
			std::vector<float> dtexcoords = split<float>(line, 1, ' ', std::atof);
			texCoords_.push_back(dtexcoords[0]);
			texCoords_.push_back(dtexcoords[1]);
		}
		else if (line.length() >= 3 && line.substr(0, 3) == "vn ")
		{
			std::vector<float> dnormals = split<float>(line, 1, ' ', std::atof);
			normals_.push_back(dnormals[0]);
			normals_.push_back(dnormals[1]);
			normals_.push_back(dnormals[2]);
		}
		else if (line.length() >= 2 && line.substr(0, 2) == "f ")
		{
			std::vector<std::string> strs = splitByRegex(line, ' ');
			assert(strs.size()-1 == 3); // We want triangles! and NOT quads!
			for (int i = 1; i <= 3; ++i)
			{
				std::vector<int> indices = split<int>(strs[i], 0, '/', std::atoi);
				// Store the final values for each triangle vertex
				vertices.push_back(vertices_[indices[0]]);
				texCoords.push_back(texCoords_[indices[1]]);
				normals.push_back(normals_[indices[2]]);
			}
		}
	}

	// Interlace the data to be VTNVTNVTN...
	std::vector<float> vertex_buffer;// (vertices.size() + texCoords.size() + normals.size());
	for (int i = 0; i < vertices.size()/3; ++i)
	{
		// V
		vertex_buffer.push_back(vertices[i * 3 + 0]);
		vertex_buffer.push_back(vertices[i * 3 + 1]);
		vertex_buffer.push_back(vertices[i * 3 + 2]);
		// T
		vertex_buffer.push_back(texCoords[i * 2 + 0]);
		vertex_buffer.push_back(texCoords[i * 2 + 1]);
		// N
		vertex_buffer.push_back(normals[i * 3 + 0]);
		vertex_buffer.push_back(normals[i * 3 + 1]);
		vertex_buffer.push_back(normals[i * 3 + 2]);
	}

	num_vertices = vertices.size() / 3;
	return vertex_buffer;
}

}