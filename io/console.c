// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 15:20 BRT
// Last edited on October 27 of 2018, at 22:12 BRT

#include <chicago/display.h>
#include <chicago/process.h>
#include <chicago/string.h>

IntPtr ConScale = 1;
Lock ConLock = False;
UIntPtr ConCursorX = 0;
UIntPtr ConCursorY = 0;
Boolean ConRefresh = True;
UIntPtr ConBackgroundColor = 0xFF000000;
UIntPtr ConForegroundColor = 0xFFAAAAAA;

Void ConSetRefresh(Boolean s) {
	PsLock(&ConLock);																												// Lock
	ConRefresh = s;																													// Set the automatic refresh prop
	PsUnlock(&ConLock);																												// Unlock
}

Boolean ConGetRefresh(Void) {
	PsLock(&ConLock);																												// Lock
	Boolean s = ConScale;																											// Save the automatic refresh prop
	PsUnlock(&ConLock);																												// Unlock
	return s;																														// Return it
}

Void ConSetScale(IntPtr scale) {
	PsLock(&ConLock);																												// Lock
	ConScale = scale;																												// Set the scale of the console font
	PsUnlock(&ConLock);																												// Unlock
}

IntPtr ConGetScale(Void) {
	PsLock(&ConLock);																												// Lock
	IntPtr scale = ConScale;																										// Save the scale
	PsUnlock(&ConLock);																												// Unlock
	return scale;																													// Return it
}

Void ConSetColor(UIntPtr bg, UIntPtr fg) {
	PsLock(&ConLock);																												// Lock
	ConBackgroundColor = bg;																										// Set the background of the console
	ConForegroundColor = fg;																										// Set the foreground of the console
	PsUnlock(&ConLock);																												// Unlock
}

Void ConSetBackground(UIntPtr c) {
	PsLock(&ConLock);																												// Lock
	ConBackgroundColor = c;																											// Set the background of the console
	PsUnlock(&ConLock);																												// Unlock
}

Void ConSetForeground(UIntPtr c) {
	PsLock(&ConLock);																												// Lock
	ConForegroundColor = c;																											// Set the foreground of the console
	PsUnlock(&ConLock);																												// Unlock
}

Void ConGetColor(PUIntPtr bg, PUIntPtr fg) {
	PsLock(&ConLock);																												// Lock
	
	if (bg != Null) {																												// We should save the bg?
		*bg = ConBackgroundColor;																									// Yes
	}
	
	if (fg != Null) {																												// And the fg?
		*fg = ConForegroundColor;																									// Yes
	}
	
	PsUnlock(&ConLock);																												// Unlock
}

UIntPtr ConGetBackground(Void) {
	PsLock(&ConLock);																												// Lock
	IntPtr bg = ConBackgroundColor;																									// Save the bg
	PsUnlock(&ConLock);																												// Unlock
	return bg;																														// Return it
}

UIntPtr ConGetForeground(Void) {
	PsLock(&ConLock);																												// Lock
	IntPtr fg = ConForegroundColor;																									// Save the fg
	PsUnlock(&ConLock);																												// Unlock
	return fg;																														// Return it
}

Void ConSetCursor(UIntPtr x, UIntPtr y) {
	PsLock(&ConLock);																												// Lock
	ConCursorX = x;																													// Set the x position of the cursor
	ConCursorY = y;																													// Set the y position of the cursor
	PsUnlock(&ConLock);																												// Unlock
}

Void ConSetCursorX(UIntPtr pos) {
	PsLock(&ConLock);																												// Lock
	ConCursorX = pos;																												// Set the x position of the cursor
	PsUnlock(&ConLock);																												// Unlock
}

Void ConSetCursorY(UIntPtr pos) {
	PsLock(&ConLock);																												// Lock
	ConCursorY = pos;																												// Set the y position of the cursor
	PsUnlock(&ConLock);																												// Unlock
}

Void ConGetCursor(PUIntPtr x, PUIntPtr y) {
	PsLock(&ConLock);																												// Lock
	
	if (x != Null) {																												// We should save the x?
		*x = ConCursorX;																											// Yes
	}
	
	if (y != Null) {																												// And the y?
		*y = ConCursorY;																											// Yes
	}
	
	PsUnlock(&ConLock);																												// Unlock
}

UIntPtr ConGetCursorX(Void) {
	PsLock(&ConLock);																												// Lock
	IntPtr x = ConCursorX;																											// Save the x
	PsUnlock(&ConLock);																												// Unlock
	return x;																														// Return it
}

UIntPtr ConGetCursorY(Void) {
	PsLock(&ConLock);																												// Lock
	IntPtr y = ConCursorY;																											// Save the y
	PsUnlock(&ConLock);																												// Unlock
	return y;																														// Return it
}

Void ConClearScreen(Void) {
	PsLock(&ConLock);																												// Lock
	DispClearScreen(ConBackgroundColor);																							// Clear the screen
	ConCursorX = ConCursorY = 0;																									// Move the cursor to 0, 0
	
	if (ConRefresh) {
		DispRefresh();																												// Refresh the screen
	}
	
	PsUnlock(&ConLock);																												// Unlock
}

