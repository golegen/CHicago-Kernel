// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2018, at 21:15 BRT
// Last edited on December 09 of 2018, at 16:56 BRT

#ifndef __CHICAGO_EXEC_H__
#define __CHICAGO_EXEC_H__

#include <chicago/process.h>

typedef struct {
	PWChar name;
	UIntPtr loc;
} ExecSymbol, *PExecSymbol;

typedef struct {
	PWChar name;
	UIntPtr refs;
	UIntPtr base;
	Boolean resolved;
	PList symbols;
	PList deps;
} ExecHandle, *PExecHandle;

PProcess ExecCreateProcess(PWChar path);
PExecHandle ExecLoadLibrary(PWChar path, Boolean global);
Void ExecCloseLibrary(PExecHandle handle);
UIntPtr ExecGetSymbol(PExecHandle handle, PWChar name);

#endif		// __CHICAGO_EXEC_H__
