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

#include "MemoryCore.h"
#include "FallbackAllocator.h"

namespace memory
{
	/// \brief	Wraper to allocate memory when all other methods fail.
	template <typename T>
	class GlobalAllocator
	{
	public:
		using value_type = T;

		template <typename U>
		using rebind_t = GlobalAllocator<U>;

		GlobalAllocator() = default;
		template <typename U>
		GlobalAllocator(const rebind_t<U> &) {}

		static T * allocate(size_type n)
		{
			return reinterpret_cast<T *>(global_alloc(n * sizeof(T)));
		}
		static void deallocate(T * mem, size_type = 1)
		{
			return global_dealloc(reinterpret_cast<void *>(mem));
		}

		// assume we own all memory
		static bool owns(const T * p) { return p != nullptr; }
		// assume the application won't allocate more memory than the one the system can handle
		static bool is_full() { return false; }
		// assume the application won't allocate more memory than the one the system can handle
		static size_type free_size() { return ~0ul; }
	};

#if MEMORY_DEBUG_ENABLED

	/// \brief	Wraper to allocate memory when all other methods fail.
	template <typename T>
	class DebugGlobalAllocator
	{
	public:
		using value_type = T;

		template <typename U>
		using rebind_t = DebugGlobalAllocator<U>;

		DebugGlobalAllocator() = default;
		template <typename U>
		DebugGlobalAllocator(const rebind_t<U> &) {}

		static T * allocate(size_type n)
		{
			auto * result = reinterpret_cast<T *>(global_alloc(n * sizeof(T)));
			fill_with_pattern(DebugPattern::ALLOCATED, result, n * sizeof(T));
			return result;
		}
		static void deallocate(T * mem, size_type n = 1)
		{
			fill_with_pattern(DebugPattern::DEALLOCATED, mem, n * sizeof(T));
			global_dealloc(reinterpret_cast<void *>(mem));
		}

		// assume we own all memory
		static bool owns(const T * p) { return p != nullptr; }
		// assume the application won't allocate more memory than the one the system can handle
		static bool is_full() { return false; }
		// assume the application won't allocate more memory than the one the system can handle
		static size_type free_size() { return ~0ul; }
	};

	/// \brief	Expressive way to declare an allocator that has the GlobalAllocator as fallback allocator.
	template <typename ALLOC>
	using GlobalAsFallback = FallbackAllocator<
		ALLOC,
		DebugGlobalAllocator<typename ALLOC::value_type>
	>;

#else

	/// \brief	Expressive way to declare an allocator that has the GlobalAllocator as fallback allocator.
	template <typename ALLOC>
	using GlobalAsFallback = FallbackAllocator<
		ALLOC,
		GlobalAllocator<typename ALLOC::value_type>
	>;

#endif

}

