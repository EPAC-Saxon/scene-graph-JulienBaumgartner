#include <exception>
#include <stdexcept>
#include <chrono>
#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
	#define WINDOWS_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <iostream>
#endif
#include "Device.h"

namespace sgl {

	Device::Device(SDL_Window* sdl_window) 
	{
		// GL context.
		sdl_gl_context_ = SDL_GL_CreateContext(sdl_window);
		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
#if defined(__APPLE__)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// Initialize GLEW.
		if (GLEW_OK != glewInit())
		{
			throw std::runtime_error("couldn't initialize GLEW");
		}

		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major_version_);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor_version_);

		// Enable blending to 1 - source alpha.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Enable Z buffer.
		glEnable(GL_DEPTH_TEST);
	}

	void Device::Startup(std::pair<int, int> size)
	{
		program_ = std::make_shared<Program>();

		Shader vs(ShaderType::VERTEX_SHADER);
		Shader fs(ShaderType::FRAGMENT_SHADER);

		if (!vs.LoadFromFile("../Asset/SimpleVertex.glsl"))
		{
			std::cout << vs.GetErrorMessage() << "\n";
			exit(-2);
		}
		if (!fs.LoadFromFile("../Asset/SimpleFragment.glsl"))
		{
			std::cout << fs.GetErrorMessage() << "\n";
			exit(-2);
		}

		program_->AddShader(vs);
		program_->AddShader(fs);

		program_->LinkShader();
		program_->Use();

		sgl::matrix perspective_matrix((size.second / size.first) * 1 / std::tan(60 / 2), 0, 0, 0,
										0, 1 / std::tan(60 / 2), 0, 0,
										0, 0, -(1000 + 0.1) / (1000 - 0.1), -1,
										0, 0, -(2 * 1000 * 0.1) / (1000 - 0.1), 0);

		program_->UniformMatrix("projection", perspective_matrix);
		program_->UniformMatrix("view", camera_.LookAt());
		program_->UniformMatrix("model", sgl::matrix());

	}

	void Device::Draw(const double dt)
	{
		glClearColor(0.2, 0, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const auto& node : scene_tree_)
		{
			const auto& mesh = node->GetLocalMesh();
			if (mesh != nullptr)
			{
				mesh->Draw(*program_, texture_manager_, node->GetLocalModel(dt));
			}
		}
	}

} // End namespace sgl.
