#pragma once

namespace tikal
{

template<typename TPayload>
class PayloadEvent
{
public:
	typedef TPayload payload;

	PayloadEvent() = delete;
	PayloadEvent(const PayloadEvent&) = delete;
	PayloadEvent(PayloadEvent&&) = delete;
};

}
