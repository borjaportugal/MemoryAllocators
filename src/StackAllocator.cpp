/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/

#include "StackAllocator.h"

namespace memory
{
	StackAllocator::StackAllocator(size_type bytes)
		: m_memory_chunk{ bytes }
		, m_top{ m_memory_chunk.memory() }
	{}

	unsigned char * StackAllocator::allocate(size_type bytes)
	{
		if (bytes > free_size())	return nullptr;

		auto * result = m_top;
		m_top += bytes;
		return result;
	}

	size_type StackAllocator::get_offset_from_base(unsigned char * ptr)
	{
		return ptr_to_num(ptr) - ptr_to_num(m_memory_chunk.memory());
	}
	
#if MEMORY_DEBUG_ENABLED
	DebugStackAllocator::DebugStackAllocator(size_type bytes)
		: Base{ bytes }
	{
		fill_with_pattern(DebugPattern::ACQUIRED, m_memory_chunk.memory(), m_memory_chunk.bytes());
	}
	DebugStackAllocator::~DebugStackAllocator()
	{
		fill_with_pattern(DebugPattern::RELEASED, m_memory_chunk.memory(), m_memory_chunk.bytes());
	}

	unsigned char * DebugStackAllocator::allocate(size_type bytes)
	{
		if (auto * allocated = Base::allocate(bytes))
		{
			m_stats.allocations++;
			m_stats.per_allocation_stats.emplace_back(bytes, get_offset_from_base(m_top) - bytes);
			fill_with_pattern(DebugPattern::ALLOCATED, allocated, bytes);
			return allocated;
		}

		m_stats.failures++;
		return nullptr;
	}

	void DebugStackAllocator::deallocate(unsigned char * mem, size_type bytes)
	{
		m_stats.deallocations++;
		fill_with_pattern(DebugPattern::DEALLOCATED, mem, bytes);
		Base::deallocate(mem, bytes);
	}
#endif

}