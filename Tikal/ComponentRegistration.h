#pragma once

#include <Hypodermic/TypeInfo.h>

#include <functional>

namespace Hypodermic
{
class Container;
}

namespace tikal
{

class SceneObject;

class ComponentRegistration
{
public:
	ComponentRegistration(
		Hypodermic::TypeInfo instanceType,
		std::function<void*(Hypodermic::Container&, void* const, SceneObject*)> factory) :
		m_instanceType(instanceType), m_factory(factory)
	{}

	Hypodermic::TypeInfo instanceType() const
	{
		return m_instanceType;
	}

	std::function<void*(Hypodermic::Container&, void* const, SceneObject*)> factory() const
	{
		return m_factory;
	}

private:
	Hypodermic::TypeInfo m_instanceType;
	std::function<void*(Hypodermic::Container&, void* const, SceneObject*)> m_factory;
};

}
