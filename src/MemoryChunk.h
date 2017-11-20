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

namespace memory
{
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

