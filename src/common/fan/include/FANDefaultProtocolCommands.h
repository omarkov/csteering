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



#ifndef _FAN_DEFAULTPROTO
#define _FAN_DEFAULTPROTO
#include <time.h>

#define FAN_AUTHED   1
#define FAN_UNAUTHED 2
#define FAN_NOUSER   3

/**
 * The server start time
 */
extern time_t FAN_start;
/**
 * The RPC management hash
 */
extern FAN_Hash *FAN_cmd;

/**
 * Loads the default RPC commands.
 */
void FAN_init();
/**
 * Cleanups the RPC stack.
 */
void FAN_final();

/**
 * Loads an internal command [fkt] identified by [cmd]. 
 *
 * @param cmd The command identifier 
 * @param fkt Pointer to callback function
 * @param anonym Not used
 * @param available Is available
 */
void FAN_loadCmd(char *cmd, void (*fkt)(FAN_Hash*, char**),bool anonym, bool available);
/**
 * Creates a new domain.
 *
 * @param domain Name of domain
 * @param init Initialize default commands
 */
void FAN_createDomain(char *domain, bool init);

/**
 * Loads commands from configuration hash.
 *
 * @param hash The configuration hash
 */
void FAN_loadCommands(FAN_Hash *hash);
/**
 * Loads an external command [fkt] identified by [cmd]. 
 *
 * @param cmd The command identifier
 * @param fkt Pointer to callback function
 * @param anonym Not used
 * @param available Is available
 */
void FAN_loadExtCmd(char *cmd, void (*fkt)(FAN_Hash*, char**),bool anonym, bool available);

/**
 * Authentication function (user, pwd)
 */
void FAN_cmdSysAuth(FAN_Hash *ret, char **params);
/**
 * Deactivate a command
 *
 * @param name The command identifier
 */
bool FAN_cmdSysUnloadCmd(char *name);
/**
 * Activate a command
 *
 * @param name The command identifier
 */
bool FAN_cmdSysLoadCmd(char *name);
/**
 * Deactivate a command (identifier)
 */
void FAN_cmdSysUnloadCmd(FAN_Hash *ret, char **params);
/**
 * Halt the system
 */
bool FAN_sysHalt();
/**
 * Halt the system
 */
void FAN_cmdSysHalt(FAN_Hash *ret, char **params);
/**
 * Pings the system (Keepalive)
 */
void FAN_cmdSysPing(FAN_Hash *ret, char **params);
/**
 * Sets the endian (little or big)
 */
void FAN_cmdSysEndian(FAN_Hash *ret, char **params);
/**
 * Sets the readline buffer size (default: 1024)
 */
void FAN_cmdSysSetBufferSize(FAN_Hash *ret, char **params);
/**
 * Activate a command (identifier)
 */
void FAN_cmdSysLoadCmd(FAN_Hash *ret, char **params);
/**
 * Returns the current server version
 */
void FAN_cmdSysVersion(FAN_Hash *ret, char **params);
/**
 * Returns the uptime
 */
void FAN_cmdSysUptime(FAN_Hash *ret, char **params);
/**
 * Returns the server status 
 */
void FAN_cmdSysStatus(FAN_Hash *ret, char **params);

/**
 * Initializes the grid
 */
void FAN_cmdGridInit();
/**
 * Authentication function (user, pwd)
 */
void FAN_cmdGridAuth(FAN_Hash *ret, char **params);
/**
 * Initializes the grid
 */
void FAN_cmdGridInit(FAN_Hash *ret, char **params);

/**
 * Initalizes the system directory
 */
void FAN_cmdSddInit();
/**
 * Initalizes the system directory
 */
void FAN_cmdSddInit(FAN_Hash *ret, char **params);
/**
 * Authentication function (user, pwd)
 */
void FAN_cmdSddAuth(FAN_Hash *ret, char **params);
/** 
 * Return the db connection parameters
 */
void FAN_cmdSddGetDBMS(FAN_Hash *ret, char **params);
/**
 * Return the locking facility parameters
 */
void FAN_cmdSddGetLockd(FAN_Hash *ret, char **params);
/**
 * Returns the server configuration
 */
void FAN_cmdSddReadConf(FAN_Hash *ret, char **params);
/**
 * Returns the verbucher connection parameters
 */
void FAN_cmdSddGetVerbucher(FAN_Hash *ret, char **params);

/**
 * Changes the namespace
 */
void FAN_cmdUse(FAN_Hash *ret, char **params);
/**
 * Returns the available functions
 */
void FAN_cmdHelp(FAN_Hash *ret, char **params);
/**
 * Returns a SID
 */
void FAN_cmdConnect(FAN_Hash *ret, char **params);
/**
 * Enables cleartext
 */
void FAN_cmdClearText(FAN_Hash *ret, char **params);
/**
 * Disables cleartext
 */
void FAN_cmdNoClearText(FAN_Hash *ret, char **params);
/**
 * Closes the connection
 */
void FAN_cmdExit(FAN_Hash *ret, char **params);
#endif
