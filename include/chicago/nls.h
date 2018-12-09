// File author is Ítalo Lima Marconato Matias
//
// Created on December 09 of 2018, at 19:26 BRT
// Last edited on December 09 of 2018, at 20:40 BRT

#ifndef __CHICAGO_NLS_H__
#define __CHICAGO_NLS_H__

#include <chicago/types.h>

#define NLS_LANG_EN 0x00
#define NLS_LANG_BR 0x01

#define NLS_PANIC_SORRY 0x00
#define NLS_PANIC_ERRCODE 0x01
#define NLS_OS_NAME 0x02
#define NLS_OS_CODENAME 0x03
#define NLS_OS_VSTR 0x04
#define NLS_SHELL_HELP 0x05
#define NLS_SHELL_LANG_LIST 0x06
#define NLS_SHELL_LANG_INVALID 0x07
#define NLS_SHELL_PS_UNAMED 0x08
#define NLS_SHELL_PS 0x09
#define NLS_SHELL_INVALID 0x0A

PWChar NlsGetMessage(UIntPtr msg);
PWChar NlsGetLanguages(Void);
Void NlsSetLanguage(UIntPtr lang);
UIntPtr NlsGetLanguage(PWChar lang);

#endif		// __CHICAGO_NLS_H__
