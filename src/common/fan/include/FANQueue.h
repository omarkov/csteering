/*
 * FAN - Framework for Applications in Networks
 * Copyright (C) 2004 FreshX [dominik@freshx.de]
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef _FAN_QUEUE
#define _FAN_QUEUE

#include <pthread.h>

/**
 * Implementation of a thread-safe queue.
 */
class FAN_Queue
{
	/**
	 * Locking issues 
	 */
	pthread_mutex_t mut;
	/**
	 * current size of the queue
	 */
	int size;
public:
	/**
	 * Next queue element
	 */
	FAN_Queue *next;
	/**
	 * Pointer to the last used queue element
	 */
	FAN_Queue *last;
	/**
	 * Data
	 */
	void *data;
	/**
	 * Flag to identify the root
	 */
	bool isRoot;

	/**
	 * Initializes a queue-element.
	 *
	 * @param data The data
	 */
	FAN_Queue(void *data);

	/**
	 * Initializes an empty queue.
	 */
	FAN_Queue();

	/**
	 * Destructor
	 */
	~FAN_Queue();

	/**
	 * Pops and returns the first entry out of the queue.
	 *
	 * @return The first entry
	 */
	void *pop();
	/**
	 * Pushs an entry to the end of the queue.
	 *
	 * @param data the new entry
	 */
	void push(void *data);

	/**
	 * Pushs an entry to the end of the queue.
	 *
	 * @return stack size
	 */
	int getSize();
};

#endif
