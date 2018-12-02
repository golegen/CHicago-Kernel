// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 23:27 BRT
// Last edited on May 11 of 2018, at 23:59 BRT

#ifndef __CHICAGO_ARCH_SERIAL_H__
#define __CHICAGO_ARCH_SERIAL_H__

#define COM1_PORT 0x3F8				// COM ports
#define COM2_PORT 0x2F8
#define COM3_PORT 0x3E8
#define COM4_PORT 0x2E8

#include <chicago/types.h>

Void SerialInit(UInt16 port);		// Method to init an serial port (for debugging)

#endif								// __CHICAGO_ARCH_SERIAL_H__
