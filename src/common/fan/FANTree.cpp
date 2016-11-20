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

void *FAN_Tree::search(unsigned long int key)
{
	FAN_ENTER;
        pthread_mutex_lock(&mut);

	FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Search : %l", key);
	
	void *ret = search(0, key);

        pthread_mutex_unlock(&mut);

	FAN_RETURN ret;
}

void *FAN_Tree::search(int offset, unsigned long int key)
{
	FAN_ENTER;
	if(key == this->key)
	{
		FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Found");
		return this->data;	
	}
	FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_NOCR, "Branch: ");
		
	if((key & (2^offset)) > 0)
	{
		FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_QUIET, "1");
			
		if(one == NULL)
		{
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Not Found");
			FAN_RETURN NULL;
		}else
		{	
			FAN_RETURN one->search(offset+1, key);	
		}
	}else
	{
		FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_QUIET, "0");

		if(zero == NULL)
		{
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Not Found");
			FAN_RETURN NULL;
		}else
		{
			FAN_RETURN zero->search(offset+1, key);	
		}
	}
}

FAN_Tree::FAN_Tree()
{
	this->key = 0;
	this->data = NULL;

	this->one = NULL;
	this->zero = NULL;
	this->isRoot = true;

	pthread_mutex_init(&mut, NULL);
}

FAN_Tree::FAN_Tree(unsigned long int key, void *data)
{
	this->key = key;
	this->data = data;

	this->one = NULL;
	this->zero = NULL;
	this->isRoot = false;
}

FAN_Tree::~FAN_Tree()
{
	if(data == NULL)
	    pthread_mutex_destroy(&mut);
		
	if(one != NULL)
		delete one;
	if(zero != NULL)
		delete zero;
}

int FAN_Tree::insert(unsigned long int key, void *data)
{
	FAN_ENTER;
        pthread_mutex_lock(&mut);

	FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Search Empty Slot for : %ul", key);
	
	int ret = insert(0, key, data);

        pthread_mutex_unlock(&mut);

	FAN_RETURN ret;
}

int FAN_Tree::insert(int offset, unsigned long int key, void *data)
{
	FAN_ENTER;
	if(this->key == 0) 
	{
		this->key  = key;
		this->data = data;
	}else
	{
		if((key & (2^offset)) > 0)
		{
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_QUIET, "1");
			
			if(one != NULL)
			{
				FAN_RETURN one->insert(offset+1, key, data);	
			}
			else
			{
				one = new FAN_Tree(key, data);
				FAN_RETURN 1;
			}
		}else
		{
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_QUIET, "0");

			if(zero != NULL)
			{
				FAN_RETURN zero->insert(offset+1, key, data);	
			}
			else
			{
				zero = new FAN_Tree(key, data);
				FAN_RETURN 1;
			}
		}
	}
	FAN_RETURN 0;
}

int FAN_Tree::del(unsigned long int key)
{
	FAN_ENTER;
        pthread_mutex_lock(&mut);

	FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Delete : %l", key);
	
	int ret = del(0, key);

        pthread_mutex_unlock(&mut);

	FAN_RETURN ret;
}

void *FAN_Tree::pop()
{
        pthread_mutex_lock(&mut);

	bool last = false;

	void *data =  _pop(&last);

        pthread_mutex_unlock(&mut);

	return data;
}

void *FAN_Tree::_pop(bool *last)
{
	void *ret = NULL;
	if(one != NULL)
	{
		ret =  one->_pop(last);
		if(*last)
		{
                   delete one;
		   one = NULL;
		   *last = false;
		}
		if(ret != NULL)
		{
		   return ret;
		}
	}
	if(zero != NULL)
	{
		ret = zero->_pop(last);
		if(*last)
		{
                   delete zero;
		   zero = NULL;
		   *last = false;
		}
		if(ret != NULL)
		{
		   return ret;
		}

	}
	this->key = 0;
	ret = this->data;
	this->data = NULL;
	*last = true;
	return ret;
}

int FAN_Tree::count()
{
	FAN_ENTER;
	int count = 0;
	if(this->key != 0)
		count++;
	if(one != NULL) 
		count+=one->count();
	if(zero != NULL) 
		count+=zero->count();
	FAN_RETURN count;
}

int FAN_Tree::del(int offset, unsigned long int key)
{
	FAN_ENTER;
	FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Before Delete");

	if(this->key == key)
	{
		FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_NOCR, "Delete ");
		this->key = 0;
		this->data = NULL;
		FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_QUIET | FAN_GREEN, "OK");
				
		FAN_RETURN 1;
	}else
	{
		FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_NOCR, "Branch: ");

		if((key & (2^offset)) > 0)
		{
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_QUIET, "1");
			
			if(one != NULL)
			{
				FAN_RETURN one->del(offset+1, key);
			}
		}else
		{
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL | FAN_QUIET, "0");

			if(zero != NULL)
			{
				FAN_RETURN zero->del(offset+1, key);
			}
		}
	}
	FAN_RETURN 0;
}
