/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/


#pragma once

#include "MemoryCore.h"
#include "MemoryChunk.h"

namespace memory
{
	/// \brief	The StackAllocator just moves a pointer to determine the begginign 
	///			and ending of allocated memory. 
	///			Deallocations need to occur in exact reverse order to allocations.
	class StackAllocator
	{
	public:
		explicit StackAllocator(size_type bytes);
		virtual ~StackAllocator() = default;

		virtual unsigned char * allocate(size_type bytes);
		virtual void deallocate(unsigned char * mem, size_type bytes)
		{
			MEMORY_ASSERT(mem == m_top - bytes);
			m_top = mem;
		}

		bool is_full() const { return free_size() == 0; }
		size_type owns(unsigned char * mem) const { return m_memory_chunk.owns(mem); }
		size_type free_size() const
		{
			return ptr_to_num(m_memory_chunk.end_of_memory()) - ptr_to_num(m_top);
		}

	protected:
		size_type get_offset_from_base(unsigned char * ptr);

		// IMPORTANT(Borja): don't change the order of these two variables, construction order matters
		MemoryChunk m_memory_chunk;
		unsigned char * m_top{ nullptr };
	};
}

#if MEMORY_DEBUG_ENABLED

#include <deque>

namespace memory
{
	struct DebugStackAllocatorConfig
	{
		size_type padding{ 0u };
	};

	class DebugStackAllocator
		: public StackAllocator
	{
	public:
		using Base = StackAllocator;

		struct Stats
		{
			struct AllocationStats
			{
				AllocationStats(size_type s, size_type off)
					: size{ s }
					, offset{ off }
				{}

				size_type size{ 0u };
				size_type offset{ 0u };
			};

			size_type allocations{ 0 };
			size_type deallocations{ 0 };
			size_type failures{ 0 };

			// STUDY(Borja): use a linked list here? We manage it, no stl dependency...
			std::deque<AllocationStats> per_allocation_stats;
		};

	public:
		explicit DebugStackAllocator(size_type bytes);
		~DebugStackAllocator();

		unsigned char * allocate(size_type bytes) override;
		void deallocate(unsigned char * mem, size_type bytes) override;

		const Stats & get_stats() const { return m_stats; }

	private:
		Stats m_stats;
	};
}

#endif

namespace memory
{
#if MEMORY_DEBUG_ENABLED
	using DefaultStackAllocator = DebugStackAllocator;
#else
	using DefaultStackAllocator = StackAllocator;
#endif
}