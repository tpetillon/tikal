#pragma once

#include "ComponentRegistration.h"

#include <Hypodermic/Container.h>
#include <Hypodermic/TypeInfo.h>

#include <list>
#include <memory>
#include <unordered_map>

namespace tikal
{

class SceneObject;

class ComponentContainer
{
public:
	ComponentContainer(
		std::shared_ptr<Hypodermic::Container> container,
		std::list<std::shared_ptr<ComponentRegistration>> registrations);

	void* instantiateComponent(Hypodermic::TypeInfo type, void* const placement, SceneObject* sceneObject);

	template<typename T>
	T* instantiateComponent(void* const placement, SceneObject* sceneObject)
	{
		auto type = Hypodermic::Utils::getMetaTypeInfo<T>();
		return static_cast<T*>(instantiateComponent(type, placement, sceneObject));
	}

private:
	std::shared_ptr<Hypodermic::Container> m_container;
	std::unordered_map<Hypodermic::TypeInfo, std::shared_ptr<ComponentRegistration>> m_registrations;
};

}
