// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 18:42 BRT
// Last edited on July 21 of 2018, at 18:52 BRT

#include <chicago/mm.h>

PUIntPtr MmPageStack = Null;
PUIntPtr MmPageReferences = Null;
IntPtr MmPageStackPointer = -1;
UIntPtr MmMaxPages = 0;
UIntPtr MmUsedPages = 0;

UIntPtr MmAllocPage(Void) {
	if ((MmPageStack == Null) || (MmPageStackPointer == -1)) {															// Sanity checks
		return 0;
	}
	
	MmUsedPages++;																										// Increase the used pages count
	
	return MmPageStack[MmPageStackPointer--];																			// It's just an stack, pop the top element
}

UIntPtr MmReferencePage(UIntPtr addr) {
	if (addr == 0) {																									// Alloc?
		UIntPtr ret = MmAllocPage();																					// Yes!
		
		if (ret == 0) {																									// Failed?
			return 0;																									// Yes...
		}
		
		return MmReferencePage(ret);																					// Return with an recursive call!
	} else if (addr >= (MmMaxPages * MM_PAGE_SIZE)) {																	// Sanity check
		return 0;
	} else if ((addr % MM_PAGE_SIZE) != 0) {																			// Page aligned?
		addr -= addr % MM_PAGE_SIZE;																					// No, so let's align it!
	}
	
	MmPageReferences[addr / MM_PAGE_SIZE]++;
	return addr;
}

Void MmFreePage(UIntPtr addr) {
	if ((MmPageStack == Null) || (addr == 0) || (addr >= (MmMaxPages * MM_PAGE_SIZE)) || (MmUsedPages == 0)) {			// Again, sanity checks
		return;
	}
	
	MmUsedPages--;																										// This time decrease the used pages count
	MmPageStack[++MmPageStackPointer] = addr;																			// And push this address to the top of the stack!
}

Void MmDereferencePage(UIntPtr addr) {
	if ((addr == 0) || (addr >= (MmMaxPages * MM_PAGE_SIZE))) {															// Sanity check...
		return;
	} else if ((addr % MM_PAGE_SIZE) != 0) {																			// Page aligned?
		addr -= addr % MM_PAGE_SIZE;																					// No, so let's align it!
	}
	
	if (MmPageReferences[addr / MM_PAGE_SIZE] == 1) {																	// Only one reference?
		MmPageReferences[addr / MM_PAGE_SIZE]--;																		// Yes, so we can free it
		MmFreePage(addr);
	} else if (MmPageReferences[addr / MM_PAGE_SIZE] != 0) {															// Only decrement the reference count if it's bigger than 0
		MmPageReferences[addr / MM_PAGE_SIZE]--;
	}
}

UIntPtr MmGetReferences(UIntPtr addr) {
	if ((addr == 0) || (addr >= (MmMaxPages * MM_PAGE_SIZE))) {															// Sanity check...
		return 0;
	} else if ((addr % MM_PAGE_SIZE) != 0) {																			// Page aligned?
		addr -= addr % MM_PAGE_SIZE;																					// No, so let's align it!
	}
	
	return MmPageReferences[addr / MM_PAGE_SIZE];
}

UIntPtr MmGetSize(Void) {
	return MmMaxPages * MM_PAGE_SIZE;
}

UIntPtr MmGetUsage(Void) {
	return MmUsedPages * MM_PAGE_SIZE;
}

UIntPtr MmGetFree(Void) {
	return (MmMaxPages - MmUsedPages) * MM_PAGE_SIZE;
}
