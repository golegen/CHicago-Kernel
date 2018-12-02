// File author is Ítalo Lima Marconato Matias
//
// Created on October 27 of 2018, at 21:37 BRT
// Last edited on November 15 of 2018, at 16:02 BRT

#include <chicago/console.h>
#include <chicago/panic.h>

PChar PanicStrings[PANIC_COUNT] = {
	"MM_READWRITE_TO_NONPRESENT_AREA",
	"MM_WRITE_TO_READONLY_AREA",
	"KERNEL_UNEXPECTED_ERROR",
	"KERNEL_INIT_FAILED"
};

Void PanicInt(UInt32 err, Boolean perr) {
	if (!perr) {																									// Print the "Sorry" message?
		ConSetColor(0xFF8B0000, 0xFFFFFFFF);																		// Yes. Red background, white foreground
		ConClearScreen();																							// Clear the screen
		ConWriteFormated("Sorry, CHicago got a fatal error and can't continue operating.\r\n");						// And print the "Sorry" message
		ConWriteFormated("You need to restart your computer manually.\r\n\r\n");
	} else {																										// Print the error code?
		ConWriteFormated("\r\nError Code: %s\r\n", err < PANIC_COUNT ? PanicStrings[err] : "UNKNOWN_ERROR");		// Yes
	}
}
