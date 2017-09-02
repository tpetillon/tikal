#pragma once

//#include <iostream>
#include <memory>
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
	View(std::shared_ptr<EventDispatcher> eventDispatcher) : m_eventDispatcher(eventDispatcher)
	{
		//std::cout << "View constructed" << std::endl;
	}

	virtual ~View()
	{
		//std::cout << "View destructed" << std::endl;
	}

	View(View const&) = delete;
	void operator=(View const&) = delete;

	SceneObject<TBaseView>* sceneObject() const { return m_sceneObject; }

protected:
	template<typename TEvent>
	void addEventListener(std::function<void(typename TEvent::payload)> callback)
	{
		m_listenerReferences.push_back(m_eventDispatcher->addEventListener<TEvent>(callback));
	}

private:
	SceneObject<TBaseView>* m_sceneObject;

	std::shared_ptr<EventDispatcher> m_eventDispatcher;
	std::vector<std::unique_ptr<EventDispatcher::ListenerReference>> m_listenerReferences;
};

}
