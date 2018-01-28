#pragma once

#include <functional>
#include <memory>
#include <type_traits>

#include <Hypodermic/ArgumentPack.h>
#include <Hypodermic/Container.h>
#include <Hypodermic/IsSupportedArgument.h>

#include "ArgumentResolver.h"
#include "CommandTypologyDeducer.h"

namespace tikal
{

class SceneObject;

namespace Traits
{

	namespace Details
	{

		template <class TParent>
		struct CommandArgumentResolverInvoker
		{
			explicit CommandArgumentResolverInvoker(Hypodermic::Container* container)
				: m_container(container)
			{
			}

			template <class T, class = typename std::enable_if< !std::is_convertible< TParent, T >::value && Hypodermic::Traits::IsSupportedArgument< typename std::decay< T >::type >::value >::type>
			operator T()
			{
				return ArgumentResolver< typename std::decay< T >::type >::template resolve(m_container);
			}

		private:
			Hypodermic::Container* m_container;
		};


		template <class TCommand, class TPayload, class TArgumentPack>
		struct CommandDescriptor;

		template <class TCommand>
		struct CommandDescriptor< TCommand, void, Hypodermic::Utils::ArgumentPack<> >
		{
			static std::function< std::function<void()>(std::function<TCommand>, Hypodermic::Container*) > describe()
			{
				return [](std::function<TCommand> command, Hypodermic::Container* container)
				{
					return [command, container]() {
						command();
					};
				};
			}
		};

		template <class TCommand, class TPayload>
		struct CommandDescriptor< TCommand, TPayload, Hypodermic::Utils::ArgumentPack<> >
		{
			static std::function< std::function<void(TPayload)>(std::function<TCommand>, Hypodermic::Container*) > describe()
			{
				return [](std::function<TCommand> command, Hypodermic::Container* container)
				{
					return [command, container](TPayload payload) {
						command(payload);
					};
				};
			}
		};


		template <class TCommand, class... TAnyArgument>
		struct CommandDescriptor< TCommand, void, Hypodermic::Utils::ArgumentPack< TAnyArgument... > >
		{
			static std::function< std::function<void()>(std::function<TCommand>, Hypodermic::Container*) > describe()
			{
				return [](std::function<TCommand> command, Hypodermic::Container* container)
				{
					return [command, container]() {
						command(CommandArgumentResolverInvoker< typename TAnyArgument::Type >(container)...);
					};
				};
			}
		};

		template <class TCommand, class TPayload, class... TAnyArgument>
		struct CommandDescriptor< TCommand, TPayload, Hypodermic::Utils::ArgumentPack< TAnyArgument... > >
		{
			static std::function< std::function<void(TPayload)>(std::function<TCommand>, Hypodermic::Container*) > describe()
			{
				return [](std::function<TCommand> command, Hypodermic::Container* container)
				{
					return [command, container](TPayload payload) {
						command(payload, CommandArgumentResolverInvoker< typename TAnyArgument::Type >(container)...);
					};
				};
			}
		};

	}


	template <class TCommand, class TPayload>
	using CommandDescriptor = Details::CommandDescriptor< TCommand, TPayload, Traits::CommandTypologyDeducer< TCommand, TPayload > >;

} // namespace Traits

}
