#include <functional>
#include <iostream>

#include <Hypodermic/ContainerBuilder.h>

#include "EventDispatcher.h"
#include "TestEvents.h"

void voidFunction()
{
	std::cout << "void" << std::endl;
}

void intFunction(int p, int q, int r)
{
	std::cout << "int " << p << std::endl;
}

void charFunction(const char c)
{
	std::cout << "char " << c << std::endl;
}

class NonCopyableType
{
public:
	NonCopyableType(int value) : value(value) {}

	NonCopyableType(const NonCopyableType&) = delete;
	NonCopyableType& operator=(const NonCopyableType&) = delete;
	NonCopyableType(NonCopyableType&&) = delete;

	int value;
};

void nonCopyableTypeFunction(const NonCopyableType& v)
{
	std::cout << "NonCopyableType " << v.value << std::endl;
}

class NonCopyableTypeEvent : public tikal::PayloadEvent<const NonCopyableType&> {};

class ClassA {
public:
	ClassA()
	{
		std::cout << "ClassA constructor" << std::endl;
	}

	void hello() const
	{
		std::cout << "Hello from ClassA" << std::endl;
	}
};

class ClassB {
public:
	ClassB(std::shared_ptr<ClassA> instanceA)
	{
		std::cout << "ClassB constructor" << std::endl;
		instanceA->hello();
	}
};

int main(int argc, char *argv[])
{
	std::function<void(void)> f1 = voidFunction;
	std::function<void(int, int, int)> f2 = intFunction;
	std::function<void(const char&)> f3 = charFunction;
	std::cout << "sizeof(f1): " << sizeof(f1) << std::endl;
	std::cout << "sizeof(f2): " << sizeof(f2) << std::endl;

	f1();
	f2(1, 2, 3);

	tikal::EventDispatcher ed;
	auto listenerRef1 = ed.addEventListener<CharEvent>(charFunction);

	char c = 'K';
	ed.dispatchEvent<CharEvent>(c);

	{
		auto listnerRef2 = ed.addEventListener<NonCopyableTypeEvent>(nonCopyableTypeFunction);
	}

	NonCopyableType v(64);
	ed.dispatchEvent<NonCopyableTypeEvent>(v);

	ed.addEventListener<VoidEvent>(voidFunction);
	ed.dispatchEvent<VoidEvent>();

	Hypodermic::ContainerBuilder builder;

	builder.registerType<ClassA>().singleInstance();
	builder.registerType<ClassB>().singleInstance();

	auto container = builder.build();

	auto instanceA = container->resolve<ClassB>();
}
