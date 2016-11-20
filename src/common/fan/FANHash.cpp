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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "FANClasses.h"

#define MAXLINE          1000

void FAN_Hash::lock()
{
        pthread_mutex_lock(&mut);
}

void FAN_Hash::unlock()
{
        pthread_mutex_unlock(&mut);
}

FAN_Hash::FAN_Hash()
{
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init(&attributes);
#if defined(__IRIX__) || defined(__DARWIN_OSX__)
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
#else
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
	pthread_mutex_init(&mut, &attributes);

	hash = NULL;
	pointerHash = NULL;
        m_line = new char[MAXLINE];
        m_buffer = new char[MAXLINE];

	hash = g_hash_table_new(
		g_str_hash, 
		g_str_equal);

	pointerHash = g_hash_table_new(
		g_str_hash, 
		g_str_equal);

}

FAN_Hash::FAN_Hash(bool tb)
{
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init(&attributes);
#if defined(__IRIX__) || defined(__DARWIN_OSX__)
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
#else
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
	pthread_mutex_init(&mut, &attributes);

	if(tb)
	{
		FAN_ENTER;
	}
	hash = NULL;
	pointerHash = NULL;
        m_line = new char[MAXLINE];
        m_buffer = new char[MAXLINE];

	hash = g_hash_table_new(
		g_str_hash, 
		g_str_equal);

	pointerHash = g_hash_table_new(
		g_str_hash, 
		g_str_equal);

	if(tb)
	{
		FAN_RETURN;
	}else
	{
		return;
	}
}

FAN_Hash::~FAN_Hash()
{
	if(hash != NULL)
	{
		clear();
		g_hash_table_destroy(hash);
		g_hash_table_destroy(pointerHash);
	}
	ZAP_ARRAY(m_line);
	ZAP_ARRAY(m_buffer);
	pthread_mutex_destroy(&mut);
	pthread_mutexattr_destroy(&attributes);
}
void FAN_Hash::clear ()
{
	FAN_ENTER;
	int i,size;
        char **list;
	lock();
        size = getLength();
        list = (char**)malloc(size*sizeof(char*));

    	getKeys(list);

	for(i=0; i < size; i++)
	{
		char *key   = *(list+i);
		insert(key, NULL);
	}

	free(list);

        size = getPointerLength();
        list = (char**)malloc(size*sizeof(char*));

    	getPointerKeys(list);

	for(i=0; i < size; i++)
	{
		char *key   = *(list+i);
		insertPointer(key, NULL);
	}

	free(list);
	unlock();
	FAN_RETURN;
}

void foreachHashKey (void *pkey, void *pvalue, void *plist)
{
	FAN_ENTER;
	FAN_keys *list = (FAN_keys*)plist;
        *(list->key) = (char*)pkey;

        list->key ++;
	FAN_RETURN;
}

void FAN_Hash::copyPointers(FAN_Hash *des)
{
    FAN_ENTER;
    int i,size;
    char **list;
    lock();

    size = getPointerLength();
    list = (char**)malloc(size*sizeof(char*));

    getPointerKeys(list);

    for(i=0; i<size; i++)
    {
	    des->insertPointer(*(list+i), getPointer(*(list+i)));
    }
    unlock();
    
    free(list);
    FAN_RETURN;
}

void FAN_Hash::copyValues(FAN_Hash *des)
{
    FAN_ENTER;
    int i,size;
    char **list;
    lock();
    size = getLength();
    list = (char**)malloc(size*sizeof(char*));

    getKeys(list);

    for(i=0; i<size; i++)
    {
	    char *key = list[i];
	    char *val = getValue(list[i]);

	    if(val != NULL)
	    {
	    	des->insert(key, val);
	    }
    }
    
    free(list);
    unlock();
    FAN_RETURN;
}

void FAN_Hash::dump()
{	
        FAN_ENTER;	
	dump(0);
	FAN_RETURN;
}

