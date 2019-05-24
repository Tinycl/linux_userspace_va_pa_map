# linux_userspace_va_pa_map
https://www.kernel.org/doc/html/latest/admin-guide/mm/pagemap.html
user space get the pa from pa 
pagemap is a new (as of 2.6.25) set of interfaces in the kernel that allow userspace programs to examine the page tables and related information by reading files in /proc.

There are four components to pagemap:

        /proc/pid/pagemap. This file lets a userspace process find out which physical frame each virtual page is mapped to. It contains one 64-bit value for each virtual page, containing the following data (from fs/proc/task_mmu.c, above pagemap_read):

                Bits 0-54 page frame number (PFN) if present
                Bits 0-4 swap type if swapped
                Bits 5-54 swap offset if swapped
                Bit 55 pte is soft-dirty (see Documentation/admin-guide/mm/soft-dirty.rst)
                Bit 56 page exclusively mapped (since 4.2)
                Bits 57-60 zero
                Bit 61 page is file-page or shared-anon (since 3.5)
                Bit 62 page swapped
                Bit 63 page present

        Since Linux 4.0 only users with the CAP_SYS_ADMIN capability can get PFNs. In 4.0 and 4.1 opens by unprivileged fail with -EPERM. Starting from 4.2 the PFN field is zeroed if the user does not have CAP_SYS_ADMIN. Reason: information about PFNs helps in exploiting Rowhammer vulnerability.

        If the page is not present but in swap, then the PFN contains an encoding of the swap file number and the page’s offset into the swap. Unmapped pages return a null PFN. This allows determining precisely which pages are mapped (or in swap) and comparing mapped pages between processes.

        Efficient users of this interface will use /proc/pid/maps to determine which areas of memory are actually mapped and llseek to skip over unmapped regions.

        /proc/kpagecount. This file contains a 64-bit count of the number of times each page is mapped, indexed by PFN.

The page-types tool in the tools/vm directory can be used to query the number of times a page is mapped.

        /proc/kpageflags. This file contains a 64-bit set of flags for each page, indexed by PFN.

        The flags are (from fs/proc/page.c, above kpageflags_read):

                LOCKED
                ERROR
                REFERENCED
                UPTODATE
                DIRTY
                LRU
                ACTIVE
                SLAB
                WRITEBACK
                RECLAIM
                BUDDY
                MMAP
                ANON
                SWAPCACHE
                SWAPBACKED
                COMPOUND_HEAD
                COMPOUND_TAIL
                HUGE
                UNEVICTABLE
                HWPOISON
                NOPAGE
                KSM
                THP
                OFFLINE
                ZERO_PAGE
                IDLE
                PGTABLE

        /proc/kpagecgroup. This file contains a 64-bit inode number of the memory cgroup each page is charged to, indexed by PFN. Only available when CONFIG_MEMCG is set.


