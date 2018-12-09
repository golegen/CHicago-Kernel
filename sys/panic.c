// File author is Ítalo Lima Marconato Matias
//
// Created on October 27 of 2018, at 21:37 BRT
// Last edited on December 09 of 2018, at 20:04 BRT

#include <chicago/console.h>
#include <chicago/nls.h>
#include <chicago/panic.h>

PWChar PanicStrings[PANIC_COUNT] = {
	L"MM_READWRITE_TO_NONPRESENT_AREA",
	L"MM_WRITE_TO_READONLY_AREA",
	L"KERNEL_UNEXPECTED_ERROR",
	L"KERNEL_INIT_FAILED"
};

Void PanicInt(UInt32 err, Boolean perr) {
	if (!perr) {																														// Print the "Sorry" message?
		ConSetColor(0xFF8B0000, 0xFFFFFFFF);																							// Yes. Red background, white foreground
		ConClearScreen();																												// Clear the screen
		ConWriteFormated(NlsGetMessage(NLS_PANIC_SORRY));																				// And print the "Sorry" message
	} else {																															// Print the error code?
		ConWriteFormated(NlsGetMessage(NLS_PANIC_ERRCODE), err < PANIC_COUNT ? PanicStrings[err] : L"UNKNOWN_ERROR");					// Yes
	}
}
