/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/


#pragma once

#include "MemoryCore.h"

namespace memory
{
	/// \brief	Manages a block of memory, the class is responsible of the block deletion.
	class MemoryChunk
	{
	public:
		explicit MemoryChunk(size_type bytes)
			: m_memory{ reinterpret_cast<unsigned char *>(global_alloc(bytes)) }
			, m_bytes{ bytes }
		{}
		~MemoryChunk()
		{
			global_dealloc(m_memory);
			m_memory = nullptr;
		}

		unsigned char * memory() const { return m_memory; }
		size_type bytes() const { return m_bytes; }
		unsigned char * end_of_memory() const { return memory() + bytes(); }
		bool owns(unsigned char * mem) const
		{
			return ptr_to_num(mem) - ptr_to_num(memory()) < bytes();
		}

	private:
		unsigned char * m_memory{ nullptr };
		size_type m_bytes{ 0 };
	};
}

