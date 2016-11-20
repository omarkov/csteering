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



#ifndef _FAN_UTILS
#define _FAN_UTILS

#include "FANClasses.h"

#define FAN_BEFORE_KEY       0x0
#define FAN_KEY              0x1
#define FAN_AFTER_KEY        0x2

#define FAN_INT    0x1
#define FAN_FLOAT  0x2
#define FAN_DOUBLE 0x3
#define FAN_CHAR   0x4
#define FAN_BYTE   0x5
#define FAN_LONG   0x6
#define FAN_STRUCT 0x7


/**
 * Communication structure
 * 
 * WARNING: Do not manipulate the fields of this struct.
 */
typedef struct {
  pthread_mutex_t com_mutex;
  pthread_mutex_t lock_mutex;
  pthread_cond_t  com_cv;
  FAN_Queue      *MsgQueue;
  FAN_Queue      *msgRecycler;
  FAN_Queue      *replyMsgRecycler;
} FAN_Com;
/**
 * Message structure
 */
typedef struct {
  /* The message type, used for message routing */
  char    *type;
  /* Pointer to message data */
  void    *value;
  /* The senders thread-id */
  int      threadid;
  /* The type of the reply message, used for message routing */
  char    *replyType;
  /* The reply communication queue */
  FAN_Com *reply;
} FAN_Msg;

/**
 * Reply message structure for automated synchronous messaging  (sendMessage)
 */

// typedef struct {
  /* Pointer to message data */
////    void   *msg;
  /* Pointer to result data */
//   void   *res;
  /* The reply communication queue */
//   FAN_Com *reply;
  /* The type of the reply message, used for message routing */
//   char   *replyType;
// } FAN_ReplyMsg;

/**
 * Recycles the message [msg].
 *
 * @param com communication queue
 * @param msg message
 * 
 * @see FAN_recycleReplyMsg
 * @see FAN_newMsg
 * @see FAN_newReplyMsg
 */
void FAN_recycleMsg(FAN_Com *com, FAN_Msg *msg);
/**
 * Recycles the reply message [msg].
 *
 * @param com communication queue
 * @param msg message
 * 
 * @see FAN_recycleMsg
 * @see FAN_newMsg
 * @see FAN_newReplyMsg
 */
// void FAN_recycleReplyMsg(FAN_Com *com, FAN_ReplyMsg *msg);
/**
 * Creates a new message.
 *
 * @return new or recycled message
 *
 * @param com communication queue
 * 
 * @see FAN_recycleMsg
 * @see FAN_recycleReplyMsg
 * @see FAN_newReplyMsg
 */
FAN_Msg *FAN_newMsg(FAN_Com *com);
/**
 * Creates a new reply message.
 *
 * @param com communication queue
 *
 * @return new or recycled reply message
 * 
 * @see FAN_recycleMsg
 * @see FAN_recycleReplyMsg
 * @see FAN_newMsg
 */
// FAN_ReplyMsg *FAN_newReplyMsg(FAN_Com *com);

/**
 * Initializes a message queue + locking facilities for inter-thread communication
 * via messages.
 *
 * For cleanup call the FAN_freeMessages function.
 * 
 * @return Instance to an initialized communication queue.
 * @see FAN_peekMessage
 * @see FAN_sendMessage
 * @see FAN_postMessage
 * @see FAN_freeMessages
 */
FAN_Com *FAN_initMessages();

/**
 * Waits for incoming messages (blocking).
 *
 * @param com communication queue
 * @return the message
 * 
 * @see FAN_initMessages
 * @see FAN_sendMessage
 * @see FAN_postMessage
 */
FAN_Msg *FAN_peekMessage(FAN_Com *com);

/**
 * @returns A copy of the main application instance (thread-safe). 
 */
void *FAN_getApp();

/**
 * Frees the communication queue + locking facilities
 *
 * @see FAN_initMessages
 */
void FAN_freeMessages(FAN_Com *com);

/** 
 * Sends a message to a communication queue. (synchronous)
 *
 * @param com communication queue
 * @param type the message type. Used for message routing (@see #FAN_registerHandler) 
 * @param value the message. Any global pointer is valid 
 *
 * @see FAN_initMessages
 * @see FAN_peekMessage
 * @see FAN_postMessage
 */
