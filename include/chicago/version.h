// File author is Ítalo Lima Marconato Matias
//
// Created on September 15 of 2018, at 13:12 BRT
// Last edited on December 09 of 2018, at 17:42 BRT

#ifndef __CHICAGO_VERSION_H__
#define __CHICAGO_VERSION_H__

#include <chicago/types.h>

#define CHICAGO_MAJOR 1
#define CHICAGO_MINOR 0
#define CHICAGO_BUILD 33
#define CHICAGO_CODENAME L"Cosmos"
#define CHICAGO_CODENAME_C "Cosmos"
#define CHICAGO_ARCH ARCH
#define CHICAGO_ARCH_C ARCH_C
#define CHICAGO_VSTR L"Version " TextifyMacro(CHICAGO_MAJOR) L"." TextifyMacro(CHICAGO_MINOR) L"." TextifyMacro(CHICAGO_BUILD)
#define CHICAGO_VSTR_C "Version " TextifyMacroC(CHICAGO_MAJOR) "." TextifyMacroC(CHICAGO_MINOR) "." TextifyMacroC(CHICAGO_BUILD)
#define CHICAGO_VSTR_LEN StrGetLength(CHICAGO_VSTR)
#define CHICAGO_VSTR_C_LEN StrGetLength(CHICAGO_VSTR_C)

#endif		// __CHICAGO_VERSION_H__
