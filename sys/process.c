// File author is Ítalo Lima Marconato Matias
//
// Created on July 27 of 2018, at 14:59 BRT
// Last edited on November 17 of 2018, at 14:01 BRT

#define __CHICAGO_PROCESS__

#include <chicago/arch/process.h>

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/mm.h>
#include <chicago/panic.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/timer.h>
#include <chicago/list.h>
#include <chicago/queue.h>
#include <chicago/virt.h>

extern Void KernelMainLate(Void);

Boolean PsTaskSwitchEnabled = False;
PThread PsCurrentThread = Null;
PQueue PsThreadQueue = Null;
PList PsProcessList = Null;
UIntPtr PsNextID = 0;

PThread PsCreateThreadInt(UIntPtr entry, UIntPtr userstack, Boolean user) {
	if (user && (userstack == 0)) {																												// Valid user stack?
		return Null;																															// Nope
	}
	
	PThread th = (PThread)MemAllocate(sizeof(Thread));																							// Let's try to allocate the process struct!
	
	if (th == Null) {
		return Null;																															// Failed
	} else if ((th->ctx = PsCreateContext(entry, userstack, user)) == Null) {																	// Create the thread context
		MemFree((UIntPtr)th);																													// Failed
		return Null;
	}
	
	th->id = 0;																																	// We're going to set the id and the parent process later
	th->parent = Null;
	
	return th;
}

PProcess PsCreateProcessInt(PChar name, UIntPtr entry, UIntPtr dir) {
	PProcess proc = (PProcess)MemAllocate(sizeof(Process));																						// Let's try to allocate the process struct!
	
	if (proc == Null) {
		return Null;																															// Failed
	}
	
	proc->id = PsNextID++;																														// Use the next PID
	proc->name = StrDuplicate(name);																											// Duplicate the name
	
	if ((proc->dir = dir) == 0) {																												// Create an new page dir?
		proc->dir = MmCreateDirectory();																										// Yes
		
		if (proc->dir == 0) {
			MemFree((UIntPtr)proc->name);																										// Failed...
			MemFree((UIntPtr)proc);
			
			return Null;
		}
	}
	
	if ((proc->threads = ListNew(False, False)) == Null) {																						// Create the thread list
		MmFreeDirectory(proc->dir);																												// Failed...
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
	}
	
	proc->last_tid = 0;
	proc->alloc_base = Null;
	proc->mem_usage = 0;
	proc->handle_list = Null;
	proc->global_handle_list = Null;
	proc->files = ListNew(False, False);																										// Init our process file list
	proc->last_fid = 0;
	proc->exec_path = Null;
	
	if (proc->files == Null) {
		ListFree(proc->threads);																												// Failed...
		MmFreeDirectory(proc->dir);
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
		return Null;
	}
	
	PThread th = PsCreateThreadInt(entry, 0, False);																							// Create the first thread
	
	if (th == Null) {
		ListFree(proc->files);																													// Failed...
		ListFree(proc->threads);
		MmFreeDirectory(proc->dir);
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
		return Null;
	}
	
	th->id = proc->last_fid++;																													// Set the thread id
	th->parent = proc;																															// And the parent process
	
	if (!ListAdd(proc->threads, th)) {																											// Add it!
		PsFreeContext(th->ctx);																													// Failed...
		MemFree((UIntPtr)th);
		ListFree(proc->files);
		ListFree(proc->threads);
		MmFreeDirectory(proc->dir);
		MemFree((UIntPtr)proc->name);
		MemFree((UIntPtr)proc);
		return Null;
	}
	
	return proc;
}

PThread PsCreateThread(UIntPtr entry, UIntPtr userstack, Boolean user) {
	return PsCreateThreadInt(entry, userstack, user);																							// Use our PsCreateThreadInt function
}

PProcess PsCreateProcess(PChar name, UIntPtr entry) {
	return PsCreateProcessInt(name, entry, 0);																									// Use our PsCreateProcessInt function
}