void *FAN_sendMessage (FAN_Com *com, char *type, void *value);
/** 
 * Sends a message to a communication queue. (synchronous)
 *
 * @param com communication queue
 * @param type the message type. Used for message routing (see #FAN_registerHandler)
 * @param replyType the type of the reply  message. Used for message routing (see #FAN_registerHandler)
 * @param value the message. Any global pointer is valid 
 *
 * @see FAN_initMessages
 * @see FAN_peekMessage
 * @see FAN_postMessage
 */
void *FAN_sendMessage (FAN_Com *com, char *type, char *replyType, void *value);
/** 
 * Sends a message to a communication queue. (asynchronous)
 *
 * @param com communication queue
 * @param type the message type. Used for message routing (see #FAN_registerHandler)
 * @param reply communication queue for the reply message. Use NULL unidirectional messages.
 * @param value the message. Any global pointer is valid 
 *
 * @see FAN_initMessages
 * @see FAN_peekMessage
 * @see FAN_sendMessage
 */
void  FAN_postMessage (FAN_Com *com, char *type, FAN_Com *reply, void *value);
/** 
 * Sends a message to a communication queue. (asynchronous)
 *
 * @param com communication queue
 * @param type the message type. Used for message routing (see #FAN_registerHandler)
 * @param reply communication queue for the reply message. Use NULL unidirectional messages.
 * @param replyType the type of the reply  message. Used for message routing (see #FAN_registerHandler)
 * @param value the message. Any global pointer is valid 
 *
 * @see FAN_initMessages
 * @see FAN_peekMessage
 * @see FAN_sendMessage
 */
void  FAN_postMessage (FAN_Com *com, char *type, FAN_Com *reply, char *replyType, void *value);

/** 
 * Reverse the byte order of the binary data (bytes). 
 * The structure of the binary data is given by the fmt string.
 * 
 * The function is mainly for internal use.
 *
 * @param bytes pointer to binary data
 * @param fmt the binary structure [see #FAN_vrpc(FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)]
 * @param isStruct boolean
 *
 * @see FAN_aBin2Base64
 * @see FAN_aBase642Bin
 * @see FAN_vrpc(FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)
 */
unsigned char *FAN_reverseByteOrder(unsigned char *bytes, char *fmt, bool isStruct);
/**
 * Determines the number of parameters in the template.
 *
 * @param fmt the template [see #FAN_vrpc(FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)] 
 * @return the number of parameters in the template
 */
int FAN_getParamCount(char *fmt);
/**
 * Determines the size (in bytes) of the binary data described by the template (fmt).
 * 
 * @param fmt the template [see #FAN_vrpc(FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)]
 * @param isStruct is struct
 * @return the size in bytes
 */
int FAN_getParamSize(char *fmt, bool isStruct);
/**
 * Determines the next type in the template (types) 
 *
 * @param types the template [see #FAN_vrpc(FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)]
 * @param txt pointer to the begining of the textual type (e.g. int[3])
 * @param asize the size of the array. 1, if single value
 */
int FAN_getNextType(char **types, char **txt, int *asize);
/**
 * Converts binary data to its base-64 representation. The size increases by 1/3.
 *
 * @param fmt the template [see #FAN_vrpc(FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)]
 * @param arg pointer to the binary data
 * @return the base-64 representation
 *
 * @see FAN_aBase642Bin(char *fmt, char *arg)
 * @see FAN_aBase642Bin(char *fmt, char *arg, char *remoteEndian)
 */
char *FAN_aBin2Base64(char *fmt, unsigned char *arg);
/**
 * Converts a base-64 string to binary data described by the template (fmt).
 *
 * @param fmt he template [see #FAN_vrpc(FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)]
 * @param arg pointer to binary data
 * @param remoteEndian the endian configuration (little/big) of the remote peer
 *
 * @see FAN_aBin2Base64(char *fmt, unsigned char *arg)
 * @see FAN_aBase642Bin(char *fmt, unsigned char *arg)
 */
