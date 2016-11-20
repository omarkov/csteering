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


#include "FANClasses.h"

void *FAN_Queue::pop()
{
	FAN_ENTER;
        pthread_mutex_lock(&mut);

	FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "First");

	FAN_Queue *first = next;
	void *ret = NULL;

	if(first != NULL)
	{
		if(last == first)
		{
			last = this;
		}else
		{
			next = first->next;
			first->next = last->next;
			last->next  = first;
		  	size --;
		}
		ret = first->data;
		first->data = NULL;
//		delete(first);
	}

	
        pthread_mutex_unlock(&mut);

	FAN_RETURN ret;
}

int FAN_Queue::getSize()
{
	FAN_ENTER;
	FAN_RETURN size;
}

FAN_Queue::FAN_Queue()
{
	this->size = 0;
	this->isRoot = true;
	this->data = NULL;
	this->next = NULL;
	this->last = this;
	pthread_mutex_init(&mut, NULL);
}

FAN_Queue::FAN_Queue(void *data)
{
	this->data = data;
	this->next = NULL;
	this->last = NULL;
	this->isRoot = false;
}

FAN_Queue::~FAN_Queue()
{

	if(isRoot)
	    pthread_mutex_destroy(&mut);

	if(next != NULL)
		delete next;
}

void FAN_Queue::push(void *data)
{
	FAN_ENTER;
        pthread_mutex_lock(&mut);

	FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Push");

	if(last->next == NULL)
	{
	    last->next = new FAN_Queue(data);
	}else
	{
	    last->next->data = data;
	}
	last = last->next;
	size ++;

        pthread_mutex_unlock(&mut);
	FAN_RETURN;
}
