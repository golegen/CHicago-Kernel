// File author is Ítalo Lima Marconato Matias
//
// Created on November 16 of 2018, at 21:03 BRT
// Last edited on November 17 of 2018, at 13:19 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/chexec.h>
#include <chicago/exec.h>
#include <chicago/mm.h>
#include <chicago/string.h>
#include <chicago/virt.h>

static PChar ExecGetName(PChar path) {
	if (path == Null) {																								// Sanity check
		return Null;
	}
	
	PChar last = Null;
	PChar dup = StrDuplicate(path);
	PChar tok = StrTokenize(dup, "\\");
	
	while (tok != Null) {																							// Let's go!
		if (last != Null) {																							// Free the old last?
			MemFree((UIntPtr)last);																					// Yes
		}
		
		last = StrDuplicate(tok);																					// Duplicate the token
		
		if (last == Null) {
			MemFree((UIntPtr)dup);																					// Failed...
			return Null;
		}
		
		tok = StrTokenize(Null, "\\");																				// Tokenize next
	}
	
	MemFree((UIntPtr)dup);
	
	return last;
}

static Boolean ExecLoadDependencies(PUInt8 buf) {
	if (buf == Null) {																								// Sanity check
		return False;
	}
	
	PCHExecHeader hdr = (PCHExecHeader)buf;
	PCHExecDependency dep = (PCHExecDependency)(((UIntPtr)buf) + hdr->dep_start);
	
	for (UIntPtr i = 0; i < hdr->dep_count; i++) {																	// Let's do it!
		if (ExecLoadLibrary(dep->name, True) == Null) {																// Try to load this library
			return False;																							// Failed...
		}
		
		dep = (PCHExecDependency)(((UIntPtr)dep) + sizeof(CHExecDependency) + dep->name_len);
	}
	
	return True;
}

static Boolean ExecRelocate(UIntPtr base, PUInt8 buf) {
	if ((base == 0) || (buf == Null)) {																				// Sanity checks
		return False;
	}
	
	PCHExecHeader hdr = (PCHExecHeader)buf;
	PCHExecRelocation rel = (PCHExecRelocation)(((UIntPtr)buf) + hdr->rel_start);
	
	for (UIntPtr i = 0; i < hdr->rel_count; i++) {																	// Let's do it...
		PUIntPtr loc = (PUIntPtr)(base + rel->virt);
		
		if (rel->op == 0) {																							// Absolute
			*loc += base;																							// Just add the base to it!
		} else if (rel->op == 1) {																					// Symbol
			UIntPtr sym = ExecGetSymbol(Null, rel->name);															// Try to get the symbol
			
			if (sym == 0) {
				return False;																						// Failed
			}
			
			*loc = sym;
		} else if (rel->op == 2) {																					// Relative symbol
			UIntPtr sym = ExecGetSymbol(Null, rel->name);															// Try to get the symbol
			
			if (sym == 0) {
				return False;																						// Failed
			}
			
			*loc += sym - (UIntPtr)loc;
		} else if (rel->op == 3) {																					// Symbol as base
			UIntPtr sym = ExecGetSymbol(Null, rel->name);															// Try to get the symbol
			
			if (sym == 0) {
				return False;																						// Failed
			}
			
			*loc += sym;
		} else {
			return False;																							// Unimplemented relocation...
		}
		
		rel = (PCHExecRelocation)(((UIntPtr)rel) + sizeof(CHExecRelocation) + rel->name_len);
	}
	
	return True;
}

static Void ExecCreateProcessInt(Void) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null) || (PsCurrentProcess->exec_path == Null)) {			// Sanity checks
		PsExitProcess();
	}
	
	UIntPtr stack = VirtAllocAddress(0, 0x8000, VIRT_PROT_READ | VIRT_PROT_WRITE | VIRT_FLAGS_HIGHEST);				// Alloc the stack
	
	if (stack == 0) {
		MemFree((UIntPtr)PsCurrentProcess->exec_path);																// Failed...
		PsExitProcess();
	}
	
	PFsNode file = FsOpenFile(PsCurrentProcess->exec_path);															// Try to open the executable
	
	MemFree((UIntPtr)PsCurrentProcess->exec_path);																	// Free the executable path
	
	if (file == Null) {
		VirtFreeAddress(stack, 0x8000);																				// Failed to open it
		PsExitProcess();
	}
	
	PUInt8 buf = (PUInt8)MemAllocate(file->length);																	// Try to alloc the buffer to read it
	
	if (buf == Null) {
		FsCloseFile(file);																							// Failed
		VirtFreeAddress(stack, 0x8000);
		PsExitProcess();
	} else if (!FsReadFile(file, 0, file->length, buf)) {															// Read it!
		MemFree((UIntPtr)buf);																						// Failed...
		FsCloseFile(file);
		VirtFreeAddress(stack, 0x8000);
		PsExitProcess();
	}
	
	FsCloseFile(file);																								// Ok, now we can close the file!
	
	if (!CHExecValidateHeader(buf, True)) {																			// Check if this is a valid CHExec executable
		MemFree((UIntPtr)buf);																						// ...
		VirtFreeAddress(stack, 0x8000);
		PsExitProcess();
	}
	
	UIntPtr base = CHExecLoadSections(buf);																			// Load the sections into the memory
	UIntPtr entry = base + ((PCHExecHeader)buf)->entry;																// Calculate the base address
	
	if (base == 0) {
		MemFree((UIntPtr)buf);																						// Failed to load the sections...
		VirtFreeAddress(stack, 0x8000);
		PsExitProcess();
	} else if (!ExecLoadDependencies(buf)) {																		// Load the dependencies
		MmFreeUserMemory(base);
		MemFree((UIntPtr)buf);
		VirtFreeAddress(stack, 0x8000);
		PsExitProcess();
	} else if (!ExecRelocate(base, buf)) {																			// And relocate!
		MmFreeUserMemory(base);
		MemFree((UIntPtr)buf);
		VirtFreeAddress(stack, 0x8000);
		PsExitProcess();
	}
	
	MemFree((UIntPtr)buf);																							// Free the buffer
	ArchUserJump(entry, stack + 0x8000);																			// Jump!
	MmFreeUserMemory(base);																							// If it returns (somehow), free the sections
	VirtFreeAddress(stack, 0x8000);																					// Free the stack
	PsExitProcess();																								// And exit
}

PProcess ExecCreateProcess(PChar path) {
	if (path == Null) {																								// Check if wew have a valid path
		return Null;
	}
	
	PChar name = ExecGetName(path);																					// Try to extract our module name
	
	if (name == Null) {
		return Null;																								// Failed
	}
	
	PChar pathh = StrDuplicate(path);																				// Let's duplicate the path
	
	if (pathh == Null) {
		MemFree((UIntPtr)name);
		return Null;
	}
	
	PFsNode file = FsOpenFile(path);																				// Let's check if the file exists
	
	if (file == Null) {
		MemFree((UIntPtr)pathh);																					// Nope, so we can't continue
		MemFree((UIntPtr)name);
		return Null;
	}
	
	PProcess proc = PsCreateProcess(name, (UIntPtr)ExecCreateProcessInt);											// Create the process pointing to the ExecCreateProcessInt function
	
	MemFree((UIntPtr)name);																							// Free the name, we don't need it anymore
	
	if (proc == Null) {
		MemFree((UIntPtr)pathh);																					// Failed to create the process :(
		return Null;
	}
	
	proc->exec_path = pathh;
	
	return proc;
}