unsigned char *FAN_aBase642Bin(char *fmt, char *arg, char *remoteEndian);
/**
 * Converts a base-64 string to binary data described by the template (fmt).
 *
 * @param fmt he template [see #FAN_vrpc(FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)]
 * @param arg pointer to binary data
 *
 * @see FAN_aBin2Base64(char *fmt, unsigned char *arg)
 * @see FAN_aBase642Bin(char *fmt, unsigned char *arg, char *remoteEndian)
 */
unsigned char *FAN_aBase642Bin(char *fmt, char *arg);

/**
 * Calls a remote procedure.
 *
 * @param hash a initialized hash for the return values of the function
 * @param sd the socket descriptor
 * @param fkt the function name
 * @param params the number of parameters in the vector 
 * @return true if succeed
 *
 * @see FAN_rpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, ...)
 * @see FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, int params, va_list argument)
 * @see FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, va_list argument)
 */
int  FAN_rpc  (FAN_Hash *hash, int sd, char *fkt, int params, ...);
/**
 * Calls a remote procedure.
 *
 * @param hash a initialized hash for the return values of the function
 * @param sd the socket descriptor
 * @param fkt the function name
 * @param fmt the template of the vector [see #FAN_vrpc (FAN_Hash *hash,int sd,char *fkt,char *fmt, va_list argument)]
 * @return true if succeed
 *
 * @see FAN_rpc(FAN_Hash *hash, int sd, char *fkt, int params, ...)
 * @see FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, int params, va_list argument)
 * @see FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, va_list argument)
 */
int  FAN_rpc  (FAN_Hash *hash, int sd, char *fkt, char *fmt, ...);
/**
 * Calls a remote procedure.
 *
 * @param hash a initialized hash for the return values of the function
 * @param sd the socket descriptor
 * @param fkt the function name
 * @param params the number of parameters in the vector 
 * @param argument the argument vector
 * @return true if succeed
 *
 * @see FAN_rpc(FAN_Hash *hash, int sd, char *fkt, int params, ...)
 * @see FAN_rpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, ...)
 * @see FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, va_list argument)
 */
int  FAN_vrpc (FAN_Hash *hash, int sd, char *fkt, int params, va_list argument);
/**
 * Calls a remote procedure.
 *
 *
 * @param hash a initialized hash for the return values of the function
 * @param sd the socket descriptor
 * @param fkt the function name
 * @param fmt the template 
 * @param argument the argument vector
 * @return true if succeed
 *
 * The template (fmt) describes the binary data in the parameter vector:
 *
 * <b>Elementary types:</b>
 * <pre>
 * - byte (unsigned char)
 * - char
 * - int
 * - float (double)
 * - double 
 * - long</pre>
 *
 * <b>Compound types:</b>
 * <pre>
 * - Struct: {int;byte} 
 * - Array: int[5]</pre>
 * 
 * <b>Complex example:</b>
 * <pre>
 *  typedef struct struct1
 *  {
 *    double d1;
 *    double d2;
 *    double d3;
 *  }
 *  typedef struct struct2
 *  {
 *    char text[20];
 *    struct1 vectors[100];
 *  }
 *  int *ia = new int[5];
 *  struct2 *s = new struct2[3];
 *  long l = 100;
 *
 *  fillStruct(s);
 *
 *  char *templ = "int[5];{char[20];{double;double;double}[100]}[3];long";
 *  FAN_vrpc(hash, sd, "test", templ, ia, s, &l);</pre>
 *
 * @see FAN_rpc(FAN_Hash *hash, int sd, char *fkt, int params, ...)
 * @see FAN_rpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, ...)
 * @see FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, int params, va_list argument)
 */
int  FAN_vrpc (FAN_Hash *hash, int sd, char *fkt, char *fmt, va_list argument);
int  FAN_vbinaryPush (int sd, char *fmt, va_list argument);

#ifdef __IRIX__
/**
 * Implementation of asprintf for IRIX64
 */
int   asprintf( char** s, const char* f, ... );
#endif

/**
 * Connects via tcp to a server.
 *
 * @param hostname the hostname/ip of the server
 * @param port the tcp port for the connection
 * @return the socket descriptor or 0 if the connection failed
 */
