// File author is Ítalo Lima Marconato Matias
//
// Created on July 27 of 2018, at 14:42 BRT
// Last edited on December 09 of 2018, at 16:58 BRT

#ifndef __CHICAGO_PROCESS_H__
#define __CHICAGO_PROCESS_H__

#include <chicago/arch/process.h>
#include <chicago/alloc-int.h>
#include <chicago/file.h>
#include <chicago/queue.h>

#define PsLockTaskSwitch(i) Boolean i ## e = PsTaskSwitchEnabled; if (i ## e) PsTaskSwitchEnabled = False;
#define PsUnlockTaskSwitch(i) if (i ## e) PsTaskSwitchEnabled = True;
#define PsCurrentProcess (PsCurrentThread->parent)
#define PsDontRequeue ((PVoid)1)
#define PS_DEFAULT_QUANTUM 20

typedef Boolean Lock, *PLock;

typedef struct {
	UIntPtr id;
	PWChar name;
	UIntPtr dir;
	PList threads;
	UIntPtr last_tid;
	PAllocBlock alloc_base;
	UIntPtr mem_usage;
	PList handle_list;
	PList global_handle_list;
	PList files;
	IntPtr last_fid;
	PWChar exec_path;
} Process, *PProcess;

typedef struct {
	UIntPtr id;
	PContext ctx;
	UIntPtr quantum;
	PProcess parent;
} Thread, *PThread;

typedef struct {
	PFsNode file;
	IntPtr num;
} ProcessFile, *PProcessFile;

#ifndef __CHICAGO_PROCESS__
extern Boolean PsTaskSwitchEnabled;
extern PThread PsCurrentThread;
extern PQueue PsThreadQueue;
extern PList PsProcessList;
#endif

PThread PsCreateThreadInt(UIntPtr entry, UIntPtr userstack, Boolean user);
PProcess PsCreateProcessInt(PWChar name, UIntPtr entry, UIntPtr dir);
PThread PsCreateThread(UIntPtr entry, UIntPtr userstack, Boolean user);
PProcess PsCreateProcess(PWChar name, UIntPtr entry);
Void PsAddThread(PThread th);
Void PsAddProcess(PProcess proc);
PThread PsGetThread(UIntPtr id);
PProcess PsGetProcess(UIntPtr id);
Void PsExitThread(Void);
Void PsExitProcess(Void);
Void PsSleep(UIntPtr ms);
Void PsWaitThread(UIntPtr thid);
Void PsWaitProcess(UIntPtr pid);
Void PsLock(PLock lock);
Void PsUnlock(PLock lock);
PContext PsCreateContext(UIntPtr entry, UIntPtr userstack, Boolean user);
Void PsFreeContext(PContext context);
Void PsSwitchTask(PVoid priv);
Void PsInit(Void);

#endif		// __CHICAGO_PROCESS_H__
