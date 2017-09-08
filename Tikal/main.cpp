#include <functional>
#include <iostream>

#include <Hypodermic/ContainerBuilder.h>

#include "Component.h"
#include "ComponentContainerBuilder.h"
#include "ComponentInstantiator.h"
#include "EventDispatcher.h"
#include "SceneObject.h"
#include "SceneRoot.h"
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
	ClassB(std::shared_ptr<ClassA> instanceA) : m_instanceA(instanceA)
	{
		std::cout << "ClassB constructor" << std::endl;
		instanceA->hello();
	}

	void say()
	{
		std::cout << "Hello from ClassB" << std::endl;
		m_instanceA->hello();
	}

private:
	std::shared_ptr<ClassA> m_instanceA;
};

/*class BaseView : public tikal::View<BaseView>
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
};*/

class ComponentA : public tikal::Component
{
public:
	ComponentA(std::shared_ptr<ClassB> bInstance) :
		m_bInstance(bInstance), m_value(12345789)
	{
		std::cout << "ComponentA constructed" << std::endl;
	}

	~ComponentA()
	{
		std::cout << "ComponentA destructed" << std::endl;
	}

	Hypodermic::TypeInfo type() const override
	{
		return Hypodermic::Utils::getMetaTypeInfo<ComponentA>();
	}

	void say() const
	{
		std::cout << "Hello from ComponentA" << std::endl;
		m_bInstance->say();
	}

private:
	std::shared_ptr<ClassB> m_bInstance;
	int m_value;
};

//class TestViewInstantiator : public tikal::ViewInstantiator<BaseView> {};

/*typedef tikal::ViewInstantiator<BaseView> TestViewInstantiator;
typedef tikal::SceneRoot<BaseView> TestSceneRoot;
typedef tikal::SceneObject<BaseView> TestSceneObject;*/

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

	//builder.registerType<ViewA>();
	//builder.registerType<ViewB>();

	auto container = builder.build();

	auto instanceA = container->resolve<ClassB>();

	/*auto viewInstantiator = TestViewInstantiator(container);

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

	ed->dispatchEvent<CharEvent>('T');*/

	{
		tikal::ComponentContainerBuilder componentContainerBuilder;
		componentContainerBuilder.registerComponent<ComponentA>();
		auto componentContainer = componentContainerBuilder.build(container);

		/*auto placement = ::operator new(sizeof(ComponentA));

		auto componentA = componentContainer->instantiateComponent<ComponentA>(placement);
		componentA->say();*/

		auto componentRepo = std::make_shared<tikal::ComponentRepository>();
		auto componentInstantiator = std::make_shared<tikal::ComponentInstantiator>(componentContainer, componentRepo);

		auto sceneRoot = tikal::SceneRoot(componentInstantiator);
		auto sceneObject = sceneRoot.createSceneObject();
		auto componentA = sceneObject->addComponent<ComponentA>();
		componentA->say();
	}

	tikal::ObjectPool pool(sizeof(int), sizeof(int) * 3);
	std::cout << "object count: " << pool.objectCount() << ", block count: " << pool.blockCount() << std::endl;
	for (int i = 0; i < 10; i++)
	{
		auto location = pool.reserveLocation();
		new (location) int(i);
	}
	for (auto location : pool)
	{
		std::cout << *static_cast<int*>(location) << " ";
	}
	std::cout << std::endl;
	std::cout << "object count: " << pool.objectCount() << ", block count: " << pool.blockCount() << std::endl;
	int l = 0;
	for (auto location : pool)
	{
		if (l % 2 == 0) pool.freeLocation(location);
		l++;
	}
	for (auto location : pool)
	{
		std::cout << *static_cast<int*>(location) << " ";
	}
	std::cout << std::endl;
	std::cout << "object count: " << pool.objectCount() << ", block count: " << pool.blockCount() << std::endl;
	for (int i = 0; i < 10; i++)
	{
		auto location = pool.reserveLocation();
		new (location) int(100 + i);
	}
	for (auto location : pool)
	{
		std::cout << *static_cast<int*>(location) << " ";
	}
	std::cout << std::endl;
	std::cout << "object count: " << pool.objectCount() << ", block count: " << pool.blockCount() << std::endl;
	for (auto location : pool)
	{
		//std::cout << "free location " << *(static_cast<int*>(location)) << std::endl;
		pool.freeLocation(location);
	}
	for (auto location : pool)
	{
		std::cout << *static_cast<int*>(location) << " ";
	}
	std::cout << std::endl;
	std::cout << "object count: " << pool.objectCount() << ", block count: " << pool.blockCount() << std::endl;
	//for (auto location : pool)
	for (auto it = pool.begin(); it != pool.end(); ++it)
	//for (auto it = pool.rbegin(); it != pool.rend(); --it)
	{
		auto location = *it;
		std::cout << *static_cast<int*>(location) << " ";
	}
	std::cout << std::endl;
}
