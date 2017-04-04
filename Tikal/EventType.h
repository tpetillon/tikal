#pragma once

namespace tikal
{

class VoidEventType
{
	VoidEventType() = delete;
	VoidEventType(const VoidEventType&) = delete;
	VoidEventType(VoidEventType&&) = delete;
};

class EventWithPayload
{

};

template<typename TPayload>
class EventType : public EventWithPayload
{
public:
	TPayload payload;

	EventType() = delete;
	EventType(const EventType&) = delete;
	EventType(EventType&&) = delete;

	//virtual int value() const = 0;
	/*virtual int value() const
	{
		return 0;
	}*/
};

}
