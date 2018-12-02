// File author is Ítalo Lima Marconato Matias
//
// Created on October 27 of 2018, at 18:28 BRT
// Last edited on October 27 of 2018, at 18:32 BRT

#include <chicago/queue.h>

PQueue QueueNew(Boolean user) {
	return ListNew(False, user);						// Redirect to ListNew
}

Void QueueFree(PQueue queue) {
	ListFree(queue);									// Redirect to ListFree
}

Boolean QueueAdd(PQueue queue, PVoid data) {	
	return ListAddStart(queue, data);					// Redirect to ListAddStart
}

PVoid QueueRemove(PQueue queue) {
	return ListRemove(queue, queue->length - 1);		// Redirect to ListRemove
}