Void PsAddThread(PThread th) {
	if ((th == Null) || (PsCurrentThread == Null) || (PsCurrentProcess == Null) || (PsThreadQueue == Null) || (PsProcessList == Null)) {		// Sanity checks
		return;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	th->id = PsCurrentProcess->last_tid++;																										// Set the ID
	th->parent = PsCurrentProcess;																												// Set the parent process
	
	QueueAdd(PsThreadQueue, th);																												// Try to add this thread to the queue
	ListAdd(PsCurrentProcess->threads, th);																										// Try to add this thread to the thread list of this process
	PsUnlockTaskSwitch(old);																													// Unlock
}

Void PsAddProcess(PProcess proc) {
	if ((proc == Null) || (proc->dir == 0) || (proc->threads == Null) || (PsThreadQueue == Null) || (PsProcessList == Null)) {					// Sanity checks
		return;
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	ListForeach(proc->threads, i) {																												// Add all the threads from this process to the thread queue
		QueueAdd(PsThreadQueue, (PThread)i->data);
	}
	
	ListAdd(PsProcessList, proc);																												// Try to add this process to the process list
	PsUnlockTaskSwitch(old);																													// Unlock
}

PThread PsGetThread(UIntPtr id) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null) || (PsProcessList == Null)) {													// Sanity checks
		return Null;
	}
	
	ListForeach(PsCurrentProcess->threads, i) {																									// Let's search!
		PThread th = (PThread)i->data;
		
		if (th->id == id) {																														// Match?
			return th;																															// Yes :)
		}
	}
	
	return Null;
}

PProcess PsGetProcess(UIntPtr id) {
	if (PsProcessList == Null) {																												// Sanity checks
		return Null;
	}
	
	ListForeach(PsProcessList, i) {																												// Let's search!
		PProcess proc = (PProcess)i->data;
		
		if (proc->id == id) {																													// Match?
			return proc;																														// Yes :)
		}
	}
	
	return Null;
}

Void PsSleep(UIntPtr ms) {
	if ((ms == 0) || (PsCurrentThread == Null) || (PsCurrentProcess == Null) || (PsProcessList == Null)) {										// Sanity checks
		return;
	}
	
	TimerSleepProcess(ms);																														// Sleep!
}

Void PsWaitThread(UIntPtr id) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null) || (PsProcessList == Null)) {													// Sanity checks
		return;
	}
	
	while (PsGetThread(id)) {																													// This isn't the right way to do it...
		PsSwitchTask(Null);
	}
}

Void PsWaitProcess(UIntPtr id) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null) || (PsProcessList == Null)) {													// Sanity checks
		return;
	}
	
	while (PsGetProcess(id)) {																													// This isn't the right way to do it...
		PsSwitchTask(Null);
	}
}

Void PsLock(PLock lock) {
	if ((lock == Null) || (PsCurrentProcess == Null)) {																							// Sanity checks
		return;
	}
	
	while (*lock) {																																// Just keep on checking!
		PsSwitchTask(Null);
	}
	
	*lock = True;
}

Void PsUnlock(PLock lock) {
	if ((lock == Null) || (PsCurrentProcess == Null)) {																							// Sanity checks
		return;
	}
	
	*lock = False;																																// Unlock...
}

Void PsExitThread(Void) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null) || (PsProcessList == Null)) {													// Sanity checks
		return;
	} else if ((PsCurrentThread->id == 0) && (PsCurrentProcess->id == 0)) {																		// Kernel main thread?
		PsLockTaskSwitch(old);																													// Yes, so PANIC!
		DbgWriteFormated("PANIC! Tried to close the kernel main thread\r\n");
		Panic(PANIC_KERNEL_UNEXPECTED_ERROR);
	} else if (PsCurrentProcess->threads->length == 1) {																						// We're the only thread?
		PsExitProcess();																														// Yes, so call PsExitProcess	
	}
	
	PsLockTaskSwitch(old);																														// Lock
	
	UIntPtr idx = 0;
	Boolean found = False;
	
	ListForeach(PsCurrentProcess->threads, i) {																									// Let's try to remove ourself from the thread list of this process
		if (i->data == PsCurrentThread) {																										// Found?
			found = True;																														// YES!
			break;
		} else {
			idx++;																																// nope
		}
	}
	
	if (found) {																																// Found?
		ListRemove(PsCurrentProcess->threads, idx);																								// Yes, remove it!
	}
	
	PsFreeContext(PsCurrentThread->ctx);																										// Free the context
	MemFree((UIntPtr)PsCurrentThread);																											// And the current thread itself
	
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(PsDontRequeue);																												// Switch to the next thread
	ArchHalt();																																	// Halt
}

