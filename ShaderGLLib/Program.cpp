#include "Program.h"
#include <stdexcept>

namespace sgl {

	Program::Program()
	{
		program_id_ = glCreateProgram();
	}

	Program::~Program()
	{
		glDeleteProgram(program_id_);
	}

	void Program::AddShader(const Shader& shader)
	{
		glAttachShader(program_id_, shader.GetId());
	}

	void Program::LinkShader()
	{
		glLinkProgram(program_id_);
	}

	void Program::Use() const
	{
		glUseProgram(program_id_);
	}

	void Program::UniformBool(const std::string& name, bool value) const
	{
		glUniform1i(GetMemoizeUniformLocation(name), (int)value);
	}

	void Program::UniformInt(const std::string& name, int value) const
	{
		glUniform1i(GetMemoizeUniformLocation(name), value);
	}

	void Program::UniformFloat(const std::string& name, float value) const
	{
		glUniform1f(GetMemoizeUniformLocation(name), value);
	}

	void Program::UniformVector2(
		const std::string& name, 
		const sgl::vector2& vec2) const
	{
		glUniform2f(GetMemoizeUniformLocation(name), vec2.x, vec2.y);
	}

	void Program::UniformVector3(
		const std::string& name, 
		const sgl::vector3& vec3) const
	{
		glUniform3f(GetMemoizeUniformLocation(name), vec3.x, vec3.y, vec3.z);
	}

	void Program::UniformVector4(
		const std::string& name, 
		const sgl::vector4& vec4) const
	{
		glUniform4f(GetMemoizeUniformLocation(name), vec4.x, vec4.y, vec4.z, vec4.w);
	}

	void Program::UniformMatrix(
		const std::string& name, 
		const sgl::matrix& mat,
		const bool transpose /*= false*/) const
	{
		glUniformMatrix4fv(
			GetMemoizeUniformLocation(name),
			1,
			transpose ? GL_TRUE : GL_FALSE,
			&mat._11);
	}

	const int Program::GetMemoizeUniformLocation(const std::string& name) const
	{
		if (memoize_map_.find(name) == memoize_map_.end())
		{
			memoize_map_[name] = glGetUniformLocation(program_id_, name.c_str());
		}
		return memoize_map_[name];
	}

} // End namespace sgl.
