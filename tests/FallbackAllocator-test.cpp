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


#include "testing\testing.h"

#define DEBUG_INLINE_ALLOCATOR_ENABLED 1
#include "InlineAllocator.h"
using namespace memory;


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

