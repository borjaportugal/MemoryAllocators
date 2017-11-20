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

#include "MemoryChunk.h"
using namespace memory;

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


