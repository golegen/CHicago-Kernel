// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on November 17 of 2018, at 13:06 BRT

#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/display.h>
#include <chicago/exec.h>
#include <chicago/file.h>
#include <chicago/ipc.h>
#include <chicago/panic.h>
#include <chicago/version.h>

Void KernelMain(Void) {
	ArchInitDebug();																										// Init the architecture-dependent debugging method
	DbgWriteFormated("[Kernel] Arch debugging initialized\r\n");
	
	DbgWriteFormated("[Kernel] CHicago %s (codename %s, for %s)\r\n", CHICAGO_VSTR, CHICAGO_CODENAME, CHICAGO_ARCH);		// Print the system version
	
	ArchInitFPU();																											// Init the architecture-dependent FPU (floating point unit)
	DbgWriteFormated("[Kernel] Arch FPU initialized\r\n");
	
	ArchInitPMM();																											// Init the physical memory manager
	DbgWriteFormated("[Kernel] Arch PMM initialized\r\n");
	
	ArchInitVMM();																											// Init the virtual memory manager
	DbgWriteFormated("[Kernel] Arch VMM initialized\r\n");
	
	ArchInitDisplay();																										// Init the display
	DbgWriteFormated("[Kernel] Arch display initialized\r\n");
	
	ArchInitMouse();																										// Init the mouse
	DbgWriteFormated("[Kernel] Arch mouse initialized\r\n");
	
	ArchInitKeyboard();																										// Init the keyboard
	DbgWriteFormated("[Kernel] Arch keyboard intialized\r\n");
	
	DispDrawProgessBar();																									// Draw the progress bar
	DbgWriteFormated("[Kernel] The boot progress bar has been shown\r\n");
	
	ArchInitSc();																											// Init the system call interface
	DbgWriteFormated("[Kernel] Arch system call interface initialized\r\n");
	
	ArchInit();																												// Let's finish it by initalizating the other architecture-dependent bits of the kernel
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Arch initialized\r\n");	
	
	FsInitDevices();																										// Now init the basic devices
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Devices initialized\r\n");
	
	FsInit();																												// Init the filesystem list, mount point list, and add the basic mount points
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Filesystem initialized\r\n");
	
	PsInit();																												// Init tasking
	ArchHalt();																												// Halt
}

Void KernelMainLate(Void) {
	DbgWriteFormated("[Kernel] Tasking initialized\r\n");																	// Tasking initialized
	
	IpcInit();																												// Init the IPC interface
	DbgWriteFormated("[Kernel] IPC initialized\r\n");
	
	DispFillProgressBar();																									// Kernel initialized
	DbgWriteFormated("[Kernel] Kernel initialized\r\n\r\n");
	
	ConClearScreen();																										// Clear the screen
	ConWriteFormated("CHicago Operating System for %s\r\n", CHICAGO_ARCH);													// Print some system informations
	ConWriteFormated("Codename '%s'\r\n", CHICAGO_CODENAME);
	ConWriteFormated("%s\r\n\r\n", CHICAGO_VSTR);
	
	PProcess proc = ExecCreateProcess("\\System\\Programs\\sesmgr.che");													// Let's create the session manager process!
	
	if (proc == Null) {
		DbgWriteFormated("PANIC! Failed to run \\System\\Programs\\sesmgr.che\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	UIntPtr pid = proc->id;																									// Save the pid of the session manager process
	
	PsAddProcess(proc);																										// RUN!
	PsWaitProcess(pid);																										// Session manager is not supposed to exit, so this function should never return
	DbgWriteFormated("PANIC! The \\System\\Programs\\sesmgr.che program closed\r\n");										// ...
	Panic(PANIC_KERNEL_UNEXPECTED_ERROR);																					// Panic
}
