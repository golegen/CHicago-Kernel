// File author is Ítalo Lima Marconato Matias
//
// Created on September 15 of 2018, at 13:12 BRT
// Last edited on November 16 of 2018, at 16:40 BRT

#ifndef __CHICAGO_VERSION_H__
#define __CHICAGO_VERSION_H__

#include <chicago/types.h>

#define CHICAGO_MAJOR 1
#define CHICAGO_MINOR 0
#define CHICAGO_BUILD 31
#define CHICAGO_CODENAME "Cosmos"
#define CHICAGO_ARCH ARCH
#define CHICAGO_VSTR "Version " TextifyMacro(CHICAGO_MAJOR) "." TextifyMacro(CHICAGO_MINOR) "." TextifyMacro(CHICAGO_BUILD)
#define CHICAGO_VSTR_LEN StrGetLength(CHICAGO_VSTR)

#endif		// __CHICAGO_VERSION_H__
