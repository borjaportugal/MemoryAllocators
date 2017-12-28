/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/

#pragma once

#include "MemoryCore.h"
#include "FallbackAllocator.h"
#include "GlobalAllocator.h"

#include <bitset>

#ifndef DEBUG_INLINE_ALLOCATOR_ENABLED 
#define DEBUG_INLINE_ALLOCATOR_ENABLED MEMORY_DEBUG_ENABLED
#endif

namespace memory
{
	/// \brief	This type should never be instantiated, serves as information
	/// in case at some point we try to instantiate it and the compiler complains.
	struct InlineAllocatorWildcard 
	{
		InlineAllocatorWildcard() = delete;
		~InlineAllocatorWildcard() = delete;
		InlineAllocatorWildcard(const InlineAllocatorWildcard &) = delete;
		InlineAllocatorWildcard(InlineAllocatorWildcard&&) = delete;
		InlineAllocatorWildcard& operator=(const InlineAllocatorWildcard &) = delete;
		InlineAllocatorWildcard& operator=(InlineAllocatorWildcard&&) = delete;
	};

#if DEBUG_INLINE_ALLOCATOR_ENABLED
	namespace impl
	{
		template <size_type N, typename T>
		class DebugInlineAllocator;
	}
#endif

	/// \brief	Allocates memory for the given number of T elements.
	///			Take first the number of elements so that we can pass just the allocator with the size and
	///			then rebind it to any type. (i.e. InlineAllocator<15>::rebind_t<int> my_alloc;)
	template <size_type N, typename T = InlineAllocatorWildcard>
	class InlineAllocator
	{
#if DEBUG_INLINE_ALLOCATOR_ENABLED
		friend class impl::DebugInlineAllocator<N, T>;
#endif

	public:
		template <typename U>
		using rebind_t = InlineAllocator<N, U>;

		static constexpr size_type object_num = N;
		static constexpr size_type object_size = sizeof(T);
		static constexpr size_type total_size = object_size * object_num;
		using value_type = T;

		InlineAllocator() = default;
		InlineAllocator(const InlineAllocator &) = default;
		InlineAllocator(InlineAllocator &&) = default;
		template <typename U>
		InlineAllocator(const rebind_t<U> &) {}
		virtual ~InlineAllocator() = default;

		virtual T * allocate(size_type n = 1)
		{
			const auto idx = find_block_for_objects(n);
			if (idx < object_num)
			{
				set_flags(idx, n, true);
				return reinterpret_cast<T *>(m_memory + idx * object_size);
			}

			return nullptr;
		}

		virtual void deallocate(T * mem, size_type n = 1)
		{
			MEMORY_ASSERT(owns(mem));
			set_flags(get_idx(mem), n, false);
		}

		bool is_full() const
		{
			return m_alloc_flags.all();
		}

		bool owns(const T * obj) const
		{
			const auto ptr_val = ptr_to_num(obj);
			const auto start = ptr_to_num(m_memory);

			// make sure object is whithin our memory and at the correct offset
			const auto base = (ptr_val - start);
			return (base < total_size) && (base % object_size == 0);
		}

		size_type free_size() const
		{
			const auto free_objects = N - m_alloc_flags.count();
			return free_objects * object_size;
		}
		
	private:
		void set_flags(size_type idx, size_type n, bool flag)
		{
			n += idx;
			for (size_type i = idx; i < n; ++i)
			{
				MEMORY_ASSERT(m_alloc_flags.test(i) != flag);
				m_alloc_flags.set(i, flag);
			}
		}
		size_type find_block_for_objects(size_type n) const
		{
			if (object_num < n)	return object_num;

			// STUDY(Borja): How could we improve this search?
			for (size_type i = 0; i <= object_num - n; ++i)
			{
				bool succeded = true;
				for (size_type j = i; j < i + n; ++j)
				{
					if (m_alloc_flags.test(j))
					{
						succeded = false;
						i = j;
						break;
					}
				}

				if (succeded)
					return i;
			}

			// no block available
			return object_num;
		}
		size_type get_idx(const T * mem) const
		{
			const auto ptr_val = ptr_to_num(mem);
			const auto start = ptr_to_num(m_memory);
			return (ptr_val - start) / object_size;
		}

		// STUDY(Borja): store int containing the number of free objects?

		std::bitset<N> m_alloc_flags;
		unsigned char m_memory[total_size];
	};

	/// \brief	The inline allocator returns nullptr when the memory is over,
	/// usually this is not the desired behavior, this is the allocator to be used when
	///	we want to be able to allocate more memory when the inline allocator 
	template <size_type N, typename T = InlineAllocatorWildcard>
	using DefaultInlineAllocator = GlobalAsFallback<InlineAllocator<N, T>>;

#if DEBUG_INLINE_ALLOCATOR_ENABLED

