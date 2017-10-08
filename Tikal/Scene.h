#pragma once

#include "ObjectPool.h"
#include "SceneObject.h"

#include <memory>

namespace tikal
{

class ComponentInstantiator;

class Scene
{
public:
	Scene(std::shared_ptr<ComponentInstantiator> componentInstantiator);
	~Scene();

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
