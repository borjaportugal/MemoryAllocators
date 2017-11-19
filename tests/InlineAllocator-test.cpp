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

#include <type_traits>



// InlineAllocator

TEST_F(inline_allocator_allocates_the_memory_in_the_class)
{
	TEST_ASSERT(sizeof(InlineAllocator<128, double>) >= 128 * sizeof(double));
	TEST_ASSERT(sizeof(InlineAllocator<128, unsigned long long>) >= 128 * sizeof(unsigned long long));
}
TEST_F(inline_allocator_can_request_all_the_free_size)
{
	InlineAllocator<4, int> int_alloc;
	TEST_ASSERT(int_alloc.free_size() == sizeof(int) * 4);

	InlineAllocator<14, double> double_alloc;
	TEST_ASSERT(double_alloc.free_size() == sizeof(double) * 14);
}
TEST_F(inline_allocator_allocations_are_contiguous)
{
	InlineAllocator<4, int> int_alloc;

	int * a = int_alloc.allocate();
	int * b = int_alloc.allocate();

	TEST_ASSERT(a + 1 == b);
}
TEST_F(inline_allocator_can_allocate_more_than_one_object_at_a_time)
{
	InlineAllocator<5, int> int_alloc;

	int * a = int_alloc.allocate();
	int * b = int_alloc.allocate(2);
	int * c = int_alloc.allocate(2);

	TEST_ASSERT(a + 1 == b);
	TEST_ASSERT(b + 2 == c);
}
TEST_F(inline_allocator_can_check_if_has_more_space_left)
{
	InlineAllocator<4, int> int_alloc;

	TEST_ASSERT(int_alloc.is_full() == false);

	int * an_int = int_alloc.allocate();
	int * two_ints = int_alloc.allocate(2);

	TEST_ASSERT(int_alloc.is_full() == false);
	int * last_int = int_alloc.allocate();

	TEST_ASSERT(int_alloc.is_full());
}
TEST_F(inline_allocator_returns_null_on_failure)
{
	InlineAllocator<4, int> int_alloc;

	TEST_ASSERT(int_alloc.allocate() != nullptr);
	TEST_ASSERT(int_alloc.allocate(2) != nullptr);

	TEST_ASSERT(int_alloc.allocate(10) == nullptr);

	TEST_ASSERT(int_alloc.allocate() != nullptr);

	TEST_ASSERT(int_alloc.allocate() == nullptr);
}
TEST_F(inline_allocator_can_determine_if_a_pointer_was_allocated_by_him)
{
	InlineAllocator<4, int> int_alloc;

	int * an_int = int_alloc.allocate();
	int * two_ints = int_alloc.allocate(2);

	TEST_ASSERT(int_alloc.owns(an_int));
	TEST_ASSERT(int_alloc.owns(two_ints));

	int non_owned_by_int_alloc;
	TEST_ASSERT(int_alloc.owns(&non_owned_by_int_alloc) == false);

	const int * invalid = reinterpret_cast<int *>(reinterpret_cast<char *>(an_int) + 1);
	TEST_ASSERT(int_alloc.owns(invalid) == false);
}
TEST_F(inline_allocator_can_deallocate_objects)
{
	InlineAllocator<4, int> int_alloc;

	int * a = int_alloc.allocate();	// a 0 0 0
	int * b = int_alloc.allocate(); // a b 0 0
	int_alloc.deallocate(a);		// 0 b 0 0
	int * c = int_alloc.allocate(2);// 0 b c c

	int * x = int_alloc.allocate();
	TEST_ASSERT(x == a);		// x b c c

	int_alloc.deallocate(c, 2);	// x b 0 0
	int_alloc.deallocate(b);	// x 0 0 0

	TEST_ASSERT(int_alloc.allocate(3) == b);
}
TEST_F(inline_allocator_provides_an_interface_to_rebind_the_type)
{
	using int_alloc_type = InlineAllocator<4>::rebind_t<int>;
	using float_alloc_type = int_alloc_type::rebind_t<float>;

	bool same_type = std::is_same<float_alloc_type, InlineAllocator<4, float>>::value;
	TEST_ASSERT(same_type);

	using char_alloc_type = float_alloc_type::rebind_t<char>;
	same_type = std::is_same<char_alloc_type, InlineAllocator<4, char>>::value;
	TEST_ASSERT(same_type);
}


// DebugInlineAllocator

#if DEBUG_INLINE_ALLOCATOR_ENABLED

