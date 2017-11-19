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


