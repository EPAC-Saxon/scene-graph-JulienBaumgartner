#include "Scene.h"
#include <stdexcept>

namespace sgl {

	const sgl::matrix SceneMatrix::GetLocalModel(const double dt) const
	{
		if (parent_ != nullptr)
		{
			return parent_->GetLocalModel(dt) * matrix_;
		}
		return matrix_;
	}

	const std::shared_ptr<sgl::Mesh> SceneMatrix::GetLocalMesh() const
	{
		return nullptr;
	}

	const sgl::matrix SceneMesh::GetLocalModel(const double dt) const
	{
		if (parent_ != nullptr)
		{
			return parent_->GetLocalModel(dt);
		}
		sgl::matrix m;
		m.IdentityMatrix();
		return m;
	}

	const std::shared_ptr<sgl::Mesh> SceneMesh::GetLocalMesh() const
	{
		return mesh_;
	}

	void SceneTree::AddNode(
		const std::shared_ptr<Scene>& node, 
		const std::shared_ptr<Scene>& parent /*= nullptr*/)
	{
		node->SetParent(parent);
		push_back(node);
	}

	const std::shared_ptr<Scene> SceneTree::GetRoot() const
	{
		std::shared_ptr<Scene> parent = nullptr;
		for (const auto& node : *this)
		{
			if (node->IsRoot())
			{
				if (parent == nullptr)
				{
					parent = node;
				}
				else 
				{
					throw std::runtime_error("There is more than one root");
				}
			}
		}
		return { nullptr };
	}

} // End namespace sgl.
