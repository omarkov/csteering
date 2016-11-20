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



#ifndef _FAN_TREE
#define _FAN_TREE

#include <pthread.h>

/**
 * Implementation of a thread-safe binary tree
 */
class FAN_Tree
{
	/**
	 * Searchs an entry in the tree 
	 * 
	 * @param offset Bit-shift
	 * @param key The key
	 */
	void *search(int offset, unsigned long int key);
	/**
	 * Inserts an entry into the tree 
	 * 
	 * @param offset Bit-shift
	 * @param key the Key
	 * @param data Pointer to data
	 */
	int insert(int offset, unsigned long int key, void *data);
	/**
	 * Deletes an entry from the tree 
	 * 
	 * @param offset Bit-shift
	 * @param key The key
	 */
	int del(int offset, unsigned long int key);

	void *_pop(bool *last);
	pthread_mutex_t mut;

public:
	FAN_Tree *one;
	FAN_Tree *zero;
	void *data;
	unsigned long int key;
	bool isRoot;

	/**
	 * Initializes a tree root node
	 */
	FAN_Tree();

	/**
	 * Initializes a tree-node with [key] and [data]
	 *
	 * @param key The key
	 * @param data The data
	 */
	FAN_Tree(unsigned long int key, void *data);
	/**
	 * Destructor
	 */
	~FAN_Tree();

	/**
	 * Searches an key in the tree and returns a pointer to the data.
	 *
	 * @param key The key
	 */
	void *search(unsigned long int key);
	/**
	 * Inserts a pointer int the tree.
	 *
	 * @param key The key
	 * @param data The value
	 */
	int insert(unsigned long int key, void *data);
	/**
	 * Deletes a node from the tree.
	 *
	 * @param key The key
	 */
	int del(unsigned long int key);
	/*
	 * @return The number of nodes in the tree
	 */
	int count();
	void *pop();
};

#endif
