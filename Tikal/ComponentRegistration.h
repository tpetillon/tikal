#pragma once

#include <Hypodermic/TypeInfo.h>

#include <functional>

namespace Hypodermic
{
class Container;
}

namespace tikal
{

class ComponentRegistration
{
public:
	ComponentRegistration(
		Hypodermic::TypeInfo instanceType,
		std::function<void*(Hypodermic::Container&, void* const)> factory) :
		m_instanceType(instanceType), m_factory(factory)
	{}

	Hypodermic::TypeInfo instanceType() const
	{
		return m_instanceType;
	}

	std::function<void*(Hypodermic::Container&, void* const)> factory() const
	{
		return m_factory;
	}

private:
	Hypodermic::TypeInfo m_instanceType;
	std::function<void*(Hypodermic::Container&, void* const)> m_factory;
};

}
