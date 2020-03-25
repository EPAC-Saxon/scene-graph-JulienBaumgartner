#include "Texture.h"
#include <assert.h>
#include <GL/glew.h>
#include "Image.h"

namespace sgl {

	Texture::Texture(const std::string& file)
	{
		sgl::Image image(file);
		size_ = image.GetSize();

		glGenTextures(1, &texture_id_);
		glBindTexture(GL_TEXTURE_2D, texture_id_);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(
			GL_TEXTURE_2D, // Texture type.
			0, // Mipmap level (default: 0).
			GL_RGBA8, // Internal storage format.
			static_cast<GLsizei>(size_.first), // Width.
			static_cast<GLsizei>(size_.second), // Height.
			0, // This value must be 0.
			GL_RGBA, // Specify the pixel format.
			GL_FLOAT, // Specify the type.
			image.data()); // Pointer to the array.

		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Texture::~Texture() 
	{
		glDeleteTextures(1, &texture_id_);
	}

	void Texture::Bind(const unsigned int slot /*= 0*/) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture_id_);
	}

	void Texture::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	TextureManager::~TextureManager()
	{
		DisableAll();
	}

	bool TextureManager::AddTexture(
		const std::string& name, 
		const std::shared_ptr<sgl::Texture>& texture)
	{
		auto insert_texture = name_texture_map_.insert({name, texture});
		return insert_texture.second;
	}

	bool TextureManager::RemoveTexture(const std::string& name)
	{
		auto texture = name_texture_map_.find(name);
		if (texture != name_texture_map_.end())
		{
			name_texture_map_.erase(texture);
			return true;
		}
		return false;
	}

	void TextureManager::EnableTexture(const std::string& name) const
	{
		auto texture = name_texture_map_.find(name);
		if (texture == name_texture_map_.end())
		{
			throw std::runtime_error("Texture not found: " + name);
		}

		for (int i = 0; i < name_array_.size(); i++)
		{
			if (name_array_[i].empty())
			{
				name_array_[i] = name;
				texture->second->Bind();
				return;
			}
		}

		throw std::runtime_error("Too many textures are enabled");
	}

	void TextureManager::DisableTexture(const std::string& name) const
	{
		auto texture = name_texture_map_.find(name);
		if (texture == name_texture_map_.end())
		{
			throw std::runtime_error("Texture not found: " + name);
		}

		for (int i = 0; i < name_array_.size(); i++)
		{
			if (name_array_[i] == name)
			{
				name_array_[i] = "";
				texture->second->UnBind();
				return;
			}
		}

		throw std::runtime_error("Texture is not enabled: " + name);
	}

	void TextureManager::DisableAll() const
	{
		for (int i = 0; i < name_array_.size(); i++)
		{
			if (!name_array_[i].empty())
			{
				DisableTexture(name_array_[i]);
			}
		}
	}

} // End namespace sgl.