Void PsExitProcess(Void) {
	if ((PsCurrentThread == Null) || (PsCurrentProcess == Null) || (PsProcessList == Null))  {													// Sanity checks
		return;
	} else if (PsCurrentProcess->id == 0) {																										// Kernel process?
		PsLockTaskSwitch(old);																													// Yes, so PANIC!
		DbgWriteFormated("PANIC! Tried to close the kernel process\r\n");
		Panic(PANIC_KERNEL_UNEXPECTED_ERROR);
	}
	
	PsLockTaskSwitch(old);																														// Lock
	MmSwitchDirectory(MmKernelDirectory);																										// Switch to the kernel directory
	ArchSwitchToKernelStack();																													// Switch to the kernel stack
	
	ListForeach(PsCurrentProcess->files, i) {																									// Close all the files that this process used
		PProcessFile pf = (PProcessFile)i->data;
		
		FsCloseFile(pf->file);
		MemFree((UIntPtr)pf);
	}
	
	ListForeach(PsCurrentProcess->threads, i) {																									// Let's free and remove all the threads
		PThread th = (PThread)i->data;
		
		if (th != PsCurrentThread) {																											// Remove from the queue?
			UIntPtr idx = 0;
			Boolean found = False;
			
			ListForeach(PsThreadQueue, j) {																										// Let's try to remove this thread from the thread queue
				if (j->data == th) {																											// Found?
					found = True;																												// YES!
					break;
				} else {
					idx++;																														// nope
				}
			}
			
			if (found) {																														// Found?
				ListRemove(PsThreadQueue, idx);																									// Yes, remove it!
			}
			
			PsFreeContext(th->ctx);																												// Free the context
			MemFree((UIntPtr)th);																												// And the thread struct itself
		}
	}
	
	UIntPtr idx = 0;
	Boolean found = False;
	
	ListForeach(PsProcessList, i) {																												// Let's try to remove ourself from the process list
		if (i->data == PsCurrentProcess) {																										// Found?
			found = True;																														// YES!
			break;
		} else {
			idx++;																																// nope
		}
	}
	
	if (found) {																																// Found?
		ListRemove(PsProcessList, idx);																											// Yes, remove it!
	}
	
	ListFree(PsCurrentProcess->files);																											// Free the file list
	MmFreeDirectory(PsCurrentProcess->dir);																										// Free the directory
	MemFree((UIntPtr)PsCurrentProcess->name);																									// Free the name
	MemFree((UIntPtr)PsCurrentProcess);																											// And the current process itself
//	PsFreeContext(PsCurrentThread->ctx);																										// BUG: When we try to free the current thread context, we get a page fault
	MemFree((UIntPtr)PsCurrentThread);																											// Free the thread struct
	
	PsUnlockTaskSwitch(old);																													// Unlock
	PsSwitchTask(PsDontRequeue);																												// Switch to the next process
	ArchHalt();																																	// Halt
}

Void PsInit(Void) {
	if ((PsThreadQueue = QueueNew(False)) == Null) {																							// Try to init the process queue
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	if ((PsProcessList = ListNew(False, False)) == Null) {																						// Try to init the process list
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	PProcess proc = PsCreateProcessInt("System", (UIntPtr)KernelMainLate, MmKernelDirectory);													// Try to create the system process
	
	if (proc == Null) {
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	if (!QueueAdd(PsThreadQueue, ListGet(proc->threads, 0))) {																					// Try to add it to the thread queue
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	if (!ListAdd(PsProcessList, proc)) {																										// And to the process list
		DbgWriteFormated("PANIC! Failed to init tasking\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	PsTaskSwitchEnabled = True;																													// Enable task switching
	PsSwitchTask(Null);																															// And switch to the system process!
}
