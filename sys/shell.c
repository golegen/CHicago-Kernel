// File author is Ítalo Lima Marconato Matias
//
// Created on December 08 of 2018, at 10:28 BRT
// Last edited on December 09 of 2018, at 10:08 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/display.h>
#include <chicago/heap.h>
#include <chicago/panic.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/version.h>

static UIntPtr BVal(UIntPtr bytes) {
	if (bytes < 1024) {																																	// Less than 1KB?
		return bytes;																																	// Yes, just return in bytes
	} else if (bytes < 1024 * 1024) {																													// Less than 1MB?
		return bytes / 1024;																															// Yes, return in kilobytes
	} else if (bytes < 1024 * 1024 * 1024) {																											// Less than 1GB?
		return bytes / (1024 * 1024);																													// Yes, return in megabytes
	} else {
		return bytes / (1024 * 1024 * 1024);																											// Return in gigabytes
	}
}

static PChar BStr(UIntPtr bytes) {
	if (bytes < 1024) {																																	// Less than 1KB?
		return "B";
	} else if (bytes < 1024 * 1024) {																													// Less than 1MB?
		return "KB";
	} else if (bytes < 1024 * 1024 * 1024) {																											// Less than 1GB?
		return "MB";
	} else {
		return "GB";																																	// 1GB+
	}
}

static Void ShellMain(Void) {
	PChar cmd = (PChar)MemAllocate(1024);																												// Alloc memory for reading the keyboard
	PChar *argv = (PChar*)MemAllocate(sizeof(PChar) * 256);																								// Alloc memory for transforming the cmd into argc/argv
	
	if ((cmd == Null) || (argv == Null)) {
		DbgWriteFormated("PANIC! Couldn't init the shell\r\n");																							// Failed... PANIC!
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	while (True) {
		ConWriteFormated("%fchicago%f@%f%s%r $ ", 0xFFAA5500, 0xFF55FF55, 0xFF5555FF, CHICAGO_CODENAME);												// Print the prompt
		ConsoleDeviceReadKeyboard(1023, cmd);																											// Read from the keyboard
		
		PChar tok = StrTokenize(cmd, " ");																												// Let's tokenize!
		UIntPtr argc = 0;
		
		while (tok != Null) {
			argv[argc++] = tok;																															// Add this arg
			tok = StrTokenize(Null, " ");																												// Go to the next one!
		}
		
		if (StrGetLength(argv[0]) == 3 && StrCompare(argv[0], "cls")) {																					// Clear the screen
			ConClearScreen();
		} else if (StrGetLength(argv[0]) == 4 && StrCompare(argv[0], "echo")) {																			// Print the arguments to the screen
			ConSetRefresh(False);																														// Disable screen refresh
			
			for (UIntPtr i = 1; i < argc; i++) {																										// Print
				ConWriteFormated("%s%s%s", i != 1 ? " " : "", argv[i], (i + 1) >= argc ? "\r\n\r\n" : "");
			}
			
			DispRefresh();																																// Refresh the screen
			ConSetRefresh(True);																														// Enable screen refresh
		} else if (StrGetLength(argv[0]) == 4 && StrCompare(argv[0], "help")) {																			// Print the avaliable commands
			ConWriteFormated("%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n\r\n",
							 "cls   - Clear the screen",
							 "echo  - Print the arguments to the screen",
							 "help  - Print the avaliable commands",
							 "panic - Crash the system",
							 "ps    - List all the processes",
							 "ver   - Print the system version");
		} else if (StrGetLength(argv[0]) == 5 && StrCompare(argv[0], "panic")) {																		// Crash the system
			DbgWriteFormated("PANIC! User requested panic :)\r\n");
			Panic(PANIC_KERNEL_UNEXPECTED_ERROR);
		} else if (StrGetLength(argv[0]) == 2 && StrCompare(argv[0], "ps")) {																			// List all the processes
			ConSetRefresh(False);																														// Disable screen refresh
			
			ListForeach(PsProcessList, i) {																												// Print all the processes
				PProcess proc = (PProcess)i->data;
				PChar name = proc->name == Null ? "<unamed>" : proc->name;
				UIntPtr usage = proc->mem_usage + (proc->id == 0 ? HeapGetCurrent() - HeapGetStart() : 0);
				
				ConWriteFormated("Name: %s | ID: %d | Usage: %d %s\r\n%s", name, proc->id, BVal(usage), BStr(usage), i->next == Null ? "\r\n" : "");
			}
			
			DispRefresh();																																// Refresh the screen
			ConSetRefresh(True);																														// Enable screen refresh
		} else if (StrGetLength(argv[0]) == 3 && StrCompare(argv[0], "ver")) {																			// Print the system version
			ConWriteFormated("CHicago Operating System for %s\r\nCodename '%s'\r\n%s\r\n\r\n", CHICAGO_ARCH, CHICAGO_CODENAME, CHICAGO_VSTR);
		} else {
			ConWriteFormated("Invalid command '%s'\r\n\r\n", argv[0]);																					// Invalid command!
		}
	}
	
	ArchHalt();																																			// We should never get here...
}

Void ShellRun(Void) {
	PProcess proc = PsCreateProcess("Shell", (UIntPtr)ShellMain);																						// Create the shell process
	
	if (proc == Null) {
		DbgWriteFormated("PANIC! Couldn't init the shell\r\n");																							// Failed... PANIC!
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	PsAddProcess(proc);																																	// (Try to) add it!
}
