// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2018, at 21:15 BRT
// Last edited on February 22 of 2019, at 20:44 BRT

#ifndef __CHICAGO_EXEC_H__
#define __CHICAGO_EXEC_H__

#include <chicago/process.h>

#define EXEC_INVALID_CHEXEC ((UIntPtr)-0xDEAD)

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
