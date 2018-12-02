// File author is Ítalo Lima Marconato Matias
//
// Created on November 16 of 2018, at 01:04 BRT
// Last edited on November 16 of 2018, at 16:18 BRT

#ifndef __CHICAGO_SC_H__
#define __CHICAGO_SC_H__

#include <chicago/file.h>
#include <chicago/process.h>

typedef struct {
	PUInt32 major;
	PUInt32 minor;
	PUInt32 build;
	PChar codename;
	PChar arch;
} SystemVersion, *PSystemVersion;

Void ScSysGetVersion(PSystemVersion ver);
UIntPtr ScMmAllocMemory(UIntPtr size);
Void ScMmFreeMemory(UIntPtr block);
UIntPtr ScMmReallocMemory(UIntPtr block, UIntPtr size);
UIntPtr ScMmGetUsage(Void);
UIntPtr ScVirtAllocAddress(UIntPtr addr, UIntPtr size, UInt32 flags);
Boolean ScVirtFreeAddress(UIntPtr addr, UIntPtr size);
UInt32 ScVirtQueryProtection(UIntPtr addr);
Boolean ScVirtChangeProtection(UIntPtr addr, UIntPtr size, UInt32 flags);
UIntPtr ScVirtGetUsage(Void);
UIntPtr ScPsCreateThread(UIntPtr entry);
UIntPtr ScPsGetTID(Void);
UIntPtr ScPsGetPID(Void);
Void ScPsSleep(UIntPtr ms);
Void ScPsWaitThread(UIntPtr tid);
Void ScPsWaitProcess(UIntPtr pid);
Void ScPsLock(PLock lock);
Void ScPsUnlock(PLock lock);
Void ScPsExitThread(Void);
Void ScPsExitProcess(Void);
Void ScPsForceSwitch(Void);
IntPtr ScFsOpenFile(PChar path);
Void ScFsCloseFile(IntPtr file);
Boolean ScFsReadFile(IntPtr file, UIntPtr len, PUInt8 buf);
Boolean ScFsWriteFile(IntPtr file, UIntPtr len, PUInt8 buf);
Boolean ScFsMountFile(PChar path, PChar file, PChar type);
Boolean ScFsUmountFile(PChar path);
Boolean ScFsReadDirectoryEntry(IntPtr dir, UIntPtr entry, PChar out);
IntPtr ScFsFindInDirectory(IntPtr dir, PChar name);
Boolean ScFsCreateFile(IntPtr dir, PChar name, UIntPtr type);
Boolean ScFsControlFile(IntPtr file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf);
UIntPtr ScFsGetFileSize(IntPtr file);
UIntPtr ScFsGetPosition(IntPtr file);
Void ScFsSetPosition(IntPtr file, UIntPtr base, UIntPtr off);

#endif		// __CHICAGO_SC_H__
