#pragma once

#include <memory>
#include <type_traits>

#include "SceneObject.h"
#include "ViewInstantiator.h"

namespace tikal
{

template<typename TBaseView>
class View;

template<
	typename TBaseView
	//typename = std::enable_if_t<std::is_base_of<View<TBaseView>, TBaseView>::value>
>
class SceneRoot
{
	static_assert(std::is_base_of<View<TBaseView>, TBaseView>::value, "View type must inherit base view type");

public:
	SceneRoot(const ViewInstantiator<TBaseView> & viewInstantiator) :
		m_viewInstantiator(viewInstantiator),
		m_rootObject(std::make_unique<SceneObject<TBaseView>>(viewInstantiator))
	{
	}

	SceneObject<TBaseView>* rootObject() const
	{
		return m_rootObject.get();
	}

private:
	const ViewInstantiator<TBaseView>& m_viewInstantiator;

	std::unique_ptr<SceneObject<TBaseView>> m_rootObject;
};

}
