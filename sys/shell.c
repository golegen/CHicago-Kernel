// File author is Ítalo Lima Marconato Matias
//
// Created on December 08 of 2018, at 10:28 BRT
// Last edited on December 08 of 2018, at 11:38 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/panic.h>
#include <chicago/string.h>
#include <chicago/version.h>

Void ShellRun(Void) {
	PChar cmd = (PChar)MemAllocate(1024);																						// Alloc memory for reading the keyboard
	PChar *argv = (PChar*)MemAllocate(sizeof(PChar) * 256);																		// Alloc memory for transforming the cmd into argc/argv
	
	if ((cmd == Null) || (argv == Null)) {
		DbgWriteFormated("PANIC! Couldn't init the shell\r\n");																	// Failed... PANIC!
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	while (True) {
		ConWriteFormated("%fchicago%f@%f%s%r $ ", 0xFFAA5500, 0xFF55FF55, 0xFF5555FF, CHICAGO_CODENAME);						// Print the prompt
		ConsoleDeviceReadKeyboard(1023, cmd);																					// Read from the keyboard
		
		PChar tok = StrTokenize(cmd, " ");																						// Let's tokenize!
		UIntPtr argc = 0;
		
		while (tok != Null) {
			argv[argc++] = tok;																									// Add this arg
			tok = StrTokenize(Null, " ");																						// Go to the next one!
		}
		
		if (StrGetLength(argv[0]) == 4 && StrCompare(argv[0], "echo")) {														// Print the arguments to the screen (echo)
			for (UIntPtr i = 1; i < argc; i++) {
				ConWriteFormated("%s%s%s", i != 1 ? " " : "", argv[i], (i + 1) >= argc ? "\r\n" : "");
			}
		} else if (StrGetLength(argv[0]) == 4 && StrCompare(argv[0], "help")) {													// Print the avaliable commands
			ConWriteFormated("%s\r\n%s\r\n%s\r\n",
							 "echo - Print the arguments to the screen",
							 "help - Print the avaliable commands",
							 "ver  - Print the system version");
		} else if (StrGetLength(argv[0]) == 3 && StrCompare(argv[0], "ver")) {													// Print the system version
			ConWriteFormated("CHicago Operating System\r\nCodename '%s'\r\n%s\r\n", CHICAGO_CODENAME, CHICAGO_VSTR);
		} else {
			ConWriteFormated("Invalid command '%s'\r\n", argv[0]);																// Invalid command!
		}
		
		ConWriteFormated("\r\n");
	}
	
	ArchHalt();																													// We should never get here...
}
