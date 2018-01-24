#pragma once

#include "ComponentContainer.h"
#include "ObjectPool.h"

#include <boost/optional.hpp>

#include <Hypodermic/TypeInfo.h>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace tikal
{

class Component;
class SceneObject;

class ComponentRepository
{
public:
	ComponentRepository();

	template<typename TComponent>
	TComponent* createComponent(std::shared_ptr<ComponentContainer> container, SceneObject* sceneObject)
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();
		auto& pool = getOrCreatePool(type, sizeof(TComponent), 32 * 1024); // TODO get pool size from... somewhere
		auto location = pool->reserveLocation();

		return container->instantiateComponent<TComponent>(location, sceneObject);
	}

	void destroyComponent(Component* component);

private:
	std::unordered_map<Hypodermic::TypeInfo, std::unique_ptr<ObjectPool>> m_componentPools;

	ObjectPool* getPool(Hypodermic::TypeInfo type) const;
	std::unique_ptr<ObjectPool>& getOrCreatePool(Hypodermic::TypeInfo type, size_t objectSize, size_t blockSize);

public:
	template<typename TComponent>
	class ComponentIterator : public std::iterator<
		std::bidirectional_iterator_tag,
		TComponent*,
		ptrdiff_t,
		TComponent*,
		TComponent*>
	{
	public:
		ComponentIterator() {}

		ComponentIterator(ObjectPool::Iterator poolIterator) :
			m_poolIterator(poolIterator)
		{}

		ComponentIterator(const ComponentIterator<TComponent>& other) :
			m_poolIterator(other.m_poolIterator)
		{}

		ComponentIterator<TComponent>& operator=(const ComponentIterator<TComponent>& other)
		{
			m_poolIterator = other.m_poolIterator;
			return *this;
		}

		bool operator==(const ComponentIterator<TComponent>& other)
		{
			//return other.m_poolIterator == m_poolIterator;
			return (!m_poolIterator.is_initialized() && !other.m_poolIterator.is_initialized()) ||
				(m_poolIterator.is_initialized() && other.m_poolIterator.is_initialized() &&
				m_poolIterator.get() == other.m_poolIterator.get());
		}

		bool operator!=(const ComponentIterator<TComponent>& other)
		{
			return !(this->operator==(other));
		}

		ComponentIterator<TComponent>& operator++() // pre-increment
		{
			if (m_poolIterator.is_initialized())
			{
				++(m_poolIterator.get());
			}
			return *this;
		}

		ComponentIterator<TComponent> operator++(int) // post-increment
		{
			if (m_poolIterator.is_initialized())
			{
				(m_poolIterator.get())++;
			}
			return *this;
		}

		ComponentIterator<TComponent>& operator--() // pre-decrement
		{
			if (m_poolIterator.is_initialized())
			{
				--(m_poolIterator.get());
			}
			return *this;
		}

		ComponentIterator<TComponent> operator--(int) // post-decrement
		{
			if (m_poolIterator.is_initialized())
			{
				(m_poolIterator.get())--;
			}
			return *this;
		}

		TComponent* operator*() const // dereference
		{
			if (m_poolIterator.is_initialized())
			{
				return static_cast<TComponent*>(*(m_poolIterator.get()));
			}
			return nullptr;
		}

		TComponent* operator->() const // dereference + member access
		{
			if (m_poolIterator.is_initialized())
			{
				return static_cast<TComponent*>(*(m_poolIterator.get()));
			}
			return nullptr;
		}

	private:
		boost::optional<ObjectPool::Iterator> m_poolIterator;
	};

	template<typename TComponent>
	ComponentIterator<TComponent> begin()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();
		auto it = m_componentPools.find(type);

		if (it != m_componentPools.end())
		{
			return ComponentIterator<TComponent>(it->second->begin());
		}

		return ComponentIterator<TComponent>();
	}

	template<typename TComponent>
	ComponentIterator<TComponent> end()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();
		auto it = m_componentPools.find(type);

		if (it != m_componentPools.end())
		{
			return ComponentIterator<TComponent>(it->second->end());
		}

		return ComponentIterator<TComponent>();
	}

	template<typename TComponent>
	ComponentIterator<TComponent> rbegin()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();
		auto it = m_componentPools.find(type);

		if (it != m_componentPools.end())
		{
			return ComponentIterator<TComponent>(it->second->rbegin());
		}

		return ComponentIterator<TComponent>();
	}

	template<typename TComponent>
	ComponentIterator<TComponent> rend()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "Type must derive from Component");

		auto type = Hypodermic::Utils::getMetaTypeInfo<TComponent>();
		auto it = m_componentPools.find(type);

		if (it != m_componentPools.end())
		{
			return ComponentIterator<TComponent>(it->second->rend());
		}

		return ComponentIterator<TComponent>();
	}
};

}
