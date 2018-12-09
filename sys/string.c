// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 19:05 BRT
// Last edited on December 09 of 2018, at 19:08 BRT

#include <chicago/alloc.h>

PVoid StrCopyMemory(PVoid dest, PVoid src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt8)dest)[i] = ((PUInt8)src)[i];
	}
	
	return dest;
}

PVoid StrCopyMemory24(PVoid dest, PVoid src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		*((PUInt16)(dest + (i * 3))) = *(PUInt16)(src + (i * 3));
		*((PUInt8)(dest + (i * 3) + 2)) = *(PUInt8)(src + (i * 3) + 2);
	}
	
	return dest;
}

PVoid StrCopyMemory32(PVoid dest, PVoid src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt32)dest)[i] = ((PUInt32)src)[i];
	}
	
	return dest;
}

PVoid StrSetMemory(PVoid dest, UInt8 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt8)dest)[i] = val;
	}
	
	return dest;
}

PVoid StrSetMemory24(PVoid dest, UInt32 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		*((PUInt16)(dest + (i * 3))) = (UInt16)val;
		*((PUInt8)(dest + (i * 3) + 2)) = (UInt8)(val << 16);
	}
	
	return dest;
}

PVoid StrSetMemory32(PVoid dest, UInt32 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt32)dest)[i] = val;
	}
	
	return dest;
}

Boolean StrCompareMemory(PVoid m1, PVoid m2, UIntPtr count) {
	if ((m1 == Null) || (m2 == Null) || (count == 0)) {								// m1 is an Null pointer? m2 is an Null pointer? Zero-sized compare?
		return False;																// Yes
	}
	
	PUInt8 mp1 = m1;
	PUInt8 mp2 = m2;
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		if (*mp1++ != *mp2++) {
			return False;
		}
	}
	
	return True;
}

UIntPtr StrGetLength(PWChar str) {
	if (str == Null) {																// Str is an Null pointer?
		return 0;																	// Yes
	}
	
	UIntPtr n = 0;
	
	for (; str[n] != 0; n++) ;														// Again, GCC should optimize this for us (if possible)
	
	return n;
}

UIntPtr StrGetLengthC(PChar str) {
	if (str == Null) {																// Str is an Null pointer?
		return 0;																	// Yes
	}
	
	UIntPtr n = 0;
	
	for (; str[n] != 0; n++) ;														// Again, GCC should optimize this for us (if possible)
	
	return n;
}

Boolean StrCompare(PWChar dest, PWChar src) {
	return StrCompareMemory(dest, src, StrGetLength(dest) * 4);						// Just redirect to the StrCompareMemory function
}

Boolean StrCompareC(PChar dest, PChar src) {
	return StrCompareMemory(dest, src, StrGetLengthC(dest) + 1);					// Just redirect to the StrCompareMemory function
}

PWChar StrCopy(PWChar dest, PWChar src) {
	return StrCopyMemory32(dest, src, StrGetLength(src) + 1);						// Just redirect to StrCopyMemory function
}

PChar StrCopyC(PChar dest, PChar src) {
	return StrCopyMemory(dest, src, StrGetLengthC(src) + 1);						// Just redirect to StrCopyMemory function
}

Void StrConcatenate(PWChar dest, PWChar src) {
	if ((dest == Null) || (src == Null)) {											// Destination is an Null pointer? Source is an Null pointer?
		return;																		// Yes :(
	}
	
	PWChar end = dest + StrGetLength(dest);											// Let's do it!
	
	StrCopyMemory32(end, src, StrGetLength(src));									// Let's append the src to dest
	
	end += StrGetLength(src);														// And put an 0 (NUL) at the end
	*end = '\0';
}

