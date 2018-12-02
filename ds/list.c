// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:08 BRT
// Last edited on November 10 of 2018, at 15:28 BRT

#include <chicago/alloc.h>
#include <chicago/list.h>
#include <chicago/mm.h>

static PVoid ListAllocMemory(UIntPtr size, Boolean user) {
	if (user) {
		return (PVoid)MmAllocUserMemory(size);
	} else {
		return (PVoid)MemAllocate(size);
	}
}

static Void ListFreeMemory(PVoid data, Boolean user) {
	if (user) {
		MmFreeUserMemory((UIntPtr)data);
	} else {
		MemFree((UIntPtr)data);
	}
}

PList ListNew(Boolean free, Boolean user) {
	PList out = ListAllocMemory(sizeof(List), user);								// Let's allocate the space for the initial list (without any entry)
	
	if (out == Null) {																// Failed?
		return Null;																// Yes...
	}
	
	out->head = Null;																// "Zero" everything
	out->tail = Null;
	out->length = 0;
	out->free = free;																// Except the "free", set it to what the user defined
	out->user = user;																// And except the "user"...
	
	return out;
}

Void ListFree(PList list) {
	PListNode cur;
	
	while (list->head != Null) {													// Let's free the list entries!
		cur = list->head;															// Save the cur list->head
		list->head = cur->next;														// Move to the next
		
		if (list->free) {															// We need to free this pointer?
			ListFreeMemory(cur->data, list->user);									// Yes
		}
		
		ListFreeMemory(cur, list->user);											// Free the list node
	}
	
	ListFreeMemory(list, list->user);												// Free the list itself
}

Boolean ListAddStart(PList list, PVoid data) {
	PListNode node = ListAllocMemory(sizeof(ListNode), list->user);					// Let's allocate a new list node!
	
	if (node == Null) {																// Failed?
		return False;																// Yes, so return False...
	}
	
	node->data = data;																// We don't need to copy the data
	
	if (list->length == 0) {														// This is the head and tail (the first entry)?
		node->next = Null;															// Yes!
	} else {
		node->next = list->head;													// No
		list->head->prev = node;
	}
	
	node->prev = list->head = node;
	list->length++;
	
	return True;
}

Boolean ListAdd(PList list, PVoid data) {
	PListNode node = ListAllocMemory(sizeof(ListNode), list->user);					// Let's allocate a new list node!
	
	if (node == Null) {																// Failed?
		return False;																// Yes, so return False...
	}
	
	node->data = data;																// We don't need to copy the data
	node->next = Null;
	
	if (list->length == 0) {														// This is the head and tail (the first entry)?
		list->head = list->tail = node;												// Yes!
		node->prev = node;
	} else {
		list->tail->next = node;													// No
		node->prev = list->tail;
		list->tail = node;
	}
	
	list->length++;
	
	return True;
}

PVoid ListRemove(PList list, UIntPtr idx) {
	if (idx >= list->length) {														// Too high idx?
		return Null;																// Yes, so we can't do anything
	}
	
	PListNode cur = list->head;
	PVoid data = Null;
	
	for (UInt32 i = 0; i < idx; i++) {												// Get the list node
		cur = cur->next;
	}
	
	data = cur->data;																// Save the data
	
	if (cur == list->head) {														// Was the list head?
		list->head = cur->next;														// Yup, so set the list head to the next entry (comparated to the node, of course)
	}
	if (cur == list->tail) {														// Was the list tail?
		list->tail = cur->prev;														// Yup, so set the list tail to the prev entry (comparated to the node, of course)
	}
	if (cur->prev != Null) {														// Had a prev?
		cur->prev->next = cur->next;												// Yes, so fix it
	}
	if (cur->next != Null) {														// Had a next?
		cur->next->prev = cur->prev;												// Yes, so fix it
	}
	
	ListFreeMemory(cur, list->user);												// Free the node
	
	list->length--;																	// Decrease the length
	
	return data;																	// And return the data!
}

PVoid ListGet(PList list, UInt32 idx) {
	if (idx >= list->length) {														// Too high idx?
		return Null;																// Yes, so we can't do anything
	}
	
	PListNode node = list->head;
	
	for (UInt32 i = 0; i < idx; i++) {												// Get the list node
		node = node->next;
	}
	
	return node->data;																// And return the data
}
