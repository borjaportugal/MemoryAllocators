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
	class FreeList
	{
		constexpr static size_type MIN_SIZE = sizeof(void*);
		struct Object { Object * m_next; };
	public:
		
		static size_type min_size() { return MIN_SIZE; }

		void * extract();
		void insert(void * mem);
		void insert(void * mem_start, size_type object_size, size_type object_num);

		bool empty() const;
	private:
		Object * m_head{ nullptr };

	};

	class PageAllocator
	{
		// signatures to link the memory
		/// \brief	One chunk of memory containing multiple Objects (to allocate)
		struct Page { Page * m_next; };

		Page * as_page(void * p) { return reinterpret_cast<Page *>(p); }

	public:
		PageAllocator(size_type obj_size, 
					  size_type obj_num,
					  bool allocate_page = true);
		~PageAllocator();
		
		void * allocate();
		void deallocate(void * mem);

		size_type get_page_size() const;
		size_type allocated_pages() const;

	private:
		void * offset_to_memory(Page * page);

		Page * do_page_alloc();
		void do_page_dealloc(Page * page);
		void allocate_page();
		void deallocate_all_pages();

	private:
		Page * m_pages{ nullptr };
		FreeList m_free_list;

		size_type m_object_num{ 0 };
		size_type m_object_size{ 0 };
	};



}

