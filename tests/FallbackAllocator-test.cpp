/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/


#include "testing\testing.h"

#define DEBUG_INLINE_ALLOCATOR_ENABLED 1
#include "InlineAllocator.h"
using namespace memory;	// avoid verbosity on tests


// FallbackAllocator
#include "FallbackAllocator.h"

TEST_F(fallback_allocator_requests_memory_to_primary_allocator_first)
{
	FallbackAllocator<
		InlineAllocator<4, int>,
		InlineAllocator<2, int>
	> fallback_alloc;

	TEST_ASSERT(fallback_alloc.allocate(2));
	TEST_ASSERT(fallback_alloc.allocate(1));
	TEST_ASSERT(fallback_alloc.allocate(2));
	TEST_ASSERT(fallback_alloc.allocate(1));
	TEST_ASSERT(fallback_alloc.allocate() == nullptr);
}
TEST_F(fallback_allocator_provides_simple_interface_to_not_define_the_type_in_both_allocators)
{
	FallbackAllocatorT<
		int,
		InlineAllocator<4>,
		InlineAllocator<2>
	> fallback_alloc;

	TEST_ASSERT(fallback_alloc.is_full() == false);

	TEST_ASSERT(fallback_alloc.allocate(2));	// primary
	TEST_ASSERT(fallback_alloc.allocate(1));	// primary
	TEST_ASSERT(fallback_alloc.is_full() == false);

	TEST_ASSERT(fallback_alloc.allocate(2));	// fallback, full
	TEST_ASSERT(fallback_alloc.is_full() == false);

	TEST_ASSERT(fallback_alloc.allocate(1));	// primary, full

	TEST_ASSERT(fallback_alloc.is_full());
	TEST_ASSERT(fallback_alloc.allocate() == nullptr);
}


// DefaultInlineAllocator
TEST_F(default_inline_allocator_has_some_way_to_allocate_memory_when_there_is_no_more_available_inlined_memory)
{
	DefaultInlineAllocator<16, int> alloc;

	int * a = alloc.allocate(12);
	TEST_ASSERT(a != nullptr);		// inline

	int * b = alloc.allocate(5);	// non-inline
	TEST_ASSERT(b != nullptr);

	int * c = alloc.allocate(4);	// inline
	TEST_ASSERT(c != nullptr);

	alloc.deallocate(a, 12);
	alloc.deallocate(b, 5);
	alloc.deallocate(c, 4);
}

