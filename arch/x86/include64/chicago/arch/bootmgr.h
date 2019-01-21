// File author is Ítalo Lima Marconato Matias
//
// Created on May 27 of 2018, at 13:38 BRT
// Last edited on January 19 of 2019, at 22:18 BRT

#ifndef __CHICAGO_ARCH_BOOTMGR_H__
#define __CHICAGO_ARCH_BOOTMGR_H__

#include <chicago/types.h>

typedef struct {
	UInt64 base;
	UInt64 length;
	UInt32 type;
	UInt32 acpi;
} Packed BootmgrMemoryMap, *PBootmgrMemoryMap;

extern PChar BootmgrBootDev;
extern UInt64 BootmgrDispBpp;
extern UInt64 BootmgrDispWidth;
extern UInt64 BootmgrDispHeight;
extern UInt64 BootmgrMemMapCount;
extern UInt64 BootmgrDispPhysAddr;
extern PBootmgrMemoryMap BootmgrMemMap;

#endif		// __CHICAGO_ARCH_BOOTMGR_H__
