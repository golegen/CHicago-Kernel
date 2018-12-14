// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 18:42 BRT
// Last edited on December 14 of 2018, at 13:24 BRT

#include <chicago/mm.h>

PUIntPtr MmPageMap = Null;
PUIntPtr MmPageReferences = Null;
UIntPtr MmMaxPages = 0;
UIntPtr MmUsedPages = 0;

UIntPtr MmAllocPage(Void) {
	if (MmPageMap == Null) {																													// Sanity check
		return 0;
	}
	
	for (UIntPtr i = 0; i < MmMaxPages / sizeof(UIntPtr); i++) {																				// Let's search for a free page!
		if (MmPageMap[i] != UINTPTR_MAX) {																										// All the bits are allocated?
			for (UIntPtr j = 0; j < (sizeof(UIntPtr) * 8); j++) {																				// No! Let's find the free bit!
				if ((!((i == 0) && (j == 0))) && ((MmPageMap[i] & (1 << j)) == 0)) {															// Is this one?
					MmPageMap[i] |= (1 << j);																									// Set this page as used!
					return ((i * (sizeof(UIntPtr) * 8)) + j) * MM_PAGE_SIZE;																	// And return it!
				}
			}
		}
	}
	
	return 0;																																	// No more free pages :(
}

UIntPtr MmReferencePage(UIntPtr addr) {
	if (addr == 0) {																															// Alloc?
		UIntPtr ret = MmAllocPage();																											// Yes!
		
		if (ret == 0) {																															// Failed?
			return 0;																															// Yes...
		}
		
		return MmReferencePage(ret);																											// Return with an recursive call!
	} else if (addr >= (MmMaxPages * MM_PAGE_SIZE)) {																							// Sanity check
		return 0;
	} else if ((addr % MM_PAGE_SIZE) != 0) {																									// Page aligned?
		addr -= addr % MM_PAGE_SIZE;																											// No, so let's align it!
	}
	
	MmPageReferences[addr / MM_PAGE_SIZE]++;
	return addr;
}

Void MmFreePage(UIntPtr addr) {
	if ((MmPageMap == Null) || (addr == 0) || (addr >= (MmMaxPages * MM_PAGE_SIZE)) || (MmUsedPages == 0)) {									// Again, sanity checks
		return;
	}
	
	MmPageMap[addr / (MM_PAGE_SIZE * sizeof(UIntPtr) * 8)] &= ~(1 << ((addr % (MM_PAGE_SIZE * sizeof(UIntPtr) * 8)) / MM_PAGE_SIZE));			// This time, just unset the right bit!
}

Void MmDereferencePage(UIntPtr addr) {
	if ((addr == 0) || (addr >= (MmMaxPages * MM_PAGE_SIZE))) {																					// Sanity check...
		return;
	} else if ((addr % MM_PAGE_SIZE) != 0) {																									// Page aligned?
		addr -= addr % MM_PAGE_SIZE;																											// No, so let's align it!
	}
	
	if (MmPageReferences[addr / MM_PAGE_SIZE] == 1) {																							// Only one reference?
		MmPageReferences[addr / MM_PAGE_SIZE]--;																								// Yes, so we can free it
		MmFreePage(addr);
	} else if (MmPageReferences[addr / MM_PAGE_SIZE] != 0) {																					// Only decrement the reference count if it's bigger than 0
		MmPageReferences[addr / MM_PAGE_SIZE]--;
	}
}

UIntPtr MmGetReferences(UIntPtr addr) {
	if ((addr == 0) || (addr >= (MmMaxPages * MM_PAGE_SIZE))) {																					// Sanity check...
		return 0;
	} else if ((addr % MM_PAGE_SIZE) != 0) {																									// Page aligned?
		addr -= addr % MM_PAGE_SIZE;																											// No, so let's align it!
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
