#pragma once

#include "ComponentContainer.h"
#include "ObjectPool.h"

#include <Hypodermic/TypeInfo.h>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace tikal
{

class Component;

class ComponentRepository
{
public:
	ComponentRepository();

	template<typename TComponent>
	TComponent* createComponent(std::shared_ptr<ComponentContainer> container)
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();
		auto& pool = getOrCreatePool(type, sizeof(TComponent), 32 * 1024); // TODO get pool size from... somewhere
		auto location = pool->reserveLocation();

		return container->instantiateComponent<TComponent>(location);
	}

	void destroyComponent(Component* component);

private:
	std::unordered_map<Hypodermic::TypeInfo, std::unique_ptr<ObjectPool>> m_componentPools;

	std::unique_ptr<ObjectPool>& getOrCreatePool(Hypodermic::TypeInfo type, size_t objectSize, size_t blockSize);
};

}
