#pragma once

#include <list>

namespace tikal
{

class ObjectPool
{
public:
	ObjectPool(size_t objectSize, size_t blockSize);

	void* reserveLocation();
	void freeLocation(void* location);

private:
	const size_t m_objectSize;
	const size_t m_blockSize;

	std::list<uint8_t*> m_blocks;
};

}
