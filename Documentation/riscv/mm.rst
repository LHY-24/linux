Complete virtual memory map with 5-level page tables
====================================================
.. note::

 - With 56-bit addresses, user-space memory gets expanded by a factor of 512x,
   from 0.125 PB to 64 PB. All kernel mappings shift down to the -64 PB starting
   offset and many of the regions expand to support the much larger physical
   memory supported.

========================================================================================================================
    Start addr    |   Offset   |     End addr     |  Size   | VM area description
========================================================================================================================
                  |            |                  |         |
 0000000000000000 |    0       | 00ffffffffffffff |  64 PB  | user-space virtual memory, different per mm
__________________|____________|__________________|_________|___________________________________________________________
                  |            |                  |         |
 0100000000000000 | +64    GB  | feffffffffffffff | ~16K PB | ... huge, almost 64 bits wide hole of non-canonical
                  |            |                  |         |     virtual memory addresses up to the -64 PB
                  |            |                  |         |     starting offset of kernel mappings.
__________________|____________|__________________|_________|___________________________________________________________
                                                            |
                                                            | Kernel-space virtual memory, shared between all processes:
____________________________________________________________|___________________________________________________________
                  |            |                  |         |
 ffffffc000000000 | -256    GB | ffffffc7ffffffff |   32 GB | kasan
 ffffffcefee00000 | -196    GB | ffffffcefeffffff |    2 MB | fixmap
 ffffffceff000000 | -196    GB | ffffffceffffffff |   16 MB | PCI io
 ffffffcf00000000 | -196    GB | ffffffcfffffffff |    4 GB | vmemmap
 ffffffd000000000 | -192    GB | ffffffdfffffffff |   64 GB | vmalloc/ioremap space
 ffffffe000000000 | -128    GB | ffffffff7fffffff |  124 GB | direct mapping of all physical memory
__________________|____________|__________________|_________|____________________________________________________________
                                                            |
                                                            |
____________________________________________________________|____________________________________________________________
                  |            |                  |         |
 ffffffff00000000 |   -4    GB | ffffffff7fffffff |    2 GB | modules
 ffffffff80000000 |   -2    GB | ffffffffffffffff |    2 GB | kernel, BPF
__________________|____________|__________________|_________|____________________________________________________________

usr space : 0000000000000000 ~ 00ffffffffffffff
kernel space : ff00000000000000 ~ ffffffffffffffff

Architecture defines a 64-bit virtual address. Implementations can support
less. Currently supported are 48- and 57-bit virtual addresses. Bits 63
through to the most-significant implemented bit are sign extended.
This causes hole between user space and kernel addresses if you interpret them
as unsigned.

// lhy_add