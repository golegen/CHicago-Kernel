// File author is Ítalo Lima Marconato Matias
//
// Created on June 29 of 2018, at 22:31 BRT
// Last edited on June 29 of 2018, at 22:32 BRT

#ifndef __CHICAGO_HEAP_H__
#define __CHICAGO_HEAP_H__

#include <chicago/types.h>

UIntPtr HeapGetCurrent(Void);
UIntPtr HeapGetStart(Void);
UIntPtr HeapGetEnd(Void);
Boolean HeapIncrement(UIntPtr amount);
Boolean HeapDecrement(UIntPtr amount);
Void HeapInit(UIntPtr start, UIntPtr end);

#endif		// __CHICAGO_HEAP_H__
