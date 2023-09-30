# PseudoMalloc
Project for the "Operating System" exam (2022-2023), part of the Computer Science and Automation Engineering program at Sapienza University.

The project involves the creation of a set of functions that replace the malloc. 
The allocation process depends on the requested size:

   - for small requests (< 1/4 of the page size) it uses a buddy allocator, implemented with a bitmap
     that manages 1 MB of memory for these small allocations.

   - for large request (>=1/4 of the page size) uses the mmap function
