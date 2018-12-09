// File author is Ítalo Lima Marconato Matias
//
// Created on December 08 of 2018, at 10:28 BRT
// Last edited on December 09 of 2018, at 20:41 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/display.h>
#include <chicago/heap.h>
#include <chicago/nls.h>
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

static PWChar BStr(UIntPtr bytes) {
	if (bytes < 1024) {																																	// Less than 1KB?
		return L"B";
	} else if (bytes < 1024 * 1024) {																													// Less than 1MB?
		return L"KB";
	} else if (bytes < 1024 * 1024 * 1024) {																											// Less than 1GB?
		return L"MB";
	} else {
		return L"GB";																																	// 1GB+
	}
}

static Void ShellMain(Void) {
	PWChar cmd = (PWChar)MemAllocate(1024);																												// Alloc memory for reading the keyboard
	PWChar *argv = (PWChar*)MemAllocate(sizeof(PWChar) * 256);																							// Alloc memory for transforming the cmd into argc/argv
	
	if ((cmd == Null) || (argv == Null)) {
		DbgWriteFormated("PANIC! Couldn't init the shell\r\n");																							// Failed... PANIC!
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	while (True) {
		ConWriteFormated(L"%fchicago%f@%f%s%r $ ", 0xFFAA5500, 0xFF55FF55, 0xFF5555FF, CHICAGO_CODENAME);												// Print the prompt
		ConsoleDeviceReadKeyboard(1023, cmd);																											// Read from the keyboard
		
		PWChar tok = StrTokenize(cmd, L" ");																											// Let's tokenize!
		UIntPtr argc = 0;
		
		while (tok != Null) {
			argv[argc++] = tok;																															// Add this arg
			tok = StrTokenize(Null, L" ");																												// Go to the next one!
		}
		
		if (StrGetLength(argv[0]) == 3 && StrCompare(argv[0], L"cls")) {																				// Clear the screen
			ConClearScreen();
		} else if (StrGetLength(argv[0]) == 4 && StrCompare(argv[0], L"echo")) {																		// Print the arguments to the screen
			ConSetRefresh(False);																														// Disable screen refresh
			
			for (UIntPtr i = 1; i < argc; i++) {																										// Print
				ConWriteFormated(L"%s%s%s", i != 1 ? L" " : L"", argv[i], (i + 1) >= argc ? L"\r\n\r\n" : L"");
			}
			
			DispRefresh();																																// Refresh the screen
			ConSetRefresh(True);																														// Enable screen refresh
		} else if (StrGetLength(argv[0]) == 4 && StrCompare(argv[0], L"help")) {																		// Print the avaliable commands
			ConWriteFormated(NlsGetMessage(NLS_SHELL_HELP));
		} else if (StrGetLength(argv[0]) == 4 && StrCompare(argv[0], L"lang")) {																		// Change the system language
			if (argc == 1) {
				ConWriteFormated(NlsGetMessage(NLS_SHELL_LANG_LIST), NlsGetLanguages());																// Just list the languages
			} else if (NlsGetLanguage(argv[1]) == (UIntPtr)-1) {																						// Valid language?
				ConWriteFormated(NlsGetMessage(NLS_SHELL_LANG_INVALID), argv[1]);																		// Nope...
			} else {
				NlsSetLanguage(NlsGetLanguage(argv[1]));																								// Ok, set!
				ConWriteFormated(L"\r\n");
			}
		} else if (StrGetLength(argv[0]) == 5 && StrCompare(argv[0], L"panic")) {																		// Crash the system
			PsCurrentProcess->id = PsCurrentThread->id = 0;																								// *HACK*
			DbgWriteFormated("PANIC! User requested panic :)\r\n");
			Panic(PANIC_KERNEL_UNEXPECTED_ERROR);
		} else if (StrGetLength(argv[0]) == 2 && StrCompare(argv[0], L"ps")) {																			// List all the processes
			ConSetRefresh(False);																														// Disable screen refresh
			
			ListForeach(PsProcessList, i) {																												// Print all the processes
				PProcess proc = (PProcess)i->data;
				PWChar name = proc->name == Null ? NlsGetMessage(NLS_SHELL_PS_UNAMED) : proc->name;
				UIntPtr usage = proc->mem_usage + (proc->id == 0 ? HeapGetCurrent() - HeapGetStart() : 0);
				
				ConWriteFormated(NlsGetMessage(NLS_SHELL_PS), name, proc->id, BVal(usage), BStr(usage), i->next == Null ? L"\r\n" : L"");
			}
			
			DispRefresh();																																// Refresh the screen
			ConSetRefresh(True);																														// Enable screen refresh
		} else if (StrGetLength(argv[0]) == 3 && StrCompare(argv[0], L"ver")) {																			// Print the system version
			ConSetRefresh(False);																														// Disable screen refresh
			ConWriteFormated(NlsGetMessage(NLS_OS_NAME), CHICAGO_ARCH);																					// Print some system informations
			ConWriteFormated(NlsGetMessage(NLS_OS_CODENAME), CHICAGO_CODENAME);
			ConWriteFormated(NlsGetMessage(NLS_OS_VSTR), CHICAGO_MAJOR, CHICAGO_MINOR, CHICAGO_BUILD);
			DispRefresh();																																// Refresh the screen
			ConSetRefresh(True);																														// Enable screen refresh
		} else {
			ConWriteFormated(NlsGetMessage(NLS_SHELL_INVALID), argv[0]);																				// Invalid command!
		}
	}
	
	ArchHalt();																																			// We should never get here...
}

Void ShellRun(Void) {
	PProcess proc = PsCreateProcess(L"Shell", (UIntPtr)ShellMain);																						// Create the shell process
	
	if (proc == Null) {
		DbgWriteFormated("PANIC! Couldn't init the shell\r\n");																							// Failed... PANIC!
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	PsAddProcess(proc);																																	// (Try to) add it!
}
