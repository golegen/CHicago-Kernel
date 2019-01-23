// File author is Ítalo Lima Marconato Matias
//
// Created on June 28 of 2018, at 19:26 BRT
// Last edited on November 17 of 2018, at 11:43 BRT

#ifndef __CHICAGO_ARCH_VMM_H__
#define __CHICAGO_ARCH_VMM_H__

#include <chicago/types.h>

#define MmGetPDEInt(pd, i) ((PUInt32)(pd))[((i) & ~0xFFF) >> 22]
#define MmSetPDEInt(pd, i, p, f) ((PUInt32)(pd))[((i) & ~0xFFF) >> 22] = ((p) & ~0xFFF) | ((f) & 0xFFF)

#define MmGetPTEInt(pt, i) ((PUInt32)((pt) + ((((i) & ~0xFFF) >> 22) * 0x1000)))[(((i) & ~0xFFF) << 10) >> 22]
#define MmSetPTEInt(pt, i, p, f) ((PUInt32)((pt) + ((((i) & ~0xFFF) >> 22) * 0x1000)))[(((i) & ~0xFFF) << 10) >> 22] = ((p) & ~0xFFF) | ((f) & 0xFFF)

#define MmGetPDE(i) MmGetPDEInt(0xFFFFF000, i)
#define MmSetPDE(i, p, f) MmSetPDEInt(0xFFFFF000, i, p, f)

#define MmGetPTE(i) MmGetPTEInt(0xFFC00000, i)
#define MmSetPTE(i, p, f) MmSetPTEInt(0xFFC00000, i, p, f)

#define MmInvlpg(i) Asm Volatile("invlpg %0" :: "m"(*((PChar)((i) & ~0xFFF))))

#endif		// __CHICAGO_ARCH_VMM_H__
