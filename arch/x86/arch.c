// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:21 BRT
// Last edited on November 17 of 2018, at 11:44 BRT

#include <chicago/arch/bootmgr.h>
#include <chicago/arch/gdt.h>
#include <chicago/arch/ide.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/pit.h>
#include <chicago/arch/pmm.h>
#include <chicago/arch/port.h>
#include <chicago/arch/process.h>
#include <chicago/arch/serial.h>
#include <chicago/arch/vmm.h>

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/display.h>
#include <chicago/heap.h>
#include <chicago/mm.h>
#include <chicago/panic.h>
#include <chicago/string.h>

Void ArchHalt(Void) {
	while (True) {
		Asm Volatile("hlt");																					// Halt!
	}
}

Void ArchInitFPU(Void) {
	UInt16 cw0 = 0x37E;
	UInt16 cw1 = 0x37A;
	UIntPtr cr0;
	UIntPtr d;
	
	if (!CPUIDCheck()) {																						// Let's check if we can use the CPUID instruction
		DbgWriteFormated("PANIC! CPUID instruction isn't avaliable\r\n");										// We can't but we need it...
		ArchHalt();																								// Halt
	}
	
	Asm Volatile("cpuid" : "=d"(d) : "a"(1) : "ecx", "ebx");													// EAX = 1, Get features
	
	if (!(d & (1 << 0))) {																						// FPU avaliable?
		DbgWriteFormated("PANIC! FPU isn't avaliable\r\n");														// Nope
		ArchHalt();																								// Halt
	}
	
	Asm Volatile("mov %%cr0, %0" : "=r"(cr0));
	
	cr0 &= ~(1 << 3);																							// Disable the EMulation bit
	
	Asm Volatile("mov %0, %%cr0" :: "r"(cr0));
	Asm Volatile("fninit");																						// Write some initial FPU settings
	Asm Volatile("fldcw %0" :: "m"(cw0));																		// Invalid operand exceptions enabled
	Asm Volatile("fldcw %0" :: "m"(cw1));																		// Both division-by-zero and invalid operands cause exceptions
	Asm Volatile("fxsave (%0)" :: "r"(PsFPUDefaultState));														// Save the curr state (as it is the "default state")
}

Void ArchInitPMM(Void) {
	PMMInit();																									// Init the PMM
}

Void ArchInitVMM(Void) {
	for (UIntPtr i = 0xC0800000; i < 0xE0800000; i += 0x400000) {												// First, let's pre alloc the page tables for our heap
		if ((MmGetPDE(i) & 0x01) != 0x01) {																		// We need to alloc this one?
			UIntPtr phys = MmReferencePage(0);																	// Yes, try to alloc a new page
			
			if (phys == 0) {
				DbgWriteFormated("PANIC! Couldn't init the VMM\r\n");											// Failed...
				ArchHalt();																						// Halt
			}
			
			MmSetPDE(i, phys, 0x03);																			// Set the physical address of this PDE
		}
	}
	
	HeapInit(0xC0800000, 0xE0800000);																			// 512 MiB heap, starts at 0xC0800000 and ends at 0xE0800000
}

Void ArchInitDebug(Void) {
	SerialInit(COM1_PORT);																						// Init debugging (using COM1 port)
}

Void ArchInitDisplay(Void) {
	DispInit(BootmgrDispWidth, BootmgrDispHeight, BootmgrDispBpp / 8, BootmgrDispPhysAddr);						// Init the display
}

UIntPtr ArchGetSeconds(Void) {
	UIntPtr sec;																								// Let's use the RTC!
	
	PortOutByte(0x70, 0);
	sec = PortInByte(0x71);
	
	if (!(PortInByte(0x0B) & 0x04)) {																			// Convert from BCD?
		sec = (sec & 0x0F) + ((sec / 16) * 10);																	// Yes
	}
	
	return sec;
}

Void ArchInit(Void) {
	GDTInit();																									// Init the GDT
	DbgWriteFormated("[x86] GDT initialized\r\n");
	
	IDTInit();																									// Init the IDT
	IDTRegisterInterruptHandler(0x3E, PsSwitchTaskForce);														// Register the force switch handler
	DbgWriteFormated("[x86] IDT initialized\r\n");
	
	PITInit();																									// Init the PIT
	DbgWriteFormated("[x86] PIT initialized\r\n");
	
	FsInitDeviceList();																							// Init the x86-only devices (and the device list)
	IDEInit();
	
	FsSetBootDevice(BootmgrBootDev);																			// Try to set the boot device
	
	if (FsGetBootDevice() == Null) {
		DbgWriteFormated("PANIC! Couldn't set the boot device\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	} else {
		DbgWriteFormated("[x86] Devices initialized\r\n");
	}
}
