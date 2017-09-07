#include "ObjectPool.h"

#include <cassert>
#include <cstdint>

namespace
{

uint8_t* allocateBlock(size_t objectSize, size_t blockSize)
{
	uint8_t* block = reinterpret_cast<uint8_t*>(::operator new(blockSize));

	for (uint8_t* ptr = block; ptr < block + blockSize; ptr += objectSize + sizeof(bool))
	{
		*(reinterpret_cast<bool*>(ptr)) = false;
	}

	return block;
}

}

namespace tikal
{

ObjectPool::ObjectPool(size_t objectSize, size_t blockSize) :
	m_objectSize(objectSize),
	m_blockSize(blockSize - (blockSize % (objectSize + sizeof(bool))))
{
	assert(blockSize >= objectSize + sizeof(bool));

	m_blocks.push_back(allocateBlock(objectSize, blockSize));
}

void* ObjectPool::reserveLocation()
{
	// Search an available slot in already allocated blocks

	for (auto block : m_blocks)
	{
		for (uint8_t* ptr = block; ptr < block + m_blockSize; ptr += m_objectSize + sizeof(bool))
		{
			if (!reinterpret_cast<bool*>(ptr))
			{
				*(reinterpret_cast<bool*>(ptr)) = true;
				return reinterpret_cast<void*>(ptr + sizeof(bool));
			}
		}
	}

	// No available slot found in already allocated blocks,
	// allocate a new one

	auto block = allocateBlock(m_objectSize, m_blockSize);

	m_blocks.push_back(block);

	return reinterpret_cast<void*>(block + sizeof(bool));
}

void ObjectPool::freeLocation(void* location)
{
	uint8_t* ptr = reinterpret_cast<uint8_t*>(location);
	*(reinterpret_cast<bool*>(ptr - sizeof(bool))) = true;
}

}