class DebugInlineAllocatorTest : public testing::TestCategory
{
public:
	DebugInlineAllocatorStats stats{ __FILE__, __LINE__, "int", sizeof(int), 8 };
};

TEST(DebugInlineAllocatorTest, can_generate_debug_statistics_of_inline_allocators)
{


	// simulate one call to the function where the inline allocator is defined
	{
		memory::impl::DebugInlineAllocator<8, int> alloc{ stats };

		int * a = alloc.allocate(9);	// dynamic
		int * b = alloc.allocate(6);	// inline
		int * c = alloc.allocate(6);	// dynamic
		int * d = alloc.allocate(2);	// inline

		alloc.deallocate(a);
		alloc.deallocate(b);
		alloc.deallocate(c);
		alloc.deallocate(d);
	}	// 23 objects

	TEST_ASSERT(stats.allocated_bytes() == sizeof(int) * 23);
	TEST_ASSERT(stats.non_inline_allocs == 2);
	TEST_ASSERT(stats.total_alloc_objects == 23);
}
TEST(DebugInlineAllocatorTest, inline_allocator_debug_statistics_contain_information_about_multiple_runs)
{
	// simulate one call to the function where the inline allocator is defined
	{
		memory::impl::DebugInlineAllocator<4, int> alloc{ stats };

		int * a = alloc.allocate(5);	// dynamic
		int * b = alloc.allocate(3);	// inline
		int * c = alloc.allocate(2);	// dynamic
		int * d = alloc.allocate();		// inline

		alloc.deallocate(a); alloc.deallocate(b);
		alloc.deallocate(c); alloc.deallocate(d);
	}	// 11 objects

	// simulate other call to the function where the inline allocator is defined
	{
		memory::impl::DebugInlineAllocator<4, int> alloc{ stats };

		int * a = alloc.allocate(2);	// inline
		int * b = alloc.allocate(3);	// dynamic
		int * c = alloc.allocate(2);	// inline

		alloc.deallocate(a); alloc.deallocate(b); alloc.deallocate(c);
	}	// 18 objects

	// simulate other call to the function where the inline allocator is defined
	{
		memory::impl::DebugInlineAllocator<4, int> alloc{ stats };

		int * a = alloc.allocate(1);	// inline
		int * b = alloc.allocate(2);	// inline

		alloc.deallocate(a); alloc.deallocate(b);
	}	// 21 objects

	TEST_ASSERT(stats.allocated_bytes() == sizeof(int) * 21);
	TEST_ASSERT(stats.non_inline_allocs == 3);
	TEST_ASSERT(stats.total_alloc_objects == 21);
	TEST_ASSERT(stats.use_num == 3);
	TEST_ASSERT(stats.uses_implying_non_inline_allocs == 2);
}

TEST(DebugInlineAllocatorTest, debug_inline_allocator_sets_memory_patterns)
{
	unsigned char * allocated_raw = nullptr;
	unsigned char * free_raw = nullptr;

	constexpr size_type inlined_object_num = 4;

	impl::DebugInlineAllocator<4, int> alloc{ stats };

	int * a = alloc.allocate(2);

	allocated_raw = reinterpret_cast<unsigned char *>(a);
	free_raw = allocated_raw + sizeof(int) * 2;

	for (unsigned i = 0; i < sizeof(int) * 2; ++i)
		TEST_ASSERT(allocated_raw[i] == Pattern::ALLOCATED);
	for (unsigned i = 0; i < sizeof(int) * 2; ++i)
		TEST_ASSERT(free_raw[i] == Pattern::ACQUIRED);

	alloc.deallocate(a, 2);

	for (unsigned i = 0; i < sizeof(int) * 2; ++i)
		TEST_ASSERT(allocated_raw[i] == Pattern::DEALLOCATED);
	for (unsigned i = 0; i < sizeof(int) * 2; ++i)
		TEST_ASSERT(free_raw[i] == Pattern::ACQUIRED);
}

#endif

#if DEBUG_INLINE_ALLOCATOR_ENABLED

TEST_F(debug_inline_allocator_can_be_delclared_using_a_macro_to_encapsulate_stats_creation)
{
	DEBUG_INLINE_ALLOCATOR(4, int, alloc, debug_alloc_type);
	const bool same = std::is_same<debug_alloc_type, impl::DebugInlineAllocator<4, int>>::value;
	TEST_ASSERT(same);
}

#endif
