/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

Copyright (c) 2017 Borja Portugal Martin

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* The name of the author may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY BORJA PORTUGAL MARTIN ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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