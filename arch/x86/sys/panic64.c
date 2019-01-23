// File author is Ítalo Lima Marconato Matias
//
// Created on October 27 of 2018, at 21:48 BRT
// Last edited on January 21 of 2019, at 23:17 BRT

#include <chicago/arch/registers.h>

#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/display.h>
#include <chicago/nls.h>
#include <chicago/panic.h>
#include <chicago/process.h>

Void ArchPanicWriteHex(UInt64 val) {
	if (val < 0x10) {
		ConWriteFormated(L"0x000000000000000%x", val);
	} else if (val < 0x100) {
		ConWriteFormated(L"0x00000000000000%x", val);
	} else if (val < 0x1000) {
		ConWriteFormated(L"0x0000000000000%x", val);
	} else if (val < 0x10000) {
		ConWriteFormated(L"0x000000000000%x", val);
	} else if (val < 0x100000) {
		ConWriteFormated(L"0x00000000000%x", val);
	} else if (val < 0x1000000) {
		ConWriteFormated(L"0x0000000000%x", val);
	} else if (val < 0x10000000) {
		ConWriteFormated(L"0x000000000%x", val);
	} else if (val < 0x100000000) {
		ConWriteFormated(L"0x00000000%x", val);
	} else if (val < 0x1000000000) {
		ConWriteFormated(L"0x0000000%x", val);
	} else if (val < 0x10000000000) {
		ConWriteFormated(L"0x000000%x", val);
	} else if (val < 0x100000000000) {
		ConWriteFormated(L"0x00000%x", val);
	} else if (val < 0x1000000000000) {
		ConWriteFormated(L"0x0000%x", val);
	} else if (val < 0x10000000000000) {
		ConWriteFormated(L"0x000%x", val);
	} else if (val < 0x100000000000000) {
		ConWriteFormated(L"0x00%x", val);
	} else if (val < 0x1000000000000000) {
		ConWriteFormated(L"0x0%x", val);
	} else {
		ConWriteFormated(L"0x%x", val);
	}
}

Void ArchPanic(UInt32 err, PVoid priv) {
	if (PsCurrentThread != Null) {																				// Tasking initialized?
		if (!((PsCurrentThread->id == 0) && (PsCurrentProcess->id == 0))) {										// Yes, this is the main kernel process?
			ConAcquireLock();																					// Nope, we don't want a dead lock, right?
			ConWriteFormated(L"\r\n%s", NlsGetMessage(NLS_SEGFAULT));											// Print the error to the screen
			PsExitProcess(1);																					// And just PsExitProcess()
		}
	}
	
	PsLockTaskSwitch(old);																						// Lock
	ConAcquireLock();																							// We don't want a dead lock, right?
	ConSetRefresh(False);																						// Disable the automatic screen refresh
	PanicInt(err, False);																						// Print the "Sorry" message
	
	UInt64 cr2 = 0;																								// Get the CR2
	Asm Volatile("mov %%cr2, %0" : "=r"(cr2));
	
	PRegisters regs = (PRegisters)priv;																			// Cast the priv into the PRegisters struct
	
	ConWriteFormated(L"| RAX: "); ArchPanicWriteHex(regs->rax); ConWriteFormated(L" | ");						// Print the registers
	ConWriteFormated(L"RBX: "); ArchPanicWriteHex(regs->rbx); ConWriteFormated(L" | ");
	ConWriteFormated(L"RCX:    "); ArchPanicWriteHex(regs->rcx); ConWriteFormated(L" | ");
	ConWriteFormated(L"RDX: "); ArchPanicWriteHex(regs->rdx); ConWriteFormated(L" |\r\n");
	
	ConWriteFormated(L"| RSI: "); ArchPanicWriteHex(regs->rsi); ConWriteFormated(L" | ");
	ConWriteFormated(L"RDI: "); ArchPanicWriteHex(regs->rdi); ConWriteFormated(L" | ");
	ConWriteFormated(L"RSP:    "); ArchPanicWriteHex(regs->rsp); ConWriteFormated(L" | ");
	ConWriteFormated(L"RBP: "); ArchPanicWriteHex(regs->rbp); ConWriteFormated(L" |\r\n");
	
	ConWriteFormated(L"| R8:  "); ArchPanicWriteHex(regs->r8); ConWriteFormated(L" | ");
	ConWriteFormated(L"R9:  "); ArchPanicWriteHex(regs->r9); ConWriteFormated(L" | ");
	ConWriteFormated(L"R10:    "); ArchPanicWriteHex(regs->r10); ConWriteFormated(L" | ");
	ConWriteFormated(L"R11: "); ArchPanicWriteHex(regs->r11); ConWriteFormated(L" |\r\n");
	
	ConWriteFormated(L"| R12: "); ArchPanicWriteHex(regs->r12); ConWriteFormated(L" | ");
	ConWriteFormated(L"R13: "); ArchPanicWriteHex(regs->r13); ConWriteFormated(L" | ");
	ConWriteFormated(L"R14:    "); ArchPanicWriteHex(regs->r14); ConWriteFormated(L" | ");
	ConWriteFormated(L"R15: "); ArchPanicWriteHex(regs->r15); ConWriteFormated(L" |\r\n");
	
	ConWriteFormated(L"| RIP: "); ArchPanicWriteHex(regs->rip); ConWriteFormated(L" | ");
	ConWriteFormated(L"CR2: "); ArchPanicWriteHex(cr2); ConWriteFormated(L" | ");
	ConWriteFormated(L"RFLAGS: "); ArchPanicWriteHex(regs->rflags); ConWriteFormated(L" | ");
	ConWriteFormated(L"                        |\r\n");
	
	ConWriteFormated(L"| CS:  "); ArchPanicWriteHex((UInt8)regs->cs); ConWriteFormated(L" | ");
	ConWriteFormated(L"DS:  "); ArchPanicWriteHex((UInt8)regs->ds); ConWriteFormated(L" | ");
	ConWriteFormated(L"ES:     "); ArchPanicWriteHex((UInt8)regs->es); ConWriteFormated(L" | ");
	ConWriteFormated(L"FS:  "); ArchPanicWriteHex((UInt8)regs->ds); ConWriteFormated(L" |\r\n");
	
	ConWriteFormated(L"| GS:  "); ArchPanicWriteHex((UInt8)regs->ds); ConWriteFormated(L" | ");
	ConWriteFormated(L"SS:  "); ArchPanicWriteHex((UInt8)regs->ss); ConWriteFormated(L" | ");
	ConWriteFormated(L"                           | ");
	ConWriteFormated(L"                        |\r\n");
	
	PanicInt(err, True);																						// Print the error code
	DispRefresh();																								// Refresh the screen
	ArchHalt();																									// Halt
}
