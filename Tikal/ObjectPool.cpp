#include "ObjectPool.h"

#include <cassert>
#include <cstdint>

namespace
{

uint8_t* allocateBlock(size_t wrapperSize, size_t blockSize)
{
	uint8_t* block = reinterpret_cast<uint8_t*>(::operator new(blockSize));

	for (uint8_t* ptr = block; ptr < block + blockSize; ptr += wrapperSize)
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
	m_wrapperSize(objectSize + sizeof(bool)),
	m_wrappedOffset(sizeof(bool)),
	m_blockSize(blockSize - (blockSize % (objectSize + sizeof(bool)))),
	m_objectCount(0)
{
	assert(blockSize >= objectSize + sizeof(bool));

	m_blocks.push_back(allocateBlock(m_wrapperSize, m_blockSize));
}

void* ObjectPool::reserveLocation()
{
	// Search an available slot in already allocated blocks

	for (auto block : m_blocks)
	{
		for (uint8_t* ptr = block; ptr < block + m_blockSize; ptr += m_wrapperSize)
		{
			if (!isReserved(ptr))
			{
				reserve(ptr);
				return reinterpret_cast<void*>(ptr + m_wrappedOffset);
			}
		}
	}

	// No available slot found in already allocated blocks,
	// allocate a new one

	auto block = allocateBlock(m_wrapperSize, m_blockSize);

	m_blocks.push_back(block);

	reserve(block);
	return reinterpret_cast<void*>(block + m_wrappedOffset);
}

void ObjectPool::freeLocation(void* objectLocation)
{
	uint8_t* ptr = reinterpret_cast<uint8_t*>(objectLocation) - m_wrappedOffset;
	free(ptr);
}

bool ObjectPool::isReserved(void* location)
{
	return *(reinterpret_cast<bool*>(location));
}

void ObjectPool::reserve(void* location)
{
	*(reinterpret_cast<bool*>(location)) = true;
	m_objectCount++;
}

void ObjectPool::free(void* location)
{
	*(reinterpret_cast<bool*>(location)) = false;
	m_objectCount--;
}

ObjectPool::Iterator ObjectPool::Iterator::begin(ObjectPool* pool)
{
	auto it = ObjectPool::Iterator(pool, 0);
	
	if (!pool->isReserved(it.m_position))
	{
		it.moveToNextReserved();
	}

	return it;
}

ObjectPool::Iterator ObjectPool::Iterator::end(ObjectPool* pool)
{
	return ObjectPool::Iterator(pool, pool->m_blocks.size());
}

ObjectPool::Iterator ObjectPool::Iterator::rbegin(ObjectPool* pool)
{
	auto blockIndex = pool->m_blocks.size() - 1;
	auto block = pool->m_blocks.at(blockIndex);
	auto position = block + pool->m_blockSize - pool->m_wrapperSize;
	auto it = ObjectPool::Iterator(pool, blockIndex, position);
	
	if (!pool->isReserved(it.m_position))
	{
		it.moveToPreviousReserved();
	}

	return it;
}

ObjectPool::Iterator ObjectPool::Iterator::rend(ObjectPool* pool)
{
	return ObjectPool::Iterator(pool, -1);
}

ObjectPool::Iterator::Iterator(ObjectPool* pool, int blockIndex, uint8_t* position) :
	m_pool(pool), m_blockIndex(blockIndex), m_position(position)
{
	assert(blockIndex >= -1 && blockIndex <= static_cast<int>(pool->m_blocks.size()));
	// blockIndex == -1 -> iterator is rend()
	// blockIndex == pool->m_blocks.size() -> iterator is end()

	if (blockIndex >= 0 && blockIndex < static_cast<int>(pool->m_blocks.size()))
	{
		auto block = pool->m_blocks.at(blockIndex);

		assert(position >= block && position <= block + pool->m_blockSize - pool->m_wrapperSize);
	}
}

ObjectPool::Iterator::Iterator(ObjectPool* pool, int blockIndex) :
	m_pool(pool), m_blockIndex(blockIndex)
{
	assert(blockIndex >= -1 && blockIndex <= static_cast<int>(pool->m_blocks.size()));
	// blockIndex == -1 -> iterator is rend()
	// blockIndex == pool->m_blocks.size() -> iterator is end()

	if (blockIndex >= 0 && blockIndex < static_cast<int>(pool->m_blocks.size()))
	{
		auto block = pool->m_blocks.at(blockIndex);
		m_position = block;
	}
	else
	{
		m_position = nullptr;
	}
}

ObjectPool::Iterator::Iterator(const ObjectPool::Iterator& other) :
	m_pool(other.m_pool), m_blockIndex(other.m_blockIndex), m_position(other.m_position)
{}

ObjectPool::Iterator& ObjectPool::Iterator::operator=(const ObjectPool::Iterator& other)
{
	m_pool = other.m_pool;
	m_blockIndex = other.m_blockIndex;
	m_position = other.m_position;
	return *this;
}

bool ObjectPool::Iterator::operator==(const ObjectPool::Iterator& other)
{
	if (m_pool != other.m_pool)
	{
		return false;
	}

	if (m_blockIndex == other.m_blockIndex && m_position == other.m_position)
	{
		return true;
	}

	if (m_blockIndex < 0 && other.m_blockIndex < 0)
	{
		return true;
	}
	
	auto blockCount = static_cast<int>(m_pool->m_blocks.size());
	if (m_blockIndex >= blockCount && other.m_blockIndex >= blockCount)
	{
		return true;
	}

	return false;
}

bool ObjectPool::Iterator::operator!=(const ObjectPool::Iterator& other)
{
	return !(this->operator==(other));
}

ObjectPool::Iterator& ObjectPool::Iterator::operator++()
{
	moveToNextReserved();
	return *this;
}

ObjectPool::Iterator ObjectPool::Iterator::operator++(int)
{
	ObjectPool::Iterator it = *this;
	moveToNextReserved();
	return it;
}

ObjectPool::Iterator& ObjectPool::Iterator::operator--()
{
	moveToPreviousReserved();
	return *this;
}

ObjectPool::Iterator ObjectPool::Iterator::operator--(int)
{
	ObjectPool::Iterator it = *this;
	moveToPreviousReserved();
	return it;
}

void* ObjectPool::Iterator::operator*() const
{
	return m_position + m_pool->m_wrappedOffset;
}

void* ObjectPool::Iterator::operator->() const
{
	return m_position + m_pool->m_wrappedOffset;
}

void ObjectPool::Iterator::moveToNext()
{
	if (m_blockIndex < 0)
	{
		m_blockIndex = 0;
		auto block = m_pool->m_blocks.at(m_blockIndex);
		m_position = block;
		return;
	}

	auto block = m_pool->m_blocks.at(m_blockIndex);
	m_position += m_pool->m_wrapperSize;

	if (m_position > block + m_pool->m_blockSize - m_pool->m_wrapperSize)
	{
		m_blockIndex++;

		if (m_blockIndex < static_cast<int>(m_pool->m_blocks.size()))
		{
			block = m_pool->m_blocks.at(m_blockIndex);
			m_position = block;
		}
	}
}

void ObjectPool::Iterator::moveToPrevious()
{
	if (m_blockIndex >= static_cast<int>(m_pool->m_blocks.size()))
	{
		m_blockIndex = m_pool->m_blocks.size() - 1;
		auto block = m_pool->m_blocks.at(m_blockIndex);
		m_position = block;
		return;
	}

	auto block = m_pool->m_blocks.at(m_blockIndex);
	m_position -= m_pool->m_wrapperSize;

	if (m_position < block)
	{
		m_blockIndex--;

		if (m_blockIndex >= 0)
		{
			block = m_pool->m_blocks.at(m_blockIndex);
			m_position = block + m_pool->m_blockSize - m_pool->m_wrapperSize;
		}
	}
}

void ObjectPool::Iterator::moveToNextReserved()
{
	do
	{
		moveToNext();
	}
	while (
		*(reinterpret_cast<bool*>(m_position)) == false &&
		this->operator!=(m_pool->end()));
}

void ObjectPool::Iterator::moveToPreviousReserved()
{
	do
	{
		moveToPrevious();
	} while (
		*(reinterpret_cast<bool*>(m_position)) == false &&
		this->operator!=(m_pool->rend()));
}

}
