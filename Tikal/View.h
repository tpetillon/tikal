#pragma once

//#include <iostream>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "EventDispatcher.h"

namespace tikal
{

template<typename TBaseView>
class SceneObject;

template<typename TBaseView>
class View
{
	friend SceneObject<TBaseView>;

public:
	View()
	{
		//std::cout << "View constructed" << std::endl;
	}

	virtual ~View()
	{
		//std::cout << "View destructed" << std::endl;
	}

	View(View const&) = delete;
	void operator=(View const&) = delete;

	void attach(SceneObject<TBaseView>* sceneObject, std::shared_ptr<EventDispatcher> eventDispatcher)
	{
		m_sceneObject = sceneObject;
		m_eventDispatcher = eventDispatcher;

		onAttached();
	}

	virtual void onAttached() {}

	void detach()
	{
		m_listenerReferences.clear();

		m_sceneObject = nullptr;
		m_eventDispatcher = nullptr;

		onDetached();
	}

	virtual void onDetached() {}

	SceneObject<TBaseView>* sceneObject() const { return m_sceneObject; }

protected:
	template<typename TEvent>
	void addEventListener(std::function<void(typename TEvent::payload)> callback)
	{
		if (m_eventDispatcher == nullptr)
		{
			using namespace std::string_literals;

			throw new std::runtime_error("Cannot add an event listener when the view is detached"s);
		}

		m_listenerReferences.push_back(m_eventDispatcher->addEventListener<TEvent>(callback));
	}

private:
	SceneObject<TBaseView>* m_sceneObject;

	std::shared_ptr<EventDispatcher> m_eventDispatcher;
	std::vector<std::unique_ptr<EventDispatcher::ListenerReference>> m_listenerReferences;
};

}
