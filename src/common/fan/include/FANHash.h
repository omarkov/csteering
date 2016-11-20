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



#ifndef _FAN_HASH
#define _FAN_HASH

#include <iostream>
#include <string.h>  
using namespace std; 

#define FAN_BEFORE_KEY   0x0
#define FAN_KEY          0x1
#define FAN_AFTER_KEY    0x2
#define FAN_BEFORE_VALUE 0x4
#define FAN_VALUE        0x8
#define FAN_EXT_VALUE    0x10
#define FAN_QUOTE        0x20
#define FAN_AFTER_VALUE  0x40
#define FAN_COMMENT      0x80
#define FAN_SECTION      0x100
#define FAN_EXT_KEY      0x200
#define FAN_KEY_QUOTE    0x400

/**
 * Internal structure for key iteration
 */
typedef struct
{
	char **key;
}FAN_keys;

/**
 * Implementation of a hash (<b>NOT</b> thread-safe!)
 *
 * <i>Note:</i> Keys are case-insensitive.
 */
class FAN_Hash
{
	pthread_mutex_t mut;
	pthread_mutexattr_t attributes;
	/**
	 * the string hashtable
	 */
	GHashTable *hash;
	/**
	 * the pointer hashtable
	 */
	GHashTable *pointerHash;
	char *m_line;
	char *m_buffer;

public:	

	/**
	 * Constructor
	 *
	 * @returns an instance of an empty hash.
	 */
	FAN_Hash();

	/**
	 * Internal Constructor (TRACE)
	 *
	 * @returns an instance of an empty hash.
	 */
	FAN_Hash(bool tb);
	/**
	 * Destructor
	 */
	~FAN_Hash();

	void lock();
	void unlock();


