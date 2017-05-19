#pragma once

#include <memory>
#include <type_traits>

#include <Hypodermic/Container.h>

namespace tikal
{

template<typename TBaseView>
class View;

template<
	typename TBaseView,
	typename = std::enable_if_t<std::is_base_of<View<TBaseView>, TBaseView>::value>
>
class ViewInstantiator
{
public:
	ViewInstantiator(std::shared_ptr<Hypodermic::Container> container) :
		m_container(container)
	{}

	template<
		typename TView,
		typename = std::enable_if_t<std::is_base_of<TBaseView, TView>::value>
	>
	std::shared_ptr<TView> instantiate() const
	{
		return m_container->resolve<TView>();
	}

private:
	std::shared_ptr<Hypodermic::Container> m_container;
};

}
