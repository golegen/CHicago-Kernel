// File author is Ítalo Lima Marconato Matias
//
// Created on October 27 of 2018, at 21:48 BRT
// Last edited on November 15 of 2018, at 16:04 BRT

#include <chicago/arch/registers.h>

#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/display.h>
#include <chicago/panic.h>
#include <chicago/process.h>

Void ArchPanicWriteHex(UInt32 val) {
	if (val < 0x10) {
		ConWriteFormated("0x0000000%x", val);
	} else if (val < 0x100) {
		ConWriteFormated("0x000000%x", val);
	} else if (val < 0x1000) {
		ConWriteFormated("0x00000%x", val);
	} else if (val < 0x10000) {
		ConWriteFormated("0x0000%x", val);
	} else if (val < 0x100000) {
		ConWriteFormated("0x000%x", val);
	} else if (val < 0x1000000) {
		ConWriteFormated("0x00%x", val);
	} else if (val < 0x10000000) {
		ConWriteFormated("0x0%x", val);
	} else {
		ConWriteFormated("0x%x", val);
	}
}

Void ArchPanic(UInt32 err, PVoid priv) {
	PsLockTaskSwitch(old);																						// Lock
	ConSetRefresh(False);																						// Disable the automatic screen refresh
	PanicInt(err, False);																						// Print the "Sorry" message
	
	UInt32 cr2 = 0;																								// Get the CR2
	Asm Volatile("mov %%cr2, %0" : "=r"(cr2));
	
	PRegisters regs = (PRegisters)priv;																			// Cast the priv into the PRegisters struct
	
	ConWriteFormated("| EAX: "); ArchPanicWriteHex(regs->eax); ConWriteFormated(" | ");							// Print the registers
	ConWriteFormated("EBX: "); ArchPanicWriteHex(regs->ebx); ConWriteFormated(" | ");
	ConWriteFormated("ECX:    "); ArchPanicWriteHex(regs->ecx); ConWriteFormated(" | ");
	ConWriteFormated("EDX: "); ArchPanicWriteHex(regs->edx); ConWriteFormated(" |\r\n");
	
	ConWriteFormated("| ESI: "); ArchPanicWriteHex(regs->esi); ConWriteFormated(" | ");
	ConWriteFormated("EDI: "); ArchPanicWriteHex(regs->edi); ConWriteFormated(" | ");
	ConWriteFormated("ESP:    "); ArchPanicWriteHex(regs->esp); ConWriteFormated(" | ");
	ConWriteFormated("EBP: "); ArchPanicWriteHex(regs->ebp); ConWriteFormated(" |\r\n");
	
	ConWriteFormated("| EIP: "); ArchPanicWriteHex(regs->eip); ConWriteFormated(" | ");
	ConWriteFormated("CR2: "); ArchPanicWriteHex(cr2); ConWriteFormated(" | ");
	ConWriteFormated("EFLAGS: "); ArchPanicWriteHex(regs->eflags); ConWriteFormated(" | ");
	ConWriteFormated("                |\r\n");
	
	ConWriteFormated("| CS:  "); ArchPanicWriteHex((UInt8)regs->cs); ConWriteFormated(" | ");
	ConWriteFormated("DS:  "); ArchPanicWriteHex((UInt8)regs->ds); ConWriteFormated(" | ");
	ConWriteFormated("ES:     "); ArchPanicWriteHex((UInt8)regs->es); ConWriteFormated(" | ");
	ConWriteFormated("FS:  "); ArchPanicWriteHex((UInt8)regs->fs); ConWriteFormated(" |\r\n");
	
	ConWriteFormated("| GS:  "); ArchPanicWriteHex((UInt8)regs->gs); ConWriteFormated(" | ");
	ConWriteFormated("SS:  "); ArchPanicWriteHex((UInt8)regs->ss); ConWriteFormated(" | ");
	ConWriteFormated("                   | ");
	ConWriteFormated("                |\r\n");
	
	PanicInt(err, True);																						// Print the error code
	DispRefresh();																								// Refresh the screen
	ArchHalt();																									// Halt
}
