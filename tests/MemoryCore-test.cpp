
#include "MemoryCore.h"

#include "testing\testing.h"

using namespace memory;	// avoid verbosity on tests

TEST_F(can_convert_between_common_memory_units)
{
	TEST_ASSERT(kilobyte_to_byte(1) == 1024);
	TEST_ASSERT(kilobyte_to_byte(2) == 2048);

	TEST_ASSERT(megabyte_to_byte(1) == 1024 * 1024);
	TEST_ASSERT(megabyte_to_byte(2) == 2048 * 1024);
}

#ifndef _DEBUG

TEST_F(when_we_run_out_of_memory_the_callback_is_called)
{
	// static so that we can access it from the lambda
	static bool callback_called = false;

	std::vector<void *> allocated_chunks;
	TEST_ON_EXIT()
	{
		// make sure is false, in case we execute more than once the test
		callback_called = false;

		set_out_of_memory_callback(nullptr);

		for (auto * mem : allocated_chunks)
			global_dealloc(mem);
	};

	set_out_of_memory_callback([&]()
	{
		callback_called = true;

		global_dealloc(allocated_chunks.back());
		allocated_chunks.pop_back();
	});

	const auto alloc_size = megabyte_to_byte(128);
	while (!callback_called)
	{
		allocated_chunks.emplace_back(global_alloc(alloc_size));
	}

	TEST_ASSERT(callback_called);
}

#endif