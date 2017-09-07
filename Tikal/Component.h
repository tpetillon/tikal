#pragma once

#include <Hypodermic/TypeInfo.h>

#include "Destroyable.h"

namespace tikal
{

class SceneObject;

class Component : public Destroyable
{
public:
	Component() : m_sceneObject(nullptr) {}

	virtual Hypodermic::TypeInfo type() const = 0;

	SceneObject* sceneObject() const { return m_sceneObject; }

	void attach(SceneObject* const sceneObject);

private:
	SceneObject* m_sceneObject;
};

}
