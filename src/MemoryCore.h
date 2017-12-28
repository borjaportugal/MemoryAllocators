/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/

#pragma once

// TODO(Borja): When integrating this code in an actual project, this should go in the project configuration.
#define MEMORY_DEBUG_ENABLED 1
#define MEMORY_ENABLE_DEBUG_PATTERNS 1

#if MEMORY_DEBUG_ENABLED

#define MEMORY_ASSERT(x)	\
	do						\
	{						\
		if(!(x))			\
			__debugbreak();	\
	} while(0)

#else

#define MEMORY_ASSERT(x) do{ (void)sizeof(x); } while(0)

#endif

#include <cstddef>	// std::size_t
#include <cstring>	// std::memset

#include <functional>

namespace memory
{
	using size_type = std::size_t;

	using out_of_memory_callback_type = std::function<void()>;
	out_of_memory_callback_type get_out_of_memory_callback();
	void set_out_of_memory_callback(out_of_memory_callback_type callback);

	void * global_alloc(size_type n);
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

