
#include "PageAllocator.h"

namespace memory
{
	inline void * FreeList::extract()
	{
		auto * curr = m_head;
		m_head = m_head->m_next;
		return reinterpret_cast<void *>(curr);
	}
	void FreeList::insert(void * mem)
	{
		auto * obj = reinterpret_cast<Object *>(mem);
		if (m_head)
		{
			obj->m_next = m_head;
			m_head = obj;
		}
		else
		{
			m_head = obj;
			m_head->m_next = nullptr;
		}
	}
	void FreeList::insert(void * mem_start, size_type object_size, size_type object_num)
	{
		unsigned char * raw = reinterpret_cast<unsigned char *>(mem_start);
		for (size_type i = 0; i < object_num; ++i)
		{
			insert(raw);
			raw += object_size;
		}
	}
	inline bool FreeList::empty() const
	{
		return m_head == nullptr;
	}

	PageAllocator::PageAllocator(size_type obj_size, 
								 size_type obj_num,
								 bool allocate_first_page)
		: m_object_num{ obj_num }
		// we need to be able to link the memory chunks
		, m_object_size{ obj_size < m_free_list.min_size() ? m_free_list.min_size() : obj_size }
	{
		if (allocate_first_page)
			allocate_page();
	}
	PageAllocator::~PageAllocator()
	{
		deallocate_all_pages();
	}

	inline void * PageAllocator::offset_to_memory(Page * page)
	{
		return reinterpret_cast<void *>(page + 1);
	}
	inline size_type PageAllocator::get_page_size() const
	{
		return m_object_num * m_object_size + sizeof(Page*);
	}

	PageAllocator::Page * PageAllocator::do_page_alloc()
	{
		return as_page(global_alloc(get_page_size()));
	}
	void PageAllocator::do_page_dealloc(Page * page)
	{
		global_dealloc(page);
	}

	void PageAllocator::allocate_page()
	{
		Page * new_page = do_page_alloc();
		new_page->m_next = m_pages;
		m_pages = new_page;

		// STUDY(Borja): we could track the number of free objects we have in the current page and in that way we could avoid this O(N) operation.
		// add all the objects to the free list
		m_free_list.insert(offset_to_memory(new_page), m_object_size, m_object_num);
	}
	void PageAllocator::deallocate_all_pages()
	{
		while (m_pages)
		{
			auto * next = m_pages->m_next;
			do_page_dealloc(m_pages);
			m_pages = next;
		}
	}

	void * PageAllocator::allocate()
	{
		if (m_free_list.empty())	allocate_page();

		return m_free_list.extract();
	}
	void PageAllocator::deallocate(void * mem)
	{
		m_free_list.insert(mem);
	}

	size_type PageAllocator::allocated_pages() const
	{
		size_type page_num = 0;
		for (auto * curr = m_pages; curr != nullptr; curr = curr->m_next)
		{
			page_num++;
		}

		return page_num;
	}

#if MEMORY_DEBUG_ENABLED

	DebugPageAllocator::DebugPageAllocator(size_type obj_size,
					   size_type obj_num,
					   bool allocate_page)
		: Base{ obj_size, obj_num, allocate_page }
	{}
	DebugPageAllocator::~DebugPageAllocator()
	{
		deallocate_all_pages();
	}
	void * DebugPageAllocator::allocate()
	{
		auto * mem = Base::allocate();
		fill_with_pattern(DebugPattern::ALLOCATED, mem, get_obj_size());
		return mem;
	}
	void DebugPageAllocator::deallocate(void * ptr)
	{
		fill_with_pattern(DebugPattern::DEALLOCATED, ptr, get_obj_size());
		Base::deallocate(ptr);
	}

	DebugPageAllocator::Page * DebugPageAllocator::do_page_alloc()
	{
		auto * page = Base::do_page_alloc();
		fill_with_pattern(DebugPattern::ACQUIRED, page, get_page_size());
		return page;
	}
	void DebugPageAllocator::do_page_dealloc(Page * page)
	{
		// if we call delete on the memory of the page, the runtime library may put its own
		// pattern, just in case it does not (i.e. release build)
		fill_with_pattern(DebugPattern::RELEASED, page, get_page_size());
		Base::do_page_dealloc(page);
	}
#endif
}

