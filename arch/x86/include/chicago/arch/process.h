// File author is Ítalo Lima Marconato Matias
//
// Created on July 28 of 2018, at 01:15 BRT
// Last edited on March 03 of 2019, at 10:04 BRT

#ifndef __CHICAGO_ARCH_PROCESS_H__
#define __CHICAGO_ARCH_PROCESS_H__

#include <chicago/arch/registers.h>
#include <chicago/arch/gdt.h>

#include <chicago/arch.h>

#define PS_STACK_SIZE 0x10000
#define ArchSwitchToKernelStack() Asm Volatile("mov %0, %%esp" :: "r"(((UIntPtr)(&KernelStack)) + 0x10000)); GDTSetKernelStack(((UIntPtr)(&KernelStack)) + 0x10000);

typedef struct {
	UInt8 kstack[PS_STACK_SIZE];
	UInt8 fpu_state[512];
	UIntPtr esp;
} Context, *PContext;

#ifndef __CHICAGO_ARCH_PROCESS__
extern UInt8 PsFPUStateSave[512];
extern UInt8 PsFPUDefaultState[512];

Void PsSwitchTaskForce(PRegisters regs);
#endif

#endif		// __CHICAGO_ARCH_PROCESS_H__
