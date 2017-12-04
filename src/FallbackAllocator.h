/*!
\author Borja Portugal Martin
GitHub: https://github.com/borjaportugal

This file is subject to the license terms in the LICENSE file
found in the top-level directory of this distribution.
*/

#pragma once

#include "MemoryCore.h"

namespace memory
{
	namespace impl
	{
		// avoid including all stl type_traits for this simple thing
		template <typename T1, typename T2>
		struct is_same { static constexpr bool value = false; };
		template <typename T>
		struct is_same<T, T> { static constexpr bool value = true; };
	}

	/// \brief	Allocator that request memory to the Primary allocator and in case this fails it requests it to Fallback.
	/// This implementation has been inspired by Andrei Alexandrescu (https://youtu.be/LIb3L4vKZ7U?t=28m14s)
	template <typename Primary, typename Fallback>
	class FallbackAllocator
		: public Primary
		, public Fallback
	{
		static_assert(impl::is_same<typename Primary::value_type, typename Fallback::value_type>::value, "");

	public:
		using primary = Primary;
		using fallback = Fallback;
		using value_type = typename Primary::value_type;

		template <typename U>
		using rebind_t =  FallbackAllocator<
			typename Primary::template rebind_t<U>,
			typename Fallback::template rebind_t<U>
		>;

		FallbackAllocator() = default;
		template <typename U>
		FallbackAllocator(const rebind_t<U> & other)
			: Primary{ other }
			, Fallback{ other }
		{}

		Primary & get_primary() { return *this; }
		Fallback & get_fallback() { return *this; }

		value_type * allocate(size_type n = 1)
		{
			if (auto * mem = Primary::allocate(n))	return mem;
			return Fallback::allocate(n);
		}

		void deallocate(value_type * mem, size_type n = 1)
		{
			if (Primary::owns(mem))	
				Primary::deallocate(mem, n);
			else
			{
				MEMORY_ASSERT(Fallback::owns(mem));
				Fallback::deallocate(mem, n);
			}
		}

		bool owns(const value_type * mem) const
		{
			return Primary::owns(mem) || Fallback::owns(mem);
		}

		bool is_full() const { return Primary::is_full() && Fallback::is_full(); }
		size_type free_size() const
		{
			const auto p = Primary::free_size();
			const auto f = Fallback::free_size();
			return p > f ? p : f;
		}

	};
	
	/// \brief	Helper to rebind both input allocators to allocators of T.
	template <typename T, typename Primary, typename Fallback>
	using FallbackAllocatorT = FallbackAllocator<
		typename Primary::template rebind_t<T>,
		typename Fallback::template rebind_t<T>
	>;

}

