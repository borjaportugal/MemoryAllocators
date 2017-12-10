/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/


#include "testing\testing.h"

#include "StackAllocator.h"
using namespace memory;

// StackAllocator

class StackAllocatorTest : public testing::TestCategory
{
public:
	StackAllocator alloc{ 16 };
};

TEST(StackAllocatorTest, stack_allocator_allocates_the_requested_size)
{
	TEST_ASSERT(alloc.free_size() == 16);
}

TEST(StackAllocatorTest, stack_allocator_can_allocate_memory)
{
	alloc.allocate(5);
	TEST_ASSERT(alloc.free_size() == 11);

	alloc.allocate(6);
	TEST_ASSERT(alloc.free_size() == 5);
}

TEST(StackAllocatorTest, stack_allocator_can_deallocate_memory)
{
	auto * mem = alloc.allocate(5);
	alloc.deallocate(mem, 5);
	TEST_ASSERT(alloc.free_size() == 16);
}

TEST(StackAllocatorTest, stack_allocator_deallocated_memory_can_be_reused)
{
	auto * mem = alloc.allocate(5);
	alloc.deallocate(mem, 5);

	auto * mem2 = alloc.allocate(1);
	TEST_ASSERT(mem == mem2);
}

TEST(StackAllocatorTest, stack_allocator_deallocated_memory_can_determine_if_is_full)
{
	auto * a = alloc.allocate(14);
	TEST_ASSERT(alloc.is_full() == false);

	auto * b = alloc.allocate(2);
	TEST_ASSERT(alloc.is_full() == true);

	alloc.deallocate(b, 2);
	TEST_ASSERT(alloc.is_full() == false);

	alloc.deallocate(a, 14);
	TEST_ASSERT(alloc.is_full() == false);
}

TEST(StackAllocatorTest, stack_allocator_retunrs_null_when_cannot_allocated_the_requested_size)
{
	TEST_ASSERT(alloc.allocate(4) != nullptr);
	TEST_ASSERT(alloc.allocate(10) != nullptr);
	TEST_ASSERT(alloc.allocate(3) == nullptr);
	TEST_ASSERT(alloc.allocate(2) != nullptr);
}


// DebugStackAllocator

#if MEMORY_DEBUG_ENABLED

TEST_F(debug_stack_allocator_generates_statistics)
{
	DebugStackAllocator alloc{ 16 };

	auto * a = alloc.allocate(8);
	auto * b = alloc.allocate(4);
	auto * c = alloc.allocate(3);
	alloc.allocate(100);
	alloc.deallocate(c, 3);

	const auto & stats = alloc.get_stats();
	TEST_ASSERT(stats.allocations == 3);
	TEST_ASSERT(stats.deallocations == 1);
	TEST_ASSERT(stats.failures == 1);

	TEST_ASSERT(stats.per_allocation_stats.size() == 3);
	TEST_ASSERT(stats.per_allocation_stats[0].size == 8);
	TEST_ASSERT(stats.per_allocation_stats[0].offset == 0);
	TEST_ASSERT(stats.per_allocation_stats[1].size == 4);
	TEST_ASSERT(stats.per_allocation_stats[1].offset == 8);
	TEST_ASSERT(stats.per_allocation_stats[2].size == 3);
	TEST_ASSERT(stats.per_allocation_stats[2].offset == 12);
}

TEST_F(debug_stack_allocator_fills_the_memory_with_patternss)
{
	DebugStackAllocator alloc{ 16 };

	constexpr size_type allocated_bytes = 10;
	constexpr size_type free_bytes = 6;

	auto * a = alloc.allocate(allocated_bytes);
	unsigned char * allocated_raw = reinterpret_cast<unsigned char *>(a);
	unsigned char * free_raw = allocated_raw + allocated_bytes;

	TEST_ASSERT_ALL(allocated_raw, allocated_raw + allocated_bytes,
					== DebugPattern::ALLOCATED);

	TEST_ASSERT_ALL(free_raw, free_raw + free_bytes,
					== DebugPattern::ACQUIRED);

	alloc.deallocate(a, allocated_bytes);

	TEST_ASSERT_ALL(allocated_raw, allocated_raw + allocated_bytes,
					== DebugPattern::DEALLOCATED);

	TEST_ASSERT_ALL(free_raw, free_raw + free_bytes,
					== DebugPattern::ACQUIRED);
}

#endif
