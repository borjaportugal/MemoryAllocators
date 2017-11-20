# MemoryAllocators
Implementation of multiple memory allocators, debug functionality and support for the stl containers.
Some of the allocators allocate raw memory while others are thought to allocate specific object types.
(Still in development)

## Implemented allocators
### GlobalAllocator<T>
Raw allocator used when all other allocators fail allocating memory. 
Requests memory to the OS but it could use any other method to manage the memory (like a global Arena that contains all the available memory).

#### DebugGlobalAllocator<T>
Fills the memory with debug patterns for debug purposes, to detect faster bad pointer accesses.


### InlineAllocator<N, T>
Allocator meant to be created in the stack, it will allocate enough contiguous memory to hold N objects of type T.
Once all the objects have been allocated will fail to allocate more.

#### DefaultInlineAllocator<N, T>
This is a very convenient alias for an InlineAllocator that will use the GlobalAllocator to allocate memory in case no more inlined memory is available.

#### DebugInlineAllocator<N, T>
Extends the funtionality of the DefaultInlineAllocator and generates some statistics of the allocations done. 
Inline allocators are a very usefull tool to optimize code that may need from some dynamic memory, one of the biggest problems with them is the number of objects we inline. We don't want to allocate too many objects but at the same time we want to avoid as much dynamic allocations as we can. The statistics that this allocator generates are meant to see how many allocations fall into the inlined memory and how many need from dynamic memory.


### StackAllocator
Allocates the requested number of bytes as contiguous memory and then retrieves memory to the user by moving a pointer back and forth.
Deallocations need to happen in the exact oposite order to allocations.

#### DebugStackAllocator
Fills the memory with debug patterns and generates statistics of the allocations.


### FallbackAllocator<Primary, Fallback>
This allocator wraps two allocator types, when memory is requested it first tries to allocate it using Primary allocator and if this fails uses the Fallback allocator.
[Inspired by Andrei Alexandrescu](https://youtu.be/LIb3L4vKZ7U?t=28m14s)


## Pending to implement
### Arena
### Pool
### PageAllocator (with free list)


