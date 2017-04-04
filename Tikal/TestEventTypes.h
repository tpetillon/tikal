#pragma once

#include "EventType.h"

/*class VoidEvent : public tikal::EventType<void>
{
	int value() const override
	{
		return 1;
	}
};*/

class VoidEvent : public tikal::VoidEventType {};

class CharEvent : public tikal::EventType<const char&>
{
	/*int value() const override
	{
		return 2;
	}*/
};
