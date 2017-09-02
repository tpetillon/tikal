#include <functional>
#include <iostream>

#include <Hypodermic/ContainerBuilder.h>

#include "EventDispatcher.h"
#include "SceneObject.h"
#include "SceneRoot.h"
#include "TestEvents.h"
#include "View.h"
#include "ViewInstantiator.h"

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

class ClassA
{
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

class ClassB
{
public:
	ClassB(std::shared_ptr<ClassA> instanceA)
	{
		std::cout << "ClassB constructor" << std::endl;
		instanceA->hello();
	}

	void say()
	{
		std::cout << "Hello from ClassB" << std::endl;
	}
};

class BaseView : public tikal::View<BaseView>
{
public:
	BaseView()
	{
		std::cout << "BaseView constructed" << std::endl;
	}

	~BaseView()
	{
		std::cout << "BaseView destructed" << std::endl;
	}

	virtual void say() const = 0;
};

class ViewA : public BaseView
{
public:
	ViewA()
	{
		std::cout << "ViewA constructed" << std::endl;
	}

	~ViewA()
	{
		std::cout << "ViewA destructed" << std::endl;
	}

	void say() const override
	{
		std::cout << "Hello from ViewA" << std::endl;
	}
};

class ViewB : public BaseView
{
public:
	ViewB(std::shared_ptr<ClassB> classB)
	{
		std::cout << "ViewB constructed" << std::endl;
	}

	~ViewB()
	{
		std::cout << "ViewB destructed" << std::endl;
	}

	void onAttached() override
	{
		//addEventListener<CharEvent>(std::bind(&ViewB::onCharEvent, this));

		auto onChar = [=](char c)
		{
			this->onCharEvent(c);
		};
		addEventListener<CharEvent>(onChar);
	}

	void onDetached() override
	{
		std::cout << "ViewB detached" << std::endl;
	}

	void say() const override
	{
		std::cout << "Hello from ViewB" << std::endl;
		std::cout << "\t";
		m_classB->say();
	}

	void onCharEvent(const char& c)
	{
		std::cout << "Received char event in ViewB with payload " << c << std::endl;
	}

private:
	std::shared_ptr<ClassB> m_classB;
};

//class TestViewInstantiator : public tikal::ViewInstantiator<BaseView> {};

typedef tikal::ViewInstantiator<BaseView> TestViewInstantiator;
typedef tikal::SceneRoot<BaseView> TestSceneRoot;
typedef tikal::SceneObject<BaseView> TestSceneObject;

int main(int argc, char *argv[])
{
	std::function<void(void)> f1 = voidFunction;
	std::function<void(int, int, int)> f2 = intFunction;
	std::function<void(const char&)> f3 = charFunction;
	std::cout << "sizeof(f1): " << sizeof(f1) << std::endl;
	std::cout << "sizeof(f2): " << sizeof(f2) << std::endl;

	f1();
	f2(1, 2, 3);

	auto ed = std::make_shared<tikal::EventDispatcher>();
	auto listenerRef1 = ed->addEventListener<CharEvent>(charFunction);

	char c = 'K';
	ed->dispatchEvent<CharEvent>(c);

	{
		auto listenerRef2 = ed->addEventListener<NonCopyableTypeEvent>(nonCopyableTypeFunction);
	}

	NonCopyableType v(64);
	ed->dispatchEvent<NonCopyableTypeEvent>(v);

	ed->addEventListener<VoidEvent>(voidFunction);
	ed->dispatchEvent<VoidEvent>();

	Hypodermic::ContainerBuilder builder;

	builder.registerInstance<tikal::EventDispatcher>(ed);

	builder.registerType<ClassA>().singleInstance();
	builder.registerType<ClassB>().singleInstance();

	builder.registerType<ViewA>();
	builder.registerType<ViewB>();

	auto container = builder.build();

	auto instanceA = container->resolve<ClassB>();

	auto viewInstantiator = TestViewInstantiator(container);

	auto sceneRoot = TestSceneRoot(viewInstantiator);

	auto object1 = sceneRoot.rootObject()->createChild();
	auto viewA1 = object1->addView<ViewA>();
	assert(object1 == viewA1->sceneObject());

	auto object2 = object1->createChild();
	auto viewA2 = object2->addView<ViewA>();

	{
		auto object3 = object2->createChild();
		auto viewB1 = object3->addView<ViewB>();
		//object3->removeView(viewB1);
		//object3->removeView(viewA1);

		ed->dispatchEvent<CharEvent>('T');
	}

	//viewA1->say();
	//viewA2->say();
	//viewB1->say();

	std::function<void(BaseView&)> sayFunction = [](BaseView& view)
	{
		view.say();
	};

	//object2->setActive(false);

	std::cout << "applyToViews 1:" << std::endl;
	sceneRoot.rootObject()->applyToViews(sayFunction, true, false);

	object1->removeChild(object2);

	std::cout << "applyToViews 2:" << std::endl;
	sceneRoot.rootObject()->applyToViews(sayFunction, true, false);

	ed->dispatchEvent<CharEvent>('T');
}
