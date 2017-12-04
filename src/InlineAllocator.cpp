/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
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
