// File author is Ítalo Lima Marconato Matias
//
// Created on November 16 of 2018, at 00:48 BRT
// Last edited on November 17 of 2018, at 13:27 BRT

#include <chicago/arch/idt.h>
#include <chicago/sc.h>

Void ArchScHandler(PRegisters regs) {
	switch (regs->eax) {
	case 0x00: {																						// Void SysGetVersion(PSystemRegisters regs)
		ScSysGetVersion((PSystemVersion)regs->ebx);
		break;
	}
	case 0x01: {																						// UIntPtr MmAllocMemory(UIntPtr size)
		regs->eax = ScMmAllocMemory(regs->ebx);
		break;
	}
	case 0x02: {																						// Void MmFreeMemory(UIntPtr block)
		ScMmFreeMemory(regs->ebx);
		break;
	}
	case 0x03: {																						// UIntPtr MmReallocMemory(UIntPtr block, UIntPtr size)
		regs->eax = ScMmReallocMemory(regs->ebx, regs->ecx);
		break;
	}
	case 0x04: {																						// UIntPtr MmGetUsage(Void)
		regs->eax = ScMmGetUsage();
		break;
	}
	case 0x05: {																						// UIntPtr VirtAllocAddress(UIntPtr addr, UIntPtr size, UInt32 flags)
		regs->eax = ScVirtAllocAddress(regs->ebx, regs->ecx, regs->edx);
		break;
	}
	case 0x06: {																						// Boolean VirtFreeAddress(UIntPtr addr, UIntPtr size)
		regs->eax = ScVirtFreeAddress(regs->ebx, regs->ecx);
		break;
	}
	case 0x07: {																						// UInt32 VirtQueryProtection(UIntPtr addr)
		regs->eax = ScVirtQueryProtection(regs->ebx);
		break;
	}
	case 0x08: {																						// Boolean VirtChangeProtection(UIntPtr addr, UIntPtr size, UInt32 flags)
		regs->eax = ScVirtChangeProtection(regs->ebx, regs->ecx, regs->edx);
		break;
	}
	case 0x09: {																						// UIntPtr VirtGetUsage(Void)
		regs->eax = ScVirtGetUsage();
		break;
	}
	case 0x0A: {																						// UIntPtr PsCreateThread(UIntPtr entry)
		regs->eax = ScPsCreateThread(regs->ebx);
		break;
	}
	case 0x0B: {																						// UIntPtr PsGetTID(Void)
		regs->eax = ScPsGetTID();
		break;
	}
	case 0x0C: {																						// UIntPtr PsGetPID(Void)
		regs->eax = ScPsGetPID();
		break;
	}
	case 0x0D: {																						// Void PsSleep(UIntPtr ms)
		ScPsSleep(regs->ebx);
		break;
	}
	case 0x0E: {																						// Void PsWaitThread(UIntPtr tid)
		ScPsWaitThread(regs->ebx);
		break;
	}
	case 0x0F: {																						// Void PsWaitProcess(UIntPtr pid)
		ScPsWaitProcess(regs->ebx);
		break;
	}
	case 0x10: {																						// Void PsLock(PLock lock)
		ScPsLock((PLock)regs->ebx);
		break;
	}
	case 0x11: {																						// Void PsUnlock(PLock lock)
		ScPsUnlock((PLock)regs->ebx);
		break;
	}
	case 0x12: {																						// Void PsExitThread(Void)
		ScPsExitThread();
		break;
	}
	case 0x13: {																						// Void PsExitProcess(Void)
		ScPsExitProcess();
		break;
	}
	case 0x14: {																						// Void PsForceSwitch(Void)
		ScPsForceSwitch();
		break;
	}
	case 0x15: {																						// IntPtr FsOpenFile(PChar path)
		regs->eax = ScFsOpenFile((PChar)regs->ebx);
		break;
	}
	case 0x16: {																						// Void FsCloseFile(IntPtr file)
		ScFsCloseFile(regs->ebx);
		break;
	}
	case 0x17: {																						// Boolean FsReadFile(IntPtr file, UIntPtr size, PUInt8 buf)
		regs->eax = ScFsReadFile(regs->ebx, regs->ecx, (PUInt8)regs->edx);
		break;
	}
	case 0x18: {																						// Boolean FsWriteFile(IntPtr file, UIntPtr size, PUInt8 buf)
		regs->eax = ScFsWriteFile(regs->ebx, regs->ecx, (PUInt8)regs->edx);
		break;
	}
	case 0x19: {																						// Boolean FsMountFile(PChar path, PChar file, PChar type)
		regs->eax = ScFsMountFile((PChar)regs->ebx, (PChar)regs->ecx, (PChar)regs->edx);
		break;
	}
	case 0x1A: {																						// Boolean FsUmountFile(PChar path)
		regs->eax = ScFsUmountFile((PChar)regs->ebx);
		break;
	}
	case 0x1B: {																						// Boolean FsReadDirectoryEntry(IntPtr dir, UIntPtr entry, PChar out)
		regs->eax = ScFsReadDirectoryEntry(regs->ebx, regs->ecx, (PChar)regs->edx);
		break;
	}
	case 0x1C: {																						// IntPtr FsFindInDirectory(IntPtr dir, PChar name)
		regs->eax = ScFsFindInDirectory(regs->ebx, (PChar)regs->ecx);
		break;
	}
	case 0x1D: {																						// Boolean FsCreateFile(IntPtr dir, PChar name, UIntPtr type)
		regs->eax = ScFsCreateFile(regs->ebx, (PChar)regs->ecx, regs->edx);
		break;
	}
	case 0x1E: {																						// Boolean FsControlFile(IntPtr file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf)
		regs->eax = ScFsControlFile(regs->ebx, regs->ecx, (PUInt8)regs->edx, (PUInt8)regs->esi);
		break;
	}
	case 0x1F: {																						// UIntPtr FsGetSize(IntPtr file)
		regs->eax = ScFsGetFileSize(regs->ebx);
		break;
	}
	case 0x20: {																						// UIntPtr FsGetPosition(IntPtr file)
		regs->eax = ScFsGetPosition(regs->ebx);
		break;
	}
	case 0x21: {																						// Boolean FsSetPosition(IntPtr file, IntPtr base, UIntPtr off)
		ScFsSetPosition(regs->ebx, regs->ecx, regs->edx);
		break;
	}
	default: {
		regs->eax = (UIntPtr)-1;
		break;
	}
	}
}

Void ArchInitSc(Void) {
	IDTRegisterInterruptHandler(0x3F, ArchScHandler);
}