void FAN_Hash::adump(char **string)
{
        FAN_ENTER;	
        int i,size;
        char **list;

	lock();
        size = getLength();
        list = (char**)malloc(size*sizeof(char*));

	char *APPEND = "%s%s%s%s%s%s";

	char *pstring = strdup(APPEND);

        getKeys(list);
        for(i=0; i<size; i++)
        {
		char *p = pstring;
		asprintf(&pstring, pstring, "\"",*(list+i), "\"=\"", getValue(*(list+i)), "\"\n", APPEND);
		free(p);
        }
        free(list);

	char *p = pstring;
	asprintf(&pstring, pstring, "", "", "", "", "", "");
	free(p);

	*string = pstring;
	unlock();
	FAN_RETURN;
}

void FAN_Hash::dump(int fd)	// dumpHash to filedescriptor / socket
{
        FAN_ENTER;	
        int i,size,lvlbak;
        char **list;
	lock();

        if(!fd){ fd=STDOUT_FILENO;}	// set stdout if nuffin is defined

        size = getLength();
        list = (char**)malloc(size*sizeof(char*));

        getKeys(list);
        for(i=0; i<size; i++)
        {
		FAN *app = FAN::app;
		lvlbak=app->DEBUG_LVL;
		app->DEBUG_LVL=0;
                FAN_swrite(fd,*(list+i));
                FAN_swrite(fd,"=\"");
                FAN_swrite(fd,getValue(*(list+i)));
                FAN_swrite(fd,"\"\n");
		app->DEBUG_LVL=lvlbak;
        }
        free(list);
	unlock();
	FAN_RETURN;
}

void FAN_Hash::removePointer(char *key)
{
	FAN_ENTER;	
        char *ukey;
	char *orig_key;
	char *v;
	lock();

        ukey = strdup(key);
        FAN_upperCase (ukey);

	bool ret = g_hash_table_lookup_extended(pointerHash,
                                              	ukey,
                                              	(void**)&orig_key,
                                             	(void**)&v);


	g_hash_table_remove(pointerHash, ukey);
	free(ukey);

	if(ret)
	{
		free(orig_key);
		free(v);
	}	
	unlock();
	FAN_RETURN;
}

void FAN_Hash::insert(char *key, char *value)
{
	FAN_ENTER;
	insertNoTB(key, value);
	FAN_RETURN;
}

void FAN_Hash::insertNoTB(char *key, char *value)
{
        char *ukey;
	char *orig_key = NULL;
	char *v        = NULL;
	lock();

        ukey = strdup(key);
        FAN_upperCase (ukey);
	bool ret = false;
	if((ret = g_hash_table_lookup_extended(hash,
                                         	ukey,
                                              	(void**)&orig_key,
                                             	(void**)&v)))
	{
		free(ukey);
  		ukey = orig_key;
	}

        if(value != NULL)
	{
		char *value2 = strdup(value);
	        g_hash_table_insert(hash, ukey, value2);
	}else if(ret)
	{
	        g_hash_table_remove(hash, ukey);
	}

	if(ret)
		free(v);

	if(value == NULL)
		free(ukey);
	unlock();
}

void FAN_Hash::insertPointer(char *key, void *value)
{
    FAN_ENTER;	
    char *ukey;
	char *v = NULL;
	char *orig_key = NULL;
	lock();

    ukey = strdup(key);
    FAN_upperCase (ukey);

	bool ret = false;

	if((ret = g_hash_table_lookup_extended(pointerHash,
                                              	ukey,
                                              	(void**)&orig_key,
                                             	(void**)&v)))
	{
        free(ukey);
        ukey = orig_key;
	}


	if(value != NULL)
        g_hash_table_insert(pointerHash, ukey, value);
	else if(ret)
        g_hash_table_remove(pointerHash, ukey);

	if(value == NULL)
		free(ukey);

	unlock();

	FAN_RETURN;
}

void FAN_Hash::getKeys(char **plist)
{
        FAN_ENTER;	
        FAN_keys list;

       	list.key = plist;

	lock();
       	g_hash_table_foreach(hash, (GHFunc)foreachHashKey, &list);
	unlock();

	FAN_RETURN;
}

