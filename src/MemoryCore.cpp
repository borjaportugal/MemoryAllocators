
#include "MemoryCore.h"

#include <iostream>

namespace memory
{
	namespace impl
	{
		void default_out_of_memory_callback()
		{
			// endl to flush the message
			std::cerr << "Out of memory :(" << std::endl;
			MEMORY_ASSERT(false);
		}

		static out_of_memory_callback_type s_out_of_memory_callback = default_out_of_memory_callback;

		void out_of_memory()
		{
			const auto callback = get_out_of_memory_callback();
			callback();
		}
	}

	out_of_memory_callback_type get_out_of_memory_callback()
	{
		return impl::s_out_of_memory_callback;
	}
	void set_out_of_memory_callback(out_of_memory_callback_type callback)
	{
		// make sure the callback is valid
		if (!callback)
			callback = impl::default_out_of_memory_callback;

		impl::s_out_of_memory_callback = callback;
	}

	void * global_alloc(size_type n)
	{
		try
		{
			return ::operator new(n);
		}
		catch (...)
		{
			impl::out_of_memory();
		}

		// expect the user to have deallocated some memory
		return ::operator new(n);
	}
}
