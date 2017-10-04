#pragma once

#include "CommandDescriptor.h"
#include "Event.h"
#include "EventDispatcher.h"
#include "is_base_of_template.h"
#include "PayloadEvent.h"

#include <Hypodermic/Container.h>

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

namespace tikal
{

class CommandBinder
{
public:
	CommandBinder(
		std::shared_ptr<EventDispatcher> eventDispatcher,
		std::shared_ptr<Hypodermic::Container> container) :
		m_container(container), m_eventDispatcher(eventDispatcher)
	{}

	// Passing lambdas to bindCommand() would be nice, but does not seem to be possible.
	// cf. https://stackoverflow.com/a/13359347

	template<
		typename TEvent,
		typename TPayload,
		typename... TDeps,
		typename = std::enable_if_t<is_base_of_template<TEvent, PayloadEvent>::value>
	>
	void bindCommand(std::function<void(TPayload, TDeps...)> const& command)
	{
		static_assert(
			is_base_of_template<TEvent, PayloadEvent>::value,
			"Event type must derive from PayloadEvent");
		static_assert(
			std::is_convertible<TEvent::payload, TPayload>::value,
			"Command first parameter must be assignable from payload type");

		auto&& factory = Traits::CommandDescriptor<void(TPayload, TDeps...), typename TEvent::payload>::describe();

		m_listenerReferences.push_back(m_eventDispatcher->addEventListener<TEvent>([=](typename TEvent::payload payload) {
			factory(command, *(m_container.get()))(payload);
		}));
	}

	template<
		typename TEvent,
		typename... TDeps,
		typename = std::enable_if_t<std::is_base_of<Event, TEvent>::value>
	>
	void bindCommand(std::function<void(TDeps...)> const& command)
	{
		static_assert(
			std::is_base_of<Event, TEvent>::value,
			"Event type must derive from Event");

		auto&& factory = Traits::CommandDescriptor<void(TDeps...), void>::describe();

		m_listenerReferences.push_back(m_eventDispatcher->addEventListener<TEvent>([=]() {
			factory(command, *(m_container.get()))();
		}));
	}

	template<
		typename TEvent,
		typename... TArgs
	>
	void bindCommand(void(&&command)(TArgs...))
	{
		bindCommand<TEvent>(std::function<void(TArgs...)>(command));
	}

private:
	std::shared_ptr<Hypodermic::Container> m_container;
	std::shared_ptr<EventDispatcher> m_eventDispatcher;
	std::vector<std::shared_ptr<EventDispatcher::ListenerReference>> m_listenerReferences;
};

}
