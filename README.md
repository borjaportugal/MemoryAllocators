(Project still in development)

This project is licensed under the terms of the [MIT license](./LICENSE.md).

# MemoryAllocators
This is a simple exercise I wanted to make to play around with different allocator implementations as well as debug functionality for  them.
The allocators are varied, some of them allocate raw memory while others are implemented to allocate space for specific object types and have the type of the object embedded in their own type.

In case of using these allocators in a bigger project we would probably want to implement the variation of the allocation and deallocation functions for aligned memory.

## Implemented allocators
### GlobalAllocator<T>
Raw allocator used when all other allocators fail allocating memory. 
Requests memory to the OS but it could use any other method to manage the memory (like a global Arena that contains all the available memory).

### DebugGlobalAllocator<T>
Fills the memory with debug patterns for debug purposes, to detect faster bad pointer accesses.


### InlineAllocator<N, T>
Allocator meant to be created in the stack, it will allocate enough contiguous memory to hold N objects of type T.
Once all the objects have been allocated will fail to allocate more.

### DefaultInlineAllocator<N, T>
This is a very convenient alias for an InlineAllocator that will use the GlobalAllocator to allocate memory in case no more inlined memory is available.

### DebugInlineAllocator<N, T>
Extends the functionality of the DefaultInlineAllocator and generates some statistics of the allocations done. 
Inline allocators are a very useful tool to optimize code that may need from some dynamic memory, one of the biggest problems with them is the number of objects we inline. We don't want to allocate too many objects but at the same time we want to avoid as much dynamic allocations as we can. The statistics that this allocator generates are meant to see how many allocations fall into the inlined memory and how many need from dynamic memory.


### StackAllocator
Allocates the requested number of bytes as contiguous memory and then retrieves memory to the user by moving a pointer back and forth.
Deallocations need to happen in the exact opposite order to allocations.

### DebugStackAllocator
Fills the memory with debug patterns and generates statistics of the allocations.


### FallbackAllocator<Primary, Fallback>
This allocator wraps two allocator types, when memory is requested it first tries to allocate it using Primary allocator and if this fails uses the Fallback allocator.
[Inspired by Andrei Alexandrescu](https://youtu.be/LIb3L4vKZ7U?t=28m14s)

### PageAllocator
The PageAllocator allocates pages storing N objects of S size, then, returns on object per allocation. This means that the first allocation is going to be expensive but the rest are going to be fast. 
This allocator uses a free list internally to keep track of the memory that has been freed.

### DebugPageAllocator
Extension of the PageAllocator that writes patterns in the memory and gives the possibility to add padding to the allocations to make sure the user does not write to memory outside the one that has allocated.



