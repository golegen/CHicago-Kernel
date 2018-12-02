// File author is Ítalo Lima Marconato Matias
//
// Created on July 13 of 2018, at 00:45 BRT
// Last edited on September 17 of 2018, at 14:50 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

UIntPtr MemAllocate(UIntPtr size);
UIntPtr MemAAllocate(UIntPtr size, UIntPtr align);
Void MemFree(UIntPtr blockk);
Void MemAFree(UIntPtr block);
UIntPtr MemZAllocate(UIntPtr size);
UIntPtr MemReallocate(UIntPtr blockk, UIntPtr size);

#endif		// __CHICAGO_ALLOC_H__
