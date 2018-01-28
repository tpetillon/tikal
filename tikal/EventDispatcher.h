#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <iostream>

#include <ctti/type_id.hpp>

#include "Event.h"
#include "is_base_of_template.h"
#include "PayloadEvent.h"

namespace tikal
{

class EventDispatcher
{
private:
	struct FunctionHolder
	{
		virtual ~FunctionHolder() {}
	};

	template<typename TFunction>
	struct FunctionHolderImpl : FunctionHolder
	{
		FunctionHolderImpl(TFunction const& function) : m_function(function) {}

		const TFunction& get() const { return m_function; }

	private:
		TFunction m_function;
	};

public:
	struct ListenerReference
	{
		ListenerReference(EventDispatcher* m_eventDispatcher, ctti::type_id_t eventTypeId, FunctionHolder* functionHolder) :
			m_eventDispatcher(m_eventDispatcher), m_eventTypeId(eventTypeId), m_functionHolder(functionHolder) {}

		~ListenerReference()
		{
			m_eventDispatcher->removeEventListener(m_eventTypeId, m_functionHolder);
		}

		ListenerReference(const ListenerReference&) = delete;
		ListenerReference& operator=(const ListenerReference&) = delete;
		ListenerReference(ListenerReference&&) = delete;

	private:
		EventDispatcher* m_eventDispatcher;
		ctti::type_id_t m_eventTypeId;
		FunctionHolder* m_functionHolder;
	};

	EventDispatcher() {}

	EventDispatcher(std::shared_ptr<EventDispatcher> parent) : m_parent(parent) {}

	template<
		typename TEvent,
		typename = std::enable_if_t<is_base_of_template<TEvent, PayloadEvent>::value>
	>
	std::unique_ptr<ListenerReference> addEventListener(std::function<void(typename TEvent::payload)> callback)
	{
		auto functionHolderImpl = new FunctionHolderImpl<std::function<void(typename TEvent::payload)>>(callback);
		auto functionHolder = static_cast<FunctionHolder*>(functionHolderImpl);

		constexpr ctti::type_id_t eventTypeId = ctti::type_id<TEvent>();

		m_listenersByEvent[eventTypeId].push_back(functionHolder);

		return std::make_unique<ListenerReference>(this, eventTypeId, functionHolder);
	}

	template<
		typename TEvent,
		typename = std::enable_if_t<std::is_base_of<Event, TEvent>::value>
	>
	std::unique_ptr<ListenerReference> addEventListener(std::function<void(void)> callback)
	{
		auto functionHolderImpl = new FunctionHolderImpl<std::function<void(void)>>(callback);
		auto functionHolder = static_cast<FunctionHolder*>(functionHolderImpl);

		constexpr ctti::type_id_t eventTypeId = ctti::type_id<TEvent>();

		m_listenersByEvent[eventTypeId].push_back(functionHolder);

		return std::make_unique<ListenerReference>(this, eventTypeId, functionHolder);
	}

	// Using enable_if_t as a template parameter instead of an argument leads to
	// an error when using the method.
	// TODO Find out why
	template<
		typename TEvent
		//typename std::enable_if_t<is_base_of_template<TEvent, PayloadEvent>::value> = false
	>
	void redispatchEventToParent(std::enable_if_t<is_base_of_template<TEvent, PayloadEvent>::value>* = nullptr)
	{
		constexpr ctti::type_id_t eventTypeId = ctti::type_id<TEvent>();
		redispatchEventToParent(eventTypeId);
	}

	// Same as above
	template<
		typename TEvent
		//typename std::enable_if_t<std::is_base_of<Event, TEvent>::value> = false
	>
	void redispatchEventToParent(std::enable_if_t<std::is_base_of<Event, TEvent>::value>* = nullptr)
	{
		constexpr ctti::type_id_t eventTypeId = ctti::type_id<TEvent>();
		redispatchEventToParent(eventTypeId);
	}

