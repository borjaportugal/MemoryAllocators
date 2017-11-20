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

#define MEMORY_DEBUG_ENABLED 1

#if MEMORY_DEBUG_ENABLED

#if !defined(MEMORY_ENABLE_DEBUG_PATTERNS) 
#define MEMORY_ENABLE_DEBUG_PATTERNS 1
#endif

#define MEMORY_ASSERT(x)	\
	do						\
	{						\
		if(!(x))			\
			__debugbreak();	\
	} while(0)

#else

#define MEMORY_ASSERT(x) do{} while(0)

#endif

#include <cstddef>	// std::size_t
#include <cstring>	// std::memset

namespace memory
{
	using size_type = std::size_t;

	// TODO(Borja): callback for when we run out of memory

	inline void * global_alloc(size_type n)
	{
		return ::operator new(n);
	}
	inline void global_dealloc(void * mem)
	{
		::operator delete(mem);
	}

	inline size_type kilobyte_to_byte(size_type kb)
	{
		return kb * 1024;
	}
	inline size_type megabyte_to_byte(size_type mb)
	{
		return kilobyte_to_byte(mb * 1024);
	}

	/// \brief hHelper function to convert an address to a numerical value.
	template <typename T>
	inline size_type ptr_to_num(const T * ptr)
	{
		return reinterpret_cast<size_type>(reinterpret_cast<const size_type *>(ptr));
	}

	enum DebugPattern
	{
		ALLOCATED = 0xAA,	// returned to the user by the allocate function
		PADDING = 0xBB,		// pad to debug memory corruptions
		ACQUIRED = 0xCC,	// memory has been allocated by an allocator but not given to the user jet
		DEALLOCATED = 0xDD,	// call to deallocate
		RELEASED = 0xFF,	// memory that no longer belongs to allocators (could be freed memory or stack memory)
	};
	
#if MEMORY_ENABLE_DEBUG_PATTERNS

	inline void fill_with_pattern(DebugPattern pattern, void * mem, size_type n)
	{
		std::memset(mem, static_cast<unsigned char>(pattern), n);
	}
	
#else

	inline void fill_with_pattern(DebugPattern, void *, size_type) {}
#endif
}