void FAN_Hash::getPointerKeys(char **plist)
{
        FAN_ENTER;	
        FAN_keys list;

        list.key = plist;

	lock();
        g_hash_table_foreach(pointerHash, (GHFunc)foreachHashKey, &list);
	unlock();
	FAN_RETURN;
}

void *FAN_Hash::getPointer (char *key)
{
        FAN_ENTER;
        char *ukey;
	char *orig_key = NULL;
	void *value = NULL;

        ukey = strdup(key);
        FAN_upperCase (ukey);

	lock();
	bool ret = g_hash_table_lookup_extended (pointerHash, ukey, (void**)&orig_key, (void**)&value);
	unlock();

        free(ukey);

        if (!ret || (ret && value == NULL))
        {
                FAN_xlog(FAN_DEBUG | FAN_HASH, "Couldn't find value for key %s", key);
                FAN_RETURN NULL;
        } else {
                FAN_RETURN value;
        }
}

char *FAN_Hash::getValue (char *key, char *def)
{
        FAN_ENTER;	
	char *ret =  getValueNoTB(key);
	if(ret == NULL)
	{
		ret = def;
	}
	FAN_RETURN ret;
}

char *FAN_Hash::getValue (char *key)
{
        FAN_ENTER;	
	FAN_RETURN getValueNoTB(key);
}

char *FAN_Hash::getValueNoTB (char *key)
{
	if(key == NULL)
	{
		return NULL;
	}
	lock();

        char *ukey = NULL;
        char *value = NULL;
	char *orig_key = NULL;
        ukey = strdup(key);
        FAN_upperCase (ukey);

	bool ret = g_hash_table_lookup_extended (hash, ukey, (void**)&orig_key, (void**)&value);
	unlock();


        if (!ret || (ret && value == NULL))
        {
                FAN_xlog(FAN_DEBUG | FAN_HASH, "Couldn't find value for key %s", key);
        	free(ukey);
                return NULL;
        } else {
        	free(ukey);
                return value;
        }
}

int FAN_Hash::getLength()
{
        FAN_ENTER;	
	int ret = 0;

	if(hash != NULL)
	{
		lock();
		ret = g_hash_table_size(hash);
		unlock();
	}
	FAN_RETURN ret;
}

int FAN_Hash::getPointerLength()
{
        FAN_ENTER;	
	int ret = 0;

	if(hash != NULL)
	{
		lock();
		ret = g_hash_table_size(pointerHash);
		unlock();
	}
	FAN_RETURN ret;
}

bool FAN_Hash::checkKey(char *key, char *value)
{
        FAN_ENTER;	
        int  ret;
	
	if(hash == NULL)
	{
		FAN_xlog(FAN_ERROR, "Hash does not exists");
		FAN_RETURN false;
	}
	char *val = getValue(key, "false");
	if(val != NULL && FAN::app)
        	FAN::app->errorNo = ret = strcasecmp(val, value);
	else
		ret = -999;

	if(ret != 0)
	{
		FAN_RETURN false; // FALSE
	}else
	{
		FAN_RETURN true; // TRUE
	}	
}

bool FAN_Hash::readFromFile (char *file)
{
        FAN_ENTER;	

        int FH;

        if((FH=open(file, O_RDONLY))<= 0) /* Open the file to scan */
        {
                FAN_xlog(FAN_ERROR, "Could not open file %s, %s", file, strerror(errno));

                FAN_RETURN false;
        }else
        {
               FAN_RETURN readFromFileStream(FH, 0, true);
        }
}

//bool FAN_Hash::readFromSocket (char *hostname)
//{
//	FAN_RETURN readFromSocket(hostname, port, getValue("sdduser"), getValue("sddpass"));
//}

bool FAN_Hash::readFromSocket (char *hostname, int port)
{
	FAN_ENTER;
	FAN_RETURN readFromSocket(hostname, port, getValue("sdduser"), getValue("sddpass"));
}