	void redispatchEventToParent(ctti::type_id_t eventTypeId)
	{
		if (!m_parent)
		{
			throw std::runtime_error("Cannot redirect event to parent without a parent");
		}

		m_redispatchedToParentEvents.insert(eventTypeId);
	}
	
	// Using enable_if_t as a template parameter instead of an argument leads to
	// an error when using the method.
	// TODO Find out why
	template<
		typename TEvent
		//std::enable_if_t<is_base_of_template<TEvent, PayloadEvent>::value>* = nullptr
	>
	void listenToParentEvent(std::enable_if_t<is_base_of_template<TEvent, PayloadEvent>::value>* = nullptr)
	{
		if (!m_parent)
		{
			throw std::runtime_error("Cannot listen to a parent event without a parent");
		}

		std::function<void(TEvent::payload)> dispatchToParent = [&](typename TEvent::payload payload)
		{
			dispatchEvent<TEvent>(payload);
		};

		m_listenersToParent.push_back(m_parent->addEventListener<TEvent>(dispatchToParent));
	}

	// Same as above
	template<
		typename TEvent
		//std::enable_if_t<std::is_base_of<Event, TEvent>::value>* = nullptr
	>
	void listenToParentEvent(std::enable_if_t<std::is_base_of<Event, TEvent>::value>* = nullptr)
	{
		if (!m_parent)
		{
			throw std::runtime_error("Cannot listen to a parent event without a parent");
		}

		std::function<void()> dispatchToParent = [&]()
		{
			dispatchEvent<TEvent>();
		};

		m_listenersToParent.push_back(m_parent->addEventListener<TEvent>(dispatchToParent));
	}

	template<
		typename TEvent,
		typename = std::enable_if_t<is_base_of_template<TEvent, PayloadEvent>::value>
	>
	void dispatchEvent(typename TEvent::payload payload) const
	{
		constexpr ctti::type_id_t eventTypeId = ctti::type_id<TEvent>();

		if (m_redispatchedToParentEvents.find(eventTypeId) != m_redispatchedToParentEvents.end())
		{
			m_parent->dispatchEvent<TEvent>(payload);
		}

		auto listeners = m_listenersByEvent.find(eventTypeId);

		if (listeners == m_listenersByEvent.end())
		{
			return;
		}

		for (auto&& holder : listeners->second)
		{
			auto holderImpl = dynamic_cast<FunctionHolderImpl<std::function<void(typename TEvent::payload)>>*>(holder);
			holderImpl->get()(payload);
		}
	}

	template<
		typename TEvent,
		typename = std::enable_if_t<std::is_base_of<Event, TEvent>::value>
	>
	void dispatchEvent() const
	{
		constexpr ctti::type_id_t eventTypeId = ctti::type_id<TEvent>();

		if (m_redispatchedToParentEvents.find(eventTypeId) != m_redispatchedToParentEvents.end())
		{
			m_parent->dispatchEvent<TEvent>();
		}

		auto listeners = m_listenersByEvent.find(eventTypeId);

		if (listeners == m_listenersByEvent.end())
		{
			return;
		}

		for (auto&& holder : listeners->second)
		{
			auto holderImpl = dynamic_cast<FunctionHolderImpl<std::function<void(void)>>*>(holder);
			holderImpl->get()();
		}
	}

private:
	std::shared_ptr<EventDispatcher> m_parent;

	std::unordered_map<ctti::type_id_t, std::vector<FunctionHolder*>> m_listenersByEvent;
	std::vector<std::unique_ptr<ListenerReference>> m_listenersToParent;
	std::unordered_set<ctti::type_id_t> m_redispatchedToParentEvents;

	void removeEventListener(ctti::type_id_t eventTypeId, FunctionHolder* holder)
	{
		auto& listeners = m_listenersByEvent[eventTypeId];
		listeners.erase(std::remove(listeners.begin(), listeners.end(), holder), listeners.end());
		delete holder;
	}
};

}
