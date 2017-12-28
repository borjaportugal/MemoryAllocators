/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/

#include "testing\testing.h"

#include "MemoryChunk.h"
using namespace memory;	// avoid verbosity on tests

TEST_F(memory_chunk_provides_simple_gettors)
{
	MemoryChunk chunk{ 128 };
	TEST_ASSERT(chunk.bytes() == 128);

	const auto * base = chunk.memory();
	const auto * offseted = chunk.memory() + 40;
	TEST_ASSERT(offseted == base + 40);

	TEST_ASSERT(chunk.end_of_memory() == base + 128);
}

TEST_F(memory_chunk_can_determine_if_some_memory_is_in_its_bound)
{
	MemoryChunk chunk{ 128 };
	
	TEST_ASSERT(chunk.owns(chunk.memory()));
	TEST_ASSERT(chunk.owns(chunk.memory() + 50));
	TEST_ASSERT(chunk.owns(chunk.memory() + 127));
	TEST_ASSERT(chunk.owns(chunk.memory() + 128) == false);
	TEST_ASSERT(chunk.owns(chunk.memory() - 50) == false);
}


