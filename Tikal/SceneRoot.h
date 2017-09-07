#pragma once

#include "ObjectPool.h"
#include "SceneObject.h"

#include <memory>

namespace tikal
{

class ComponentInstantiator;

class SceneRoot
{
public:
	SceneRoot(std::shared_ptr<ComponentInstantiator> componentInstantiator);
	~SceneRoot();

	SceneObject* createSceneObject();
	void destroySceneObject(SceneObject* sceneObject);

	ComponentInstantiator* componentInstantiator() const
	{
		return m_componentInstantiator.get();
	}

private:
	std::shared_ptr<ComponentInstantiator> m_componentInstantiator;

	ObjectPool m_pool;
};

}
