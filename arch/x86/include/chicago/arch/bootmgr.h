// File author is Ítalo Lima Marconato Matias
//
// Created on May 27 of 2018, at 13:38 BRT
// Last edited on January 21 of 2019, at 16:32 BRT

#ifndef __CHICAGO_ARCH_BOOTMGR_H__
#define __CHICAGO_ARCH_BOOTMGR_H__

#include <chicago/types.h>

typedef struct {
	UInt32 base;
	UInt32 base_high;
	UInt32 length;
	UInt32 length_high;
	UInt32 type;
	UInt32 acpi;
} Packed BootmgrMemoryMap, *PBootmgrMemoryMap;

extern PChar BootmgrBootDev;
extern UInt32 BootmgrDispBpp;
extern UInt32 BootmgrDispWidth;;
extern UInt32 BootmgrDispHeight;
extern UInt32 BootmgrMemMapCount;
extern UInt32 BootmgrDispPhysAddr;
extern PBootmgrMemoryMap BootmgrMemMap;

extern Boolean CPUIDCheck(Void);

#endif		// __CHICAGO_ARCH_BOOTMGR_H__