int   FAN_getClientSocket(char *hostname, int port);
/**
 * Extracts the #[pos] parameter from the parameter list [allParams] and converts
 * it from base-64 to ascii. The value is stored in the new allocated 
 * space [param]. 
 *
 * Note: [param] has to be freed by the caller.
 *
 * @param allParams the parameter list
 * @param param reference to an unallocated string. 
 * @param pos position in the parameter list (from zero)
 *
 * @see FAN_aGetBinaryParam(char **allParams, void **param, int pos)
 */
bool  FAN_aGetParam(char **allParams, char **param, int pos);
/**
 * Extracts the #[pos] parameter from the parameter list [allParams] and
 * convert int from base-64 to binary. The value is stored in the new allocated
 * space [param].
 *
 * Note: [param] has to be freed by the caller.
 *
 * @param allParams the parameter list
 * @param param unallocated pointer
 * @param pos position in the parameter list (from zero)
 *
 * @see FAN_aGetParam(char **allParams, void **param, int pos)
 */
bool  FAN_aGetBinaryParam(char **allParams, void **param, int pos);
/**
 * Initialize the logging facility.
 *
 * @param from which configuration should be used (client/server)
 */
void  FAN_setLogs(char *from);
/**
 * Converts a string to upper case.
 *
 * @param buffer the string
 */
void  FAN_upperCase(char *buffer);
/**
 * Reads the next line from an file handle (socket), allocates a string
 * and stores the data to it. 
 * 
 * @param FH the file descriptor (socket)
 * @param file not used
 * @param bufferSize Size of readline buffer
 * @param buffer Pointer to allocated buffer with the size of bufferSize
 * @return the read line
 *
 * @see FAN_readall
 */
char *FAN_arecvline(int FH, bool file, int bufferSize, char *buffer);
char *FAN_areadline(int FH, bool file, int bufferSize, char *buffer);
char *FAN_arecvlineC(int FH, bool file, int bufferSize, char *buffer);

bool FAN_binaryrecv(int FH, char *templ, int size, unsigned char* buffer);
/**
 * Tries to read [size] number of chars from a file handle (socket), and
 * stores the data in the pre-allocated string [line].
 *
 * @param line pre-allocated string
 * @param size the size of the string
 * @param FH the file descriptor (socket)
 *
 * @see FAN_areadline
 */
int   FAN_readall(char *line, int size, int FH);
int   FAN_recvall(char *line, int size, int FH);
/**
 * Writes a string to a file descriptor (socket).
 *
 * @param s the file descriptor (socket)
 * @param txt \\0-terminated string
 */
int   FAN_swrite(int s, char *txt);
int   FAN_swrite(int s, unsigned char *data, int size);
/**
 * Removes trailing CRs from the string [buf].
 *
 * @param buf the string
 *
 * @see FAN_clearCCR
 */
int   FAN_clearCR(char *buf);
/**
 * Removes trailing chars[c] from the string [buf].
 *
 * @param buf the string
 * @param c the char
 *
 * @see FAN_clearCR
 */
int   FAN_clearCCR(char *buf, char c);
/**
 * Removes the last [n] chars from the string [buf].
 *
 * @param buf the string
 * @param n the number of chars to remove
 */
void  FAN_clearLastN(char *buf, int n);
/**
 * @deprecated 
 */
char *FAN_aRegularReplace(char *orig_buf, char *pre, char *post, FAN_Hash *hash);
/**
 * @deprecated
 */
int   FAN_droproot(char *username);
/**
 * Outputs the binary data [ptr] as HEX values.
 *
 * @param ptr pointer to binary data
 * @param size the zize of the binary data
 */
void  FAN_dumpHex(char *ptr, int size);
/**
 * Returns a string with some info text about the build. 
 *
 * Note: the text must be freed by caller.
 *
 * @return Build-informations
 */
char *FAN_aGetBuild();
/**
 * @deprecated
 */
long  FAN_generateWebCode(void);
/**
 * @return id of the current thread
 */
int   FAN_getThreadId();
int   FAN__getThreadId();

/**
 *
 * Connects to a server identified by [server].
 * Gets hostname and port from the configuration file:
 *
 * hostname : [server]host
 * port : [server]port
 *
 * @param server Servername 
 * @return A FAN connection, or NULL if failed
 */
FAN_Connection *FAN_connect(char *server);

#endif
