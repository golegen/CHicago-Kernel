// File author is Ítalo Lima Marconato Matias
//
// Created on September 15 of 2018, at 13:12 BRT
// Last edited on April 20 of 2019, at 18:10 BRT

#ifndef __CHICAGO_VERSION_H__
#define __CHICAGO_VERSION_H__

#include <chicago/types.h>

#define CHICAGO_MAJOR 1
#define CHICAGO_MINOR 0
#define CHICAGO_BUILD 43
#define CHICAGO_CODENAME L"Cosmos"
#define CHICAGO_CODENAME_C "Cosmos"
#define CHICAGO_ARCH ARCH
#define CHICAGO_ARCH_C ARCH_C
#define CHICAGO_VSTR_C "Version " TextifyMacroC(CHICAGO_MAJOR) "." TextifyMacroC(CHICAGO_MINOR) "." TextifyMacroC(CHICAGO_BUILD)

#endif		// __CHICAGO_VERSION_H__
