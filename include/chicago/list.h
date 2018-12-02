// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:10 BRT
// Last edited on October 14 of 2018, at 11:20 BRT

#ifndef __CHICAGO_LIST_H__
#define __CHICAGO_LIST_H__

#include <chicago/types.h>

#define ListForeach(l, i) for (PListNode i = (l)->head; i != Null; i = i->next)

typedef struct ListNodeStruct {
	PVoid data;
	struct ListNodeStruct *next;
	struct ListNodeStruct *prev;
} ListNode, *PListNode;

typedef struct {
	PListNode head;
	PListNode tail;
	UIntPtr length;
	Boolean free;
	Boolean user;
} List, *PList;

PList ListNew(Boolean free, Boolean user);
Void ListFree(PList list);
Boolean ListAdd(PList list, PVoid data);
Boolean ListAddStart(PList list, PVoid data);
PVoid ListRemove(PList list, UInt32 idx);
PVoid ListGet(PList list, UInt32 idx);

#endif		// __CHICAGO_LIST_H__