	/// \brief	Statistics of one of an inline allocator declared in a function.
	struct DebugInlineAllocatorStats
	{
		// TODO(Borja): we need a way to access all the inline allocators of our program.
		// TODO(Borja): Some of the data (i.e. object size and name) may not be correct if the allocator has been rebound to other type (i.e. because we used it with a list)

		DebugInlineAllocatorStats(const char * file, long l,
								  const char * name, size_type size, 
								  size_type inline_obj_num)
			: type_name{ name }
			, object_size{ size }
			, filename{ file }
			, line{ l }
			, inline_object_num{ inline_obj_num }
		{}

		float average_objects() const
		{
			return static_cast<float>(total_alloc_objects) / use_num;
		}

		size_type allocated_bytes() const
		{
			return total_alloc_objects * object_size;
		}

		float growth_percentage() const
		{
			return 100.f * static_cast<float>(average_objects()) / inline_object_num;
		}

		float non_inline_alloc_use_percentage() const
		{
			return 100.f * static_cast<float>(uses_implying_non_inline_allocs) / use_num;
		}

		float non_inline_alloc_percentage() const
		{
			return 100.f * static_cast<float>(non_inline_allocs) / total_alloc_objects;
		}

		const char * const type_name{ "" };
		const char * const filename{ "" };
		const long line{ 0u };
		const size_type inline_object_num{ 0ul };
		const size_type object_size{ 0ul };

		size_type use_num{ 0ul };
		size_type uses_implying_non_inline_allocs{ 0ul };
		size_type allocation_num{ 0ul };
		size_type non_inline_allocs{ 0ul };
		size_type total_alloc_objects{ 0ul };
	};
	
	namespace impl
	{
		/// \brief	Inline allocator that generates statistics of its allocations.
		///			The user should use the macro DEBUG_INLINE_ALLOCATOR(...) to instantiate inline allocators in which he wants statistics.
		template <size_type N, typename T = InlineAllocatorWildcard>
		class DebugInlineAllocator
			: public DefaultInlineAllocator<N, T>
		{
		private:
			using Base = DefaultInlineAllocator<N, T>;

		public:
			template <typename U>
			using rebind = DebugInlineAllocator<N, U>;
			
			explicit DebugInlineAllocator(DebugInlineAllocatorStats & stats)
				: m_stats{ &stats }
				, m_initial_non_inline_allocs{ stats.non_inline_allocs }
			{
				m_stats->use_num++;
				fill_with_pattern(DebugPattern::ACQUIRED, get_primary().m_memory, total_size);
			}
			template <typename U>
			DebugInlineAllocator(const DebugInlineAllocator<N, U> & other)
				: m_stats{ other.m_stats }
			{
				fill_with_pattern(DebugPattern::ACQUIRED, get_primary().m_memory, total_size);
			}
			~DebugInlineAllocator()
			{
				// if there was any allocation we couldn't track, track it
				if (m_initial_non_inline_allocs != m_stats->non_inline_allocs)
					m_stats->uses_implying_non_inline_allocs++;

				fill_with_pattern(DebugPattern::RELEASED, get_primary().m_memory, total_size);
			}

			T * allocate(size_type n = 1) override final
			{
				m_stats->allocation_num++;
				m_stats->total_alloc_objects += n;
				if (Base::primary::free_size() < n * sizeof(T)) m_stats->non_inline_allocs++;

				auto * result = Base::allocate(n);
				fill_with_pattern(DebugPattern::ALLOCATED, result, n * object_size);
				return result;
			}

			void deallocate(T * ptr, size_type n = 1)
			{
				fill_with_pattern(DebugPattern::DEALLOCATED, ptr, n * object_size);
				Base::deallocate(ptr, n);
			}

		private:
			DebugInlineAllocatorStats * m_stats{ nullptr };

			/// \brief	Used to check if this instance of the allocator coulnd't handle an allocation.
			size_type m_initial_non_inline_allocs{ 0u };
		};
	}
#endif

}


#if DEBUG_INLINE_ALLOCATOR_ENABLED

#include <ostream>
std::ostream & operator<< (std::ostream & os, const ::memory::DebugInlineAllocatorStats & stats);

/// \brief	Must be used to define inline allocators from which we want statistics.
#	define DEBUG_INLINE_ALLOCATOR(N, T, allocator_name, alloc_typename)														\
			using alloc_typename = ::memory::impl::DebugInlineAllocator<N, T>;												\
			static ::memory::DebugInlineAllocatorStats allocator_name ## _stats{ __FILE__, __LINE__, #T, sizeof(T), N };	\
			alloc_typename allocator_name{ allocator_name ## _stats }

#else

/// \brief	Must be used to define inline allocators from which we want statistics.
#	define DEBUG_INLINE_ALLOCATOR(N, T, allocator_name, alloc_typename) \
	using alloc_typename = ::memory::InlineAllocator<N, T>;				\
	::memory::InlineAllocator<N, T> allocator_name

#endif



