// File author is Ítalo Lima Marconato Matias
//
// Created on May 31 of 2018, at 18:45 BRT
// Last edited on February 28 of 2019, at 18:43 BRT

#define __CHICAGO_PMM__

#include <chicago/arch/bootmgr.h>
#include <chicago/arch/pmm.h>

#include <chicago/debug.h>
#include <chicago/mm.h>
#include <chicago/string.h>

UIntPtr MmBootAllocPointer = (UIntPtr)&KernelEnd;
UIntPtr KernelRealEnd = 0;

UIntPtr MmBootAlloc(UIntPtr size, Boolean align) {
	if (MmBootAllocPointer == 0) {																						// Disabled?
		return 0;																										// Yup
	} else if (align && ((MmBootAllocPointer % MM_PAGE_SIZE) != 0)) {													// Align to page size?
		MmBootAllocPointer += MM_PAGE_SIZE - (MmBootAllocPointer % MM_PAGE_SIZE);										// Yes
	}
	
	MmBootAllocPointer += size;																							// Increase the ptr
	
	return MmBootAllocPointer - size;																					// Return the old one
}

UIntPtr PMMCountMemory(Void) {
	PBootmgrMemoryMap mmap = BootmgrMemMap;																				// Here we're going to use the memory map for getting the memory size
	UIntPtr mmapi = 0;
	UIntPtr memsize = 0;
	
	while (mmapi < BootmgrMemMapCount) {
		if (mmap->type > 4) {																							// Valid?
			mmap->type = 2;																								// Nope, so let's set as reserved
		} else if ((mmapi > 0) && (mmap->base == 0)) {																	// End (before expected)?
			break;
		} else {
			if (mmap->type == 1) {																						// Add to memsize?
				memsize += mmap->length;																				// Yes
			}
		}
		
		mmap++;
		mmapi++;
	}
	
	return memsize;
}

Void PMMInit(Void) {
	MmMaxPages = PMMCountMemory() / MM_PAGE_SIZE;																		// Get memory size based on the memory map entries
	MmUsedPages = MmMaxPages;																							// We're going to free the avaliable pages later
	MmPageMap = (PUIntPtr)MmBootAlloc(0x80000, False);																	// Alloc the page frame allocator stack using the initial boot allocator
	MmPageReferences = (PUIntPtr)MmBootAlloc(0x400000, False);															// Also alloc the page frame reference map
	MmMaxIndex = 0x20000;																								// Set the max index for the MmAllocPage function
	KernelRealEnd = MmBootAllocPointer;																					// Setup the KernelRealEnd variable
	MmBootAllocPointer = 0;																								// Break/disable the MmBootAlloc, now we should use MemAllocate/AAllocate/Reallocate/ZAllocate/Free/AFree
	
	StrSetMemory((PUInt8)MmPageMap, 0xFF, 0x80000);																		// We're going to free the avaliable pages later
	
	if ((KernelRealEnd % MM_PAGE_SIZE) != 0) {																			// Align the KernelRealEnd variable to page size
		KernelRealEnd += MM_PAGE_SIZE - (KernelRealEnd % MM_PAGE_SIZE);
	}
	
	PBootmgrMemoryMap mmap = BootmgrMemMap;
	UIntPtr mmapi = 0;
	UIntPtr kstart = ((UIntPtr)(&KernelStart)) - 0xC0000000;
	UIntPtr kend = KernelRealEnd - 0xC0000000;
	
	while (mmapi < BootmgrMemMapCount) {
		if (mmap->type > 4) {																							// Valid?
			mmap->type = 2;																								// Nope, so let's set as reserved
		} else if ((mmapi > 0) && (mmap->base == 0)) {																	// End (before expected)?
			break;
		} else {
			if (mmap->type == 1) {																						// Avaliable for use?
				for (UIntPtr i = 0; i < mmap->length; i += MM_PAGE_SIZE) {												// YES!
					UIntPtr addr = mmap->base + i;
					
					if ((addr != 0) && (!((addr >= kstart) && (addr < kend)))) {										// Just check if the addr isn't 0 nor it's the kernel physical address
						MmFreePage(addr);																				// Everything is ok, SO FREE IT!
					}
				}
			}
		}
		
		mmap++;
		mmapi++;
	}
}
