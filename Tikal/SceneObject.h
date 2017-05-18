#pragma once

#include <list>
#include <memory>

#include "ViewInstantiator.h"

namespace tikal
{

class View;

class SceneObject
{
public:
	SceneObject(const ViewInstantiator& viewInstantiator);
	~SceneObject();

	SceneObject(SceneObject const&) = delete;
	void operator=(SceneObject const&) = delete;

	template<typename TView>
	std::shared_ptr<TView> addView()
	{
		auto view = m_viewInstantiator.instantiate<TView>();
		m_views.push_back(view);
	}

	SceneObject* parent() const { return m_parent; }
	bool hasParent() const { return m_parent != nullptr; }

	void addChild(std::unique_ptr<SceneObject> newChild);
	std::unique_ptr<SceneObject> removeChild(SceneObject* child);

	const std::list<std::unique_ptr<SceneObject>>& children() const { return m_children; }

private:
	const ViewInstantiator& m_viewInstantiator;

	SceneObject* m_parent;
	std::list<std::unique_ptr<SceneObject>> m_children;

	std::list<std::shared_ptr<View>> m_views;

	bool m_active;
};

}
