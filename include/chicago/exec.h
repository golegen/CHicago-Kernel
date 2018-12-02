// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2018, at 21:15 BRT
// Last edited on November 17 of 2018, at 13:04 BRT

#ifndef __CHICAGO_EXEC_H__
#define __CHICAGO_EXEC_H__

#include <chicago/process.h>

typedef struct {
	PChar name;
	UIntPtr loc;
} ExecSymbol, *PExecSymbol;

typedef struct {
	PChar name;
	UIntPtr refs;
	UIntPtr base;
	Boolean resolved;
	PList symbols;
	PList deps;
} ExecHandle, *PExecHandle;

PProcess ExecCreateProcess(PChar path);
PExecHandle ExecLoadLibrary(PChar path, Boolean global);
Void ExecCloseLibrary(PExecHandle handle);
UIntPtr ExecGetSymbol(PExecHandle handle, PChar name);

#endif		// __CHICAGO_EXEC_H__