Void StrConcatenateC(PChar dest, PChar src) {
	if ((dest == Null) || (src == Null)) {											// Destination is an Null pointer? Source is an Null pointer?
		return;																		// Yes :(
	}
	
	PChar end = dest + StrGetLengthC(dest);											// Let's do it!
	
	StrCopyMemory(end, src, StrGetLengthC(src));									// Let's append the src to dest
	
	end += StrGetLengthC(src);														// And put an 0 (NUL) at the end
	*end = '\0';
}

PWChar StrDuplicate(PWChar str) {
	PWChar ret = (PWChar)MemAllocate((StrGetLength(str) + 1) * 4);
	
	if (ret == Null) {
		return Null;
	}
	
	return StrCopy(ret, str);
}

PChar StrDuplicateC(PChar str) {
	PChar ret = (PChar)MemAllocate(StrGetLengthC(str) + 1);
	
	if (ret == Null) {
		return Null;
	}
	
	return StrCopyC(ret, str);
}

PWChar StrTokenize(PWChar str, PWChar delim) {
	static PWChar temp = Null;
	
	if (str != Null) {																// First call?
		if (temp != Null) {															// Free the current temp?
			MemFree((UIntPtr)temp);													// Yup
		}
		
		temp = StrDuplicate(str);													// Yes, try to set the copy the str to temp
		
		if (temp == Null) {															// Failed?
			return Null;															// Yes
		}
	} else if (temp == Null) {														// Not the first call but temp is Null?
		return Null;																// Yes, so return Null
	} else {
		str = temp;
	}
	
	UIntPtr chars = 0;
	UIntPtr flag = 0;
	
	while (*temp) {
		for (PWChar d = delim; *d != '\0'; d++) {
			if (*temp == *d) {														// Found delim in the string?
				if (chars == 0) {													// First character in the string?
					flag = 1;														// Yes, there may be other after it, so go to the next one
					str++;
				} else {
					temp++;															// No, so we can return
					str[chars] = '\0';
					
					return str;
				}
			}
		}
		
		if (flag == 0) {															// Found delim?
			chars++;																// No, so increase chars
		}
		
		temp++;
		flag = 0;
	}
	
	temp = Null;
	str[chars] = '\0';
	str = StrDuplicate(str);														// *HACKHACKHACK*
	
	MemFree((UIntPtr)temp);
	
	return str;
}

PChar StrTokenizeC(PChar str, PChar delim) {
	static PChar temp = Null;
	
	if (str != Null) {																// First call?
		if (temp != Null) {															// Free the current temp?
			MemFree((UIntPtr)temp);													// Yup
		}
		
		temp = StrDuplicateC(str);													// Yes, try to set the copy the str to temp
		
		if (temp == Null) {															// Failed?
			return Null;															// Yes
		}
	} else if (temp == Null) {														// Not the first call but temp is Null?
		return Null;																// Yes, so return Null
	} else {
		str = temp;
	}
	
	UIntPtr chars = 0;
	UIntPtr flag = 0;
	
	while (*temp) {
		for (PChar d = delim; *d != '\0'; d++) {
			if (*temp == *d) {														// Found delim in the string?
				if (chars == 0) {													// First character in the string?
					flag = 1;														// Yes, there may be other after it, so go to the next one
					str++;
				} else {
					temp++;															// No, so we can return
					str[chars] = '\0';
					
					return str;
				}
			}
		}
		
		if (flag == 0) {															// Found delim?
			chars++;																// No, so increase chars
		}
		
		temp++;
		flag = 0;
	}
	
	temp = Null;
	str[chars] = '\0';
	str = StrDuplicateC(str);														// *HACKHACKHACK*
	
	MemFree((UIntPtr)temp);
	
	return str;
}

Void StrUnicodeFromC(PWChar dest, PChar src, UIntPtr len) {
	if ((dest == Null) || (src == Null) || (len == 0)) {							// Sanity checks
		return;
	}
	
	for (UIntPtr i = 0; i < len; i++) {												// Just copy :)
		dest[i] = src[i];
	}
	
	dest[len] = 0;
}
