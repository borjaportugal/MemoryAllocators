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
	/// \brief	Stores chunks of memory as a linked list by using part of the memory of them.
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

	/// \brief	Allocates a chunk of memory big enough to hold N objects of size S.
	///			Can only retrieve one object when the user calls to allocate. 
	///			(i.e. Cannot be used to allocate arrays)
	class PageAllocator
	{
	protected:
		/// \brief	One chunk of memory containing multiple Objects (to allocate)
		struct Page { Page * m_next; };
		
	public:
		PageAllocator(size_type obj_size, 
					  size_type obj_num,
					  bool allocate_page = true);
		virtual ~PageAllocator();
		
		virtual void * allocate();
		virtual void deallocate(void * mem);

		size_type get_page_size() const;
		size_type allocated_pages() const;

		size_type get_obj_size() const { return m_object_size; }

	protected:
		/// \brief	Allocates memory for the page.
		virtual Page * do_page_alloc();
		/// \brief	Deallocates the memory of the page.
		virtual void do_page_dealloc(Page * page);
		void deallocate_all_pages();

	private:
		Page * as_page(void * p) { return reinterpret_cast<Page *>(p); }
		void * offset_to_memory(Page * page);

		/// \brief	Allocates a new page and links it.
		void allocate_page();

	private:
		Page * m_pages{ nullptr };
		FreeList m_free_list;

		size_type m_object_num{ 0 };
		size_type m_object_size{ 0 };
	};

#if MEMORY_DEBUG_ENABLED

	/// \brief	Writes patters in the memory.
	class DebugPageAllocator
		: public PageAllocator
	{
		using Base = PageAllocator;

	public:
		DebugPageAllocator(size_type obj_size,
						   size_type obj_num,
						   bool allocate_page = true);
		~DebugPageAllocator();

		void * allocate() override;
		void deallocate(void * ptr) override;

	private:
		Page * do_page_alloc();
		void do_page_dealloc(Page * page);

	};

#endif

}

