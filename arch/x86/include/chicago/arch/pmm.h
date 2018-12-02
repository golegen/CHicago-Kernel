// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 19:20 BRT
// Last edited on August 11 of 2018, at 18:51 BRT

#ifndef __CHICAGO_ARCH_PMM_H__
#define __CHICAGO_ARCH_PMM_H__

#include <chicago/types.h>

extern PUIntPtr MmPageStack;
extern PUIntPtr MmPageReferences;
extern IntPtr MmPageStackPointer;
extern UIntPtr MmMaxPages;
extern UIntPtr MmUsedPages;

extern UIntPtr KernelStart;
extern UIntPtr KernelEnd;

#ifndef __CHICAGO_PMM__
extern UIntPtr MmBootAllocPointer;
extern UIntPtr KernelRealEnd;
#endif

Void PMMInit(Void);

#endif		// __CHICAGO_ARCH_PMM_H__
