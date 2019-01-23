// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:14 BRT
// Last edited on January 22 of 2019, at 20:42 BRT

#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/display.h>
#include <chicago/file.h>
#include <chicago/ipc.h>
#include <chicago/net.h>
#include <chicago/nls.h>
#include <chicago/panic.h>
#include <chicago/shell.h>
#include <chicago/version.h>

Void KernelMain(Void) {
	ArchInitDebug();																										// Init the architecture-dependent debugging method
	DbgWriteFormated("[Kernel] Arch debugging initialized\r\n");
	
	DbgWriteFormated("[Kernel] CHicago %s (codename %s, for %s)\r\n", CHICAGO_VSTR_C, CHICAGO_CODENAME_C, CHICAGO_ARCH_C);	// Print the system version
	
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
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] Tasking initialized\r\n");																	// Tasking initialized
	
	IpcInit();																												// Init the IPC interface
	DispIncrementProgessBar();
	DbgWriteFormated("[Kernel] IPC initialized\r\n");
	
	ArchFinishKeyboard();																									// Now we can start handling the keyboard!
	NetFinish();																											// And start handling the network packets
	DispFillProgressBar();																									// Kernel initialized
	DbgWriteFormated("[Kernel] Kernel initialized\r\n\r\n");
	
	ConClearScreen();																										// Clear the screen
	ConWriteFormated(NlsGetMessage(NLS_OS_NAME), CHICAGO_ARCH);																// Print some system informations
	ConWriteFormated(NlsGetMessage(NLS_OS_CODENAME), CHICAGO_CODENAME);
	ConWriteFormated(NlsGetMessage(NLS_OS_VSTR), CHICAGO_MAJOR, CHICAGO_MINOR, CHICAGO_BUILD);
	
	ShellRun();																												// Run the shell!
	while (True) { PsSwitchTask(PsDontRequeue); }																			// Don't requeue us
}
