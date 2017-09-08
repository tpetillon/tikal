#pragma once

#include <iterator>
#include <vector>

namespace tikal
{

class ObjectPoolIterator;

class ObjectPool
{
public:
	class Iterator : public std::iterator<
		std::bidirectional_iterator_tag,
		void*,
		ptrdiff_t,
		void*,
		void*>
	{
	public:
		static Iterator begin(ObjectPool* pool);
		static Iterator end(ObjectPool* pool);
		static Iterator rbegin(ObjectPool* pool);
		static Iterator rend(ObjectPool* pool);

		Iterator(const ObjectPool::Iterator& other);

		Iterator& operator=(const Iterator& other);

		bool operator==(const Iterator& other);
		bool operator!=(const Iterator& other);

		Iterator& operator++(); // pre-increment
		Iterator operator++(int); // post-increment

		Iterator& operator--(); // pre-decrement
		Iterator operator--(int); // post-decrement

		void* operator*() const; // dereference
		void* operator->() const; // dereference + member access

	private:
		Iterator(ObjectPool* pool, int blockIndex, uint8_t* position);
		Iterator(ObjectPool* pool, int blockIndex);

		ObjectPool* m_pool;
		int m_blockIndex;
		uint8_t* m_position;

		void moveToNext();
		void moveToPrevious();

		void moveToNextReserved();
		void moveToPreviousReserved();
	};

	ObjectPool(size_t objectSize, size_t blockSize);

	size_t objectSize() const { return m_objectSize; }
	size_t blockSize() const { return m_blockSize; }

	size_t blockCount() const { return m_blocks.size(); }

	size_t objectCount() const { return m_objectCount; }

	void* reserveLocation();
	void freeLocation(void* objectLocation);

	Iterator begin() { return Iterator::begin(this); }
	Iterator end() { return Iterator::end(this); }
	Iterator rbegin() { return Iterator::rbegin(this); }
	Iterator rend() { return Iterator::rend(this); }

private:
	const size_t m_objectSize;
	const size_t m_wrapperSize;
	const size_t m_wrappedOffset;
	const size_t m_blockSize;

	std::vector<uint8_t*> m_blocks;

	size_t m_objectCount;

	bool isReserved(void* location);
	void reserve(void* location);
	void free(void* location);

	friend ObjectPoolIterator;
};

}
