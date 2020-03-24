#include "Mesh.h"
#include <iterator>
#include <fstream>
#include <sstream>
#include <gl/glew.h>
#include "Mesh.h"

namespace sgl {

	Mesh::Mesh(const std::string& file)
	{
		auto obj = LoadFromObj(file);
		if (obj.has_value()) 
		{
			std::vector<int> indices;
			std::vector<float> positions;
			std::vector<float> textures;
			std::vector<float> normals;
			for (size_t i = 0; i < obj.value().indices.size(); i++)
			{

				indices.push_back(i);

				positions.push_back(obj.value().positions[obj.value().indices[i][0]].x);
				positions.push_back(obj.value().positions[obj.value().indices[i][0]].y);
				positions.push_back(obj.value().positions[obj.value().indices[i][0]].z);
				textures.push_back(obj.value().textures[obj.value().indices[i][1]].x);
				textures.push_back(obj.value().textures[obj.value().indices[i][1]].y);
				normals.push_back(obj.value().normals[obj.value().indices[i][2]].x);
				normals.push_back(obj.value().normals[obj.value().indices[i][2]].y);
				normals.push_back(obj.value().normals[obj.value().indices[i][2]].z);

			}

			point_buffer_.BindCopy(positions.size() * sizeof(float), positions.data());
			normal_buffer_.BindCopy(normals.size() * sizeof(float), normals.data());
			texture_buffer_.BindCopy(textures.size() * sizeof(float), textures.data());
			index_buffer_.BindCopy(indices.size() * sizeof(int), indices.data());
			index_size_ = indices.size();

			glGenVertexArrays(1, &vertex_array_object_);
			glBindVertexArray(vertex_array_object_);

			point_buffer_.Bind();
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			point_buffer_.UnBind();

			normal_buffer_.Bind();
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			normal_buffer_.UnBind();

			texture_buffer_.Bind();
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			texture_buffer_.UnBind();

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glBindVertexArray(0);

		}
		else
		{
			throw std::runtime_error("Failed to load obj");
		}
	}

	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &vertex_array_object_);
	}

	void Mesh::SetTextures(std::initializer_list<std::string> values)
	{
		textures_.clear();
		textures_.assign(values);
	}

	void Mesh::Draw(
		const sgl::Program& program,
		const sgl::TextureManager& texture_manager,
		const sgl::matrix& model /*= {}*/) const
	{
		texture_manager.DisableAll();

		for (const auto& texture : textures_)
		{
			texture_manager.EnableTexture(texture);
		}

		glBindVertexArray(vertex_array_object_);

		program.UniformMatrix("model", model);

		index_buffer_.Bind();
		glDrawElements(GL_TRIANGLES, index_size_, GL_UNSIGNED_INT, nullptr);
		index_buffer_.UnBind();

		glBindVertexArray(0);
		texture_manager.DisableAll();
	}

	std::optional<sgl::Mesh::ObjFile> Mesh::LoadFromObj(const std::string& file)
	{
		std::optional<sgl::Mesh::ObjFile> optional_obj;
		sgl::Mesh::ObjFile obj_file;
		std::ifstream mesh_file(file);
		std::string str;
		if (mesh_file.is_open()) {
			while (!std::getline(mesh_file, str).eof())
			{
				//char firstChar = str[0];
				std::string delimiter = " ";
				size_t pos = 0;
				std::string token;

				std::vector<std::string> splitStr;

				while ((pos = str.find(delimiter)) != std::string::npos) {
					token = str.substr(0, pos);
					splitStr.push_back(token);
					str.erase(0, pos + delimiter.length());
				}
				splitStr.push_back(str);

				std::string identifier;
				identifier.push_back(splitStr[0][0]);
				if (splitStr[0][1] != '\0') {
					identifier.push_back(splitStr[0][1]);
				}

				if (identifier == "v")
				{
					obj_file.positions.push_back(sgl::vector3(std::stof(splitStr[1]),
						std::stof(splitStr[2]), std::stof(splitStr[3])));
				}
				else if (identifier == "f")
				{
					for (int i = 1; i <= 3; i++)
					{
						std::string delimiter = "/";
						size_t pos = 0;
						std::string token;

						std::vector<std::string> splitStr2;

						while ((pos = splitStr[i].find(delimiter)) != std::string::npos) {
							token = splitStr[i].substr(0, pos);
							splitStr2.push_back(token);
							splitStr[i].erase(0, pos + delimiter.length());
						}
						splitStr2.push_back(splitStr[i]);

						std::array<int, 3> indices;
						indices[0] = std::stoi(splitStr2[0]) - 1;
						if (splitStr2.size() > 1)
						{
							indices[1] = std::stoi(splitStr2[1]) - 1;
							indices[2] = std::stoi(splitStr2[2]) - 1;
						}

						obj_file.indices.push_back(indices);
					}
				}
				else if (identifier == "vt")
				{
					obj_file.textures.push_back(sgl::vector2(std::stof(splitStr[1]),
						std::stof(splitStr[2])));
				}
				else if (identifier == "vn")
				{
					obj_file.normals.push_back(sgl::vector3(std::stof(splitStr[1]),
						std::stof(splitStr[2]), std::stof(splitStr[3])));
				}
			}
			optional_obj = obj_file;
		}
		return optional_obj;
	}

} // End namespace sgl.
