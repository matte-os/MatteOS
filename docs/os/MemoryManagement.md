# Memory Management

In this document, we will discuss our memory management strategy.

## Memory after boot

After boot, the kernel will use static addresses to allocate the first memory region.
The size of the region and its address are defined in the linker script. 
Later, however, I want to make some abstraction that will allow the kernel to allocate
the rest of the memory from available regions given by the FTD.
