#pragma once

namespace tikal
{

class Destroyable
{
public:
	Destroyable() : m_destroyed(false) {}

	virtual ~Destroyable()
	{
		m_destroyed = true;
	}

	bool isDestroyed() const { return m_destroyed; }

private:
	bool m_destroyed;
};

}
