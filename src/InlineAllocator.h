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

#pragma once

#include "FallbackAllocator.h"
#include "GlobalAllocator.h"

namespace memory
{	
	/// \brief	Stores the input number of bytes in the class instance.
	/// This class is designed to be used in the stack to provide fast allocations.
	/// IMPORTANT(Borja): deallocations are not tracked, once allocate has been called on an instance 
	///						with a total number of bytes >= to BYTES, the allocator will fail.
	template <size_type BYTES>
	class InlineMemory
	{
	public:
		static constexpr size_type total_size = BYTES;

		InlineMemory() = default;
		InlineMemory(const InlineMemory &) = delete;
		InlineMemory(InlineMemory &&) = delete;
		InlineMemory& operator =(const InlineMemory &) = delete;
		InlineMemory& operator=(InlineMemory &&) = delete;

		void * allocate(size_type n = 1)
		{
			if (free_size() < n)	return nullptr;
			auto * result = m_top;
			m_top += n;
			return result;
		}

		void deallocate(void * mem, size_type)
		{
			// we are not tracking deallocations, this memory is thought to be contiguous

			// TODO(Borja): should we handle deallocations?
		}

		bool is_full() const
		{
			return m_top >= m_memory + total_size;
		}

		bool owns(const void * mem) const
		{
			return (reinterpret_cast<const unsigned char *>(mem) - m_memory) < total_size;
		}

		size_type free_size() const
		{
			return static_cast<size_type>(total_size - (m_top - m_memory));
		}

		void free_all()
		{
			m_top = m_memory;
		}

	private:
		unsigned char * m_top{ m_memory };
		unsigned char m_memory[total_size];

	};

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

	/// \brief	Allocates memory for the given number of T elements, see InlineMemory for more details.
	///			Take first the number of elements so that we can pass just the allocator with the size and
	///			then rebind it to any type. (i.e. InlineAllocator<15>::rebind_t<int> my_alloc;)
	template <size_type N, typename T = InlineAllocatorWildcard>
	class InlineAllocator
		: private InlineMemory<sizeof(T) * N>
	{
	private:
		using Base = InlineMemory<sizeof(T) * N>;

	public:
		template <typename U>
		using rebind_t = InlineAllocator<N, U>;

		static constexpr size_type total_size = Base::total_size;
		static constexpr size_type alloc_size = sizeof(T);
		using value_type = T;

		InlineAllocator() = default;
		InlineAllocator(const InlineAllocator &) = default;
		InlineAllocator(InlineAllocator &&) = default;
		template <typename U>
		InlineAllocator(const rebind_t<U> &) {}

		virtual T * allocate(size_type n = 1)
		{
			return reinterpret_cast<T *>(Base::allocate(n * alloc_size));
		}

		virtual void deallocate(T * mem, size_type n = 1)
		{
			Base::deallocate(reinterpret_cast<void *>(mem), n);
		}

		bool is_full() const
		{
			return Base::free_size() < sizeof(T);
		}

		bool owns(const T * mem) const
		{
			return Base::owns(mem);
		}

		size_type free_size() const
		{
			return Base::free_size();
		}
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
		/// \brief	
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
			}
			template <typename U>
			DebugInlineAllocator(const DebugInlineAllocator<N, U> & other)
				: m_stats{ other.m_stats }
			{}
			~DebugInlineAllocator()
			{
				if (m_initial_non_inline_allocs != m_stats->non_inline_allocs)
					m_stats->uses_implying_non_inline_allocs++;
			}

			T * allocate(size_type n = 1) override final
			{
				m_stats->allocation_num++;
				m_stats->total_alloc_objects += n;
				if (Base::primary::free_size() < n * sizeof(T)) m_stats->non_inline_allocs++;
				return Base::allocate(n);
			}

		private:
			DebugInlineAllocatorStats * m_stats{ nullptr };
			size_type m_initial_non_inline_allocs{ 0u };
		};
	}
#endif

}


#if DEBUG_INLINE_ALLOCATOR_ENABLED

#include <ostream>
inline std::ostream & operator<< (std::ostream & os, const ::memory::DebugInlineAllocatorStats & stats)
{
	os << stats.filename << "[" << stats.line << "]: " << stats.type_name << '\n'
		<< "    Object size: " << stats.object_size
		<< ", Inlined Objects: " << stats.inline_object_num << "[#" << stats.object_size * stats.inline_object_num << " bytes]"
		<< ", Allocs: " << stats.allocation_num
		<< ", Uses: " << stats.use_num
		<< ", Average Size: " << stats.average_objects()
		<< ", Non inline alloc uses: " << stats.uses_implying_non_inline_allocs << " [" << stats.non_inline_alloc_use_percentage() << "%]"
		<< ", Non inline allocs: " << stats.non_inline_allocs << " [" << stats.non_inline_alloc_percentage() << "%]";
	return os;
}

/// \brief	Must be used to define inline allocators from which we want statistics.
#	define DEBUG_INLINE_ALLOCATOR(N, T, allocator_name, alloc_typename)														\
			using alloc_typename = ::memory::impl::DebugInlineAllocator<N, T>;												\
			static ::memory::DebugInlineAllocatorStats allocator_name ## _stats{ __FILE__, __LINE__, #T, sizeof(T), N };	\
			alloc_typename allocator_name{ allocator_name ## _stats }

#else

/// \brief	Must be used to define inline allocators from which we want statistics.
#	define DEBUG_INLINE_ALLOCATOR(N, T, allocator_name, alloc_typename) using alloc_typename = ::memory::InlineAllocator<N, T>; alloc_typename allocator_name

#endif



