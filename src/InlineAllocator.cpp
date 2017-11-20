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

#include "InlineAllocator.h"


#if DEBUG_INLINE_ALLOCATOR_ENABLED

std::ostream & operator<< (std::ostream & os, const ::memory::DebugInlineAllocatorStats & stats)
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

#endif