	/**
	 * Clears the hash.
	 */
	void clear();
	/**
	 * Copies all pointers of the hash into an initialized destination hash.
	 * Can be used to merge hashes of objects. Be aware of dead 
	 * references and double free.
	 *
	 * @param des reference to an initialized destination hash.
	 * @see copyValues
	 */
	void copyPointers(FAN_Hash *des);
	/**
	 * Copies all values of the hash into an initialized destination hash.
	 *
	 * @param des reference to an initialized destination hash.
	 * @see copyPointers
	 */
	void copyValues(FAN_Hash *des);
	/**
	 * Dumps a string representation of the hash to standard out.
	 */
	void dump();
	/**
	 * Allocates a string and dumps the string representation of the hash into it.
	 * <i>Note:</i> String has to be freed by the caller.
	 * 
	 * @param string reference to an unallocated string.
	 *
	 * <b>Example:</b>
	 * <pre>
	 * KEY1="value1"\\n
	 * KEY2="value2"\\n
	 * EOF</pre>
	 * @see dump
	 */
	void adump(char **string);
	/**
	 * Dumps a string representation of the hash to a file or stream.
	 *
	 * @param fd file descriptor
	 *
	 * @see dump
	 */
	void dump(int fd);
	/**
	 * Inserts, updates or deletes an entry.
	 * <i>Note:</i> The inserted value is being copied and may be freed afterwards.
	 * 
	 * @param key the case-insensitive Key
	 * @param value Value (may be NULL to delete an entry)
	 * 
	 * <b>Example: (delete)</b>
	 * <pre>
	 * insert(key, NULL)</pre>
	 * @see insertPointer
	 */
	void insert(char *key, char *value);
	/**
	 * Internal insert function (TRACE)
	 * @see insert
	 */
	void insertNoTB(char *key, char *value);
	/**
	 * Inserts or updates an entry.
	 * 
	 * @param key the case-insensitive Key
	 * @param value Value
	 * 
	 * @see insert
	 */
	void insertPointer(char *key, void *value);
	/**
	 * Removes a Pointer from the hash without freeing it.
	 *
	 * @param key the case-insensitive Key
	 */
	void removePointer(char *key);
	/**
	 * Extracts all keys from the hash and stores the key pointers into the pre-allocated 
	 * char-array [plist].
	 *
	 * @param plist pre-allocated char-array [size: Hash->getLength()*sizeof(char*)].
	 *
	 * <b>Example:</b>
	 * <pre>
	 * int i,size;
	 * char **list;
	 *
	 * size = getLength();
	 * list = (char**)malloc(size*sizeof(char*));
         *
	 * getKeys(list);
	 *
	 * for(i=0; i<size; i++)
	 * {
	 * 	FAN_xlog(FAN_INFO, "key #%d: %s", i+1, *(list+i));
	 * }
	 *
	 * free(list);
	 * </pre>
	 *
	 * This example lists the keys of the hash.
	 */
	void getKeys(char **plist);
	/**
	 * Extracts all keys from the pointer-hash and stores the key pointers into the pre-allocated 
	 * char-array [plist].
	 *
	 * @param plist pre-allocated char-array [size: Hash->getLength()*sizeof(char*)].
	 *
	 * <b>Example:</b>
	 * <pre>
	 * int i,size;
	 * char **list;
	 *
	 * size = getLength();
	 * list = (char**)malloc(size*sizeof(char*));
         *
	 * getKeys(list);
	 *
	 * for(i=0; i<size; i++)
	 * {
	 * 	FAN_xlog(FAN_INFO, "key #%d: %s", i+1, *(list+i));
	 * }
	 *
	 * free(list);
	 * </pre>
	 *
	 * This example lists the keys of the pointer-hash.
	 */
	void getPointerKeys(char **plist);
	/**
	 * Returns a reference to the corresponding string value for [key] from the hash.
	 *
	 * @param key the case-insensitive Key
	 * @returns a reference to the corresponding string value
	 * 
	 * @see getPointer
	 */
	char *getValue(char *key);
	/**
	 * Returns a reference to the corresponding string value for [key] from the hash.
	 * If the key does not exist it returns the default value [def].
	 *
	 * @param key case-insensitive Key
	 * @param def default value
	 * @returns a reference to the corresponding string value
	 * 
	 * @see getPointer
	 */
	char *getValue(char *key, char *def);
	/**
	 * Internal getValue function (TRACE)
	 * @see getValue
	 */
	char *getValueNoTB(char *key);
	/**
	 * Returns a reference to the corresponding value for [key] from the hash.
	 *
	 * @param key the case-insensitive Key
	 * @returns a reference to the corresponding value
	 *
	 * @see getValue
	 */
	void *getPointer(char *key);
	/**
	 * Returns the length of the hash.
	 *
	 * @returns length of the hash
	 */
	int  getLength();
	/**
	 * Returns the length of the pointer hash.
	 *
	 * @returns length of the pointer hash
	 */
	int  getPointerLength();
	/**
	 * Checks whether a given value matches the corresponding value of a given key. 
	 *
	 * @param key the case-insensitive Key
	 * @param value Value
	 *
	 * @returns True if the supplied value matches the value corresponding to the given key 
	 * and False otherwise.
	 */
	bool checkKey(char *key, char *value);
	/**
	 * Reads entries from a file and inserts them into the hash.
	 *
	 * @param file File path
	 * @returns True if successful, False otherwise
	 */
	bool readFromFile (char *file);
	//bool readFromSocket (char *file);
	/**
	 * Reads entries from a socket connection and inserts them into the hash.
	 *
	 * @param hostname Hostname 
	 * @param port Port 
	 * @param user Username 
	 * @param pwd User password 
	 * @returns True if successful, False otherwise
	 */
	bool readFromSocket (char *hostname, int port, char *user, char *pwd);
	/**
	 * Reads entries from a socket connection and inserts them into the hash.
	 * User and password are taken from the hash.
	 * Keys:
	 *
	 * User - "sdduser"
	 * Pass - "sddpass"
	 *
	 * @param hostname Hostname 
	 * @param port Port 
	 * @returns True if successful, False otherwise
	 */
	bool readFromSocket (char *hostname, int port);
	/**
	 * Reads entries from a string and inserts them into the hash.
	 *
	 * @param line \\0-terminated string
	 * @param decode boolean (0=none; 1=Base64)
	 * @returns True if successful, False otherwise
	 */
	bool readFromString(char *line, int decode);
	/**
	 * Reads entries from a stream and inserts them into the hash.
	 *
	 * @param FH file/stream handle 
	 * @param decode boolean (0=none; 1=Base64)
	 * @param file not used 
	 * @returns True if successful, False otherwise
	 */
	bool readFromStream(int FH, int decode, bool file);
	bool readFromFileStream(int FH, int decode, bool file);
};

#endif
