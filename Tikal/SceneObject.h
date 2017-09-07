#pragma once

#include "Component.h"
#include "ComponentInstantiator.h"

#include "Destroyable.h"

#include <Hypodermic/TypeInfo.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace tikal
{

class SceneRoot;

class SceneObject : public Destroyable
{
public:
	SceneObject(SceneRoot* const sceneRoot, ComponentInstantiator* const componentInstantiator);
	SceneObject(SceneRoot* const sceneRoot, ComponentInstantiator* const componentInstantiator, std::string name);

	~SceneObject();

	template<typename TComponent>
	TComponent* addComponent()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto component = m_componentInstantiator->instantiate<TComponent>();
		component->attach(this);

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();
		m_components[type] = static_cast<Component*>(component);

		return component;
	}

	template<typename TComponent>
	TComponent* getComponent()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();

		auto it = m_components.find(type);
		if (it != m_components.end())
		{
			return static_cast<TComponent*>(it->second);
		}
		else
		{
			// TODO throw exception?
			return nullptr;
		}
	}

	template<typename TComponent>
	void destroyComponent()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();
		
		auto it = m_components.find(type);
		if (it != m_components.end())
		{
			m_componentRepository.destroyComponent(it->second);
			m_components.erase(it);
			return;
		}
		// TODO else throw exception?
	}

private:
	SceneRoot* const m_sceneRoot;
	ComponentInstantiator* const m_componentInstantiator;

	std::string m_name;
	std::unordered_map<Hypodermic::TypeInfo, Component*> m_components;
};

}