bool FAN_Hash::readFromSocket (char *hostname, int port, char *user, char *pwd)
{
        FAN_ENTER;	
        int sd;

        sd = FAN_getClientSocket(hostname, port);

        if(sd == 0)
        {
            FAN_RETURN false;
        }

        FAN_xlog(FAN_DEBUG|FAN_SOCKET|FAN_NOCR,"Send command to %s:%d : ",hostname,port);

	if(user != NULL && pwd != NULL)
	{
        	FAN_rpc(this, sd, "sdd::auth", 2, user, pwd);
	}
        clear();
        bool ret = FAN_rpc(this, sd, "sdd::readconf", 0);
        FAN_rpc(this, sd, "exit", 0);

        close(sd);
        FAN_RETURN ret;
}

bool FAN_Hash::readFromString (char *line, int decode)
{
        FAN_ENTER;	
        int     i;
        int     status = FAN_BEFORE_KEY;

        char    *key_pointer;
        char    *info_pointer;
	char    *value;
	char    *pkey;

        int     length = 0;

	if(hash == NULL)
	{
		FAN_RETURN false;
	}

        key_pointer  = line;
        info_pointer = line;

	length = strlen(line);

	char *psection = NULL;

	lock();

        for(i=0; i < length; i++)
        {
                switch(status)
                {
			case FAN_SECTION :
				if(line[i] == ']')
				{
					line[i] = '\0';
					status = FAN_BEFORE_KEY;
				}
				break;
                        case FAN_BEFORE_KEY :
				if(line[i] == '[')
				{
					psection = (line+i+1);
				        status = FAN_SECTION;
				}
				else if(line[i] == '=')
				{
				        key_pointer = (line+i);	
					line[i] = '\0';
					status = FAN_BEFORE_VALUE;
				}
                                else if(line[i] == '#')
                                {
                                        status = FAN_COMMENT;
                                }else if(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r')
                                {
				}else if(line[i] == '"')
                                {
                                        key_pointer = (line+i+1);
                                        status = FAN_EXT_KEY;
                                }else
                                {
                                        if(line[i] >= 'a' && line[i] <= 'z')
                                        {
                                                line[i] = line[i] + 'A' - 'a';
                                        }
                                        key_pointer = (line+i);
                                        status = FAN_KEY;
                                }
                                break;

                        case FAN_KEY :
				if(line[i] == '\n')
				{
					line[i] = '\0';

					if(strncmp(key_pointer,"ERROR", 5) == 0)
					{
						unlock();
						FAN_RETURN false;
					}
		
					if(strncmp(key_pointer,"EOF", 3) == 0)
					{
						unlock();
						FAN_RETURN true;
					}
				}
                                if(line[i] == ' ' || line[i] == '\t')
                                {
                                        line[i] = '\0';
                                        status = FAN_AFTER_KEY;

                                }else if(line[i] == '=')
                                {
                                        line[i] = '\0';
                                        status = FAN_BEFORE_VALUE;
                                }else
                                {
                                        if(line[i] >= 'a' && line[i] <= 'z')
                                        {
                                                line[i] = line[i] + 'A' - 'a';
                                        }
                                }
                                break;

                        case FAN_AFTER_KEY :
                                if(line[i] == '=')
                                {
                                        status = FAN_BEFORE_VALUE;
                                        info_pointer = (line + i + 1);
                                }
                                break;

                        case FAN_BEFORE_VALUE :

                                if(line[i] == ' ' || line[i] == '\t')
                                {
                                }else if(line[i] == '"')
                                {
                                        info_pointer = (line + i + 1);
                                        status = FAN_EXT_VALUE;
                                }else
                                {
                                        info_pointer = (line + i);
                                        status = FAN_VALUE;
                                }
                                break;

                        case FAN_VALUE :
                                if(
                                        line[i] == ' '  ||
                                        line[i] == '\t' ||
                                        line[i] == '\r' ||
                                        line[i] == '\n' ||
                                        line[i] == '#')
                                {
                                        line[i] = '\0';
                                        status = FAN_AFTER_VALUE;
                                }
                                break;

                        case FAN_EXT_KEY :
                                if(line[i] == '\\')
                                {
                                        status = FAN_KEY_QUOTE;
                                }
                                else if(line[i] == '"')
                                {
                                        line[i] = '\0';
                                        status = FAN_AFTER_KEY;
                                }
                                break;


				
                        case FAN_EXT_VALUE :
                                if(line[i] == '\\')
                                {
                                        status = FAN_QUOTE;
                                }
                                else if(line[i] == '"')
                                {
                                        line[i] = '\0';
                                        status = FAN_AFTER_VALUE;
                                }
                                break;

                        case FAN_KEY_QUOTE :

                                status = FAN_EXT_KEY;
				break;

                        case FAN_QUOTE :

                                status = FAN_EXT_VALUE;
                                break;


	                case FAN_AFTER_VALUE :

				pkey = key_pointer;
				if(psection != NULL)
				{
					asprintf(&pkey, "%s%s", psection, pkey);		
				}

                		if(decode)
              			{
		
               				if(FAN_Base64::adecode64(info_pointer, &value))
					{
                       				insert(pkey, value);
                       				free(value);
					}
               			}else
                		{
                        		insert(pkey, info_pointer);
                		}

				if(psection != NULL)
				{
					free(pkey);
				}

				i--;
				status = FAN_BEFORE_KEY;
				break;

			case FAN_COMMENT :
				if(line[i] == '\n')
					status = FAN_BEFORE_KEY;				
				break;
		}
	}

        if(status == FAN_AFTER_VALUE)
	{
		pkey = key_pointer;
		if(psection != NULL)
		{
			asprintf(&pkey, "%s%s", psection, pkey);		
		}
                if(decode)
                {
                        if(FAN_Base64::adecode64(info_pointer, &value))
			{
                        	insert(pkey, value);
                        	free(value);
			}
                }else
                {
                        insert(pkey, info_pointer);

                }
		if(psection != NULL)
		{
			free(pkey);
		}
	}

	unlock();

        if(status != FAN_COMMENT && status != FAN_BEFORE_KEY && status != FAN_AFTER_VALUE)
        {
                FAN_xlog(FAN_ERROR, "Syntax error in configfile");
        }

	FAN_RETURN true;
}

bool FAN_Hash::readFromFileStream (int FH, int decode, bool file)
{
        FAN_ENTER;	
	char 	*pline = m_line;

	char    *append = "%s%s"; 
	char 	*total  = strdup(append);

	int len;
	int glen = 0;

	bool end = false;

  	m_line[0] = '\0';

        while(!end)
        {
		len=FAN_readall(pline, MAXLINE - 1, FH);
		

		if(len < 0)
		{
			FAN_RETURN false;
		}

		glen += len;

		
		if(len > 0)
		{	
			char *p = total;
			asprintf(&total, total, pline, append);
			free(p);
		}else // if(file)
		{
			end = true;
		}

		if(glen >= 9)
		{
			char *last = (total+glen-9);
			if(strstr(last, "\nEOF\n") != NULL)
			{
				end = true;
			}
		}

		m_line[0] = '\0';

	}

	char *p = total;
	asprintf(&total, total, pline, "");
	free(p);

	bool ret = readFromString(total, decode);
	free(total);
	FAN_RETURN ret;
}

bool FAN_Hash::readFromStream (int FH, int decode, bool file)
{
        FAN_ENTER;	
	char 	*pline = m_line;

	char    *append = "%s%s"; 
	char 	*total  = strdup(append);

	int len;
	int glen = 0;

	bool end = false;

  	m_line[0] = '\0';

        while(!end)
        {
		len=FAN_readall(pline, MAXLINE - 1, FH);
		

		if(len < 0)
		{
			FAN_RETURN false;
		}

		glen += len;

		
		if(len > 0)
		{	
			char *p = total;
			asprintf(&total, total, pline, append);
			free(p);
		}else // if(file)
		{
			end = true;
		}

		if(glen >= 9)
		{
			char *last = (total+glen-9);
			if(strstr(last, "\nEOF\n") != NULL)
			{
				end = true;
			}
		}

		m_line[0] = '\0';

	}

	char *p = total;
	asprintf(&total, total, pline, "");
	free(p);

	bool ret = readFromString(total, decode);
	free(total);
	FAN_RETURN ret;
}