static Void ConWriteCharacterInt(Char data) {
	switch (data) {
	case '\b': {																													// Backspace
		if (ConCursorX > 0) {																										// We have any more characters to delete in this line?
			ConCursorX++;																											// Yes
		} else if (ConCursorY > 0) {																								// We have any more lines?																										
			ConCursorY--;																											// Yes
			ConCursorX = DispGetWidth() / (ConScale * 8) - 1;
		}
		
		break;
	}
	case '\n': {																													// Line feed
		ConCursorY++;
		break;
	}
	case '\r': {																													// Carriage return
		ConCursorX = 0;
		break;
	}
	case '\t': {																													// Tab
		ConCursorX = (ConCursorX + 4) & ~3;
		break;
	}
	default: {																														// Character
		for (IntPtr i = 0; i < ConScale * 16; i++) {
			for (IntPtr j = ConScale * 8; j >= 0; j--) {
				if (DispFont[(UInt8)data][i * 8 / (ConScale * 8)] & (1 << (j * 16 / (ConScale * 16)))) {							// Put pixel if we need
					DispPutPixel((ConCursorX * (ConScale * 8)) + j, (ConCursorY * (ConScale * 16)) + i, ConForegroundColor);
				}
			}
		}
		
		ConCursorX++;
		
		break;
	}
	}
	
	if (ConCursorX >= DispGetWidth() / (ConScale * 8)) {																			// Go to the next line?
		ConCursorX = 0;																												// Yes
		ConCursorY++;
	}
	
	if (ConCursorY >= DispGetHeight() / (ConScale * 16)) {																			// Scroll up?
		DispScrollScreen(ConScale, ConBackgroundColor);																				// Yes
		ConCursorY = DispGetHeight() / (ConScale * 16) - 1;
	}
}

Void ConWriteCharacter(Char data) {
	PsLock(&ConLock);																												// Lock
	ConWriteCharacterInt(data);																										// Write the character
	
	if (ConRefresh) {
		DispRefresh();																												// Refresh the screen
	}
	
	PsUnlock(&ConLock);																												// Unlock
}

static Void ConWriteStringInt(PChar data) {
	if (data == Null) {
		return;
	}
	
	for (UIntPtr i = 0; i < StrGetLength(data); i++) {																				// Write all the characters from the string
		ConWriteCharacterInt(data[i]);
	}
}

Void ConWriteString(PChar data) {
	PsLock(&ConLock);																												// Lock
	ConWriteStringInt(data);																										// Write the string
	
	if (ConRefresh) {
		DispRefresh();																												// Refresh the screen
	}
	
	PsUnlock(&ConLock);																												// Unlock
}

static Void ConWriteIntegerInt(UIntPtr data, UInt8 base) {
	if (data == 0) {
		ConWriteCharacterInt('0');
		return;
	}
	
	static Char buf[32] = { 0 };
	Int i = 30;
	
	for (; data && i; i--, data /= base) {
		buf[i] = "0123456789ABCDEF"[data % base];
	}
	
	ConWriteStringInt(&buf[i + 1]);
}

Void ConWriteInteger(UIntPtr data, UInt8 base) {
	PsLock(&ConLock);																												// Lock
	ConWriteIntegerInt(data, base);																									// Write the integer
	
	if (ConRefresh) {
		DispRefresh();																												// Refresh the screen
	}
	
	PsUnlock(&ConLock);																												// Unlock
}

Void ConWriteFormated(PChar data, ...) {
	if (data == Null) {
		return;
	}
	
	PsLock(&ConLock);																												// Lock
	
	VariadicList va;
	VariadicStart(va, data);																										// Let's start our va list with the arguments provided by the user (if any)
	
	for (UIntPtr i = 0; i < StrGetLength(data); i++) {
		if (data[i] != '%') {																										// It's an % (integer, string, character or other)?
			ConWriteCharacterInt(data[i]);																							// No
		} else {
			switch (data[++i]) {																									// Yes, let's parse it!
			case 's': {																												// String
				ConWriteStringInt((PChar)VariadicArg(va, PChar));
				break;
			}
			case 'c': {																												// Character
				ConWriteCharacterInt((Char)VariadicArg(va, Int));
				break;
			}
			case 'd': {																												// Decimal Number
				ConWriteIntegerInt((UIntPtr)VariadicArg(va, UIntPtr), 10);
				break;
			}
			case 'x': {																												// Hexadecimal Number
				ConWriteIntegerInt((UIntPtr)VariadicArg(va, UIntPtr), 16);
				break;
			}
			default: {																												// Probably it's another % (probably)
				ConWriteCharacterInt(data[i]);
				break;
			}
			}
		}
	}
	
	VariadicEnd(va);
	
	if (ConRefresh) {
		DispRefresh();																												// Refresh the screen
	}
	
	PsUnlock(&ConLock);																												// Unlock
}
