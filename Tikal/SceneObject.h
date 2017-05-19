#pragma once

#include <algorithm>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "ViewInstantiator.h"

namespace tikal
{

template<typename TBaseView>
class View;

template<
	typename TBaseView
	//typename = std::enable_if_t<std::is_base_of<View<TBaseView>, TBaseView>::value>
>
class SceneObject
{
	static_assert(std::is_base_of<View<TBaseView>, TBaseView>::value, "View type must inherit base view type");

public:
	SceneObject(const ViewInstantiator<TBaseView>& viewInstantiator) :
		m_viewInstantiator(viewInstantiator),
		m_parent(nullptr),
		m_active(true)
	{}

	~SceneObject()
	{}

	SceneObject(SceneObject const&) = delete;
	void operator=(SceneObject const&) = delete;

	SceneObject* parent() const { return m_parent; }
	bool hasParent() const { return m_parent != nullptr; }

	void addChild(std::unique_ptr<SceneObject> newChild)
	{
		using namespace std::string_literals;

		if (newChild.get() == this)
		{
			throw std::invalid_argument("Cannot set an object to be its own child"s);
		}

		auto object = this;
		do
		{
			if (object == newChild.get())
			{
				throw std::invalid_argument("Cannot add parent object as child as it would create a loop"s);
			}
			object = object->parent();
		} while (object != nullptr);

		newChild->m_parent = this;

		m_children.push_back(std::move(newChild));
	}

	SceneObject<TBaseView>* createChild()
	{
		auto child = std::make_unique<SceneObject<TBaseView>>(m_viewInstantiator);

		auto childPtr = child.get();

		addChild(std::move(child));

		return childPtr;
	}

	std::unique_ptr<SceneObject> removeChild(SceneObject* child)
	{
		using namespace std::string_literals;

		const auto it = std::find_if(
			m_children.begin(), m_children.end(),
			[child](std::unique_ptr<SceneObject>& c) { return c.get() == child; });

		if (it == m_children.end())
		{
			throw std::invalid_argument("Given parameter is not a child"s);
		}

		auto foundChild = std::move(*it);

		m_children.erase(it);

		return std::move(foundChild);
	}

	const std::list<std::unique_ptr<SceneObject>>& children() const { return m_children; }

	template<
		typename TView
		//typename = std::enable_if_t<std::is_base_of<TBaseView, TView>::value>
	>
	std::shared_ptr<TView> addView()
	{
		static_assert(std::is_base_of<TBaseView, TView>::value, "View type must inherit base view type");

		auto view = m_viewInstantiator.instantiate<TView>();
		m_views.push_back(view);

		// This is ugly. It would be better if assisted injection was available.
		view->m_sceneObject = this;

		return view;
	}

private:
	const ViewInstantiator<TBaseView>& m_viewInstantiator;

	SceneObject* m_parent;
	std::list<std::unique_ptr<SceneObject>> m_children;

	std::list<std::shared_ptr<TBaseView>> m_views;

	bool m_active;
};

}
