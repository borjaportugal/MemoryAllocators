/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/

#include "PageAllocator.h"

#include "testing\testing.h"
using namespace memory;

TEST_F(page_allocator_computes_the_size_of_the_page_correctly)
{
	PageAllocator alloc1{ sizeof(int), 4 };
	TEST_ASSERT(alloc1.get_page_size() == sizeof(int) * 4 + sizeof(void*));

	// minimum size needs to be of a pointer
	PageAllocator alloc2{ 1, 4 };
	TEST_ASSERT(alloc2.get_page_size() == sizeof(void*) * 4 + sizeof(void*));
}

TEST_F(page_allocator_allocates_a_page_on_initialization)
{
	PageAllocator alloc1{ sizeof(int), 4, true };
	TEST_ASSERT(alloc1.allocated_pages() == 1);

	PageAllocator alloc2{ sizeof(int), 4, false };
	TEST_ASSERT(alloc2.allocated_pages() == 0);
}

TEST_F(page_allocator_allocates_pages_on_demand)
{
	PageAllocator alloc{ sizeof(int), 4, false };

	alloc.allocate();
	alloc.allocate();
	alloc.allocate();
	alloc.allocate();
	TEST_ASSERT(alloc.allocated_pages() == 1);

	alloc.allocate();
	TEST_ASSERT(alloc.allocated_pages() == 2);
}

TEST_F(page_allocator_track_objects_with_free_list)
{
	PageAllocator alloc{ sizeof(int), 4, false };

	alloc.allocate();
	auto * a0 = alloc.allocate();
	alloc.allocate();
	alloc.allocate();

	alloc.deallocate(a0);
	TEST_ASSERT(alloc.allocated_pages() == 1);

	auto * a1 = alloc.allocate();
	TEST_ASSERT(alloc.allocated_pages() == 1);
	TEST_ASSERT(a0 == a1);
}

TEST_F(page_allocator_returns_objects_at_the_correct_offsets)
{
	PageAllocator alloc{ sizeof(int), 4, true };

	// assuming first returned pointer is the last in a page
	int * a3 = reinterpret_cast<int *>(alloc.allocate());
	int * a2 = reinterpret_cast<int *>(alloc.allocate());
	int * a1 = reinterpret_cast<int *>(alloc.allocate());
	int * a0 = reinterpret_cast<int *>(alloc.allocate());

	TEST_ASSERT(a0 + 1 == a1);
	TEST_ASSERT(a1 + 1 == a2);
	TEST_ASSERT(a2 + 1 == a3);
}



