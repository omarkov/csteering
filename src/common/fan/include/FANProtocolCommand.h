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



#ifndef _FAN_PROTOCMD
#define _FAN_PROTOCMD

#include "FANHash.h"

/**
 * Encapsulates a remote procedure
 */
class FAN_ProtocolCommand
{

public:
	/* the remote procedure */
        void (*fkt)(FAN_Hash*, char**);

	/* Anonymous access to this command allowed */
	bool anonymous;
	/* Procedure currently available */
	bool available;

	/* Counter: Number of procedures */
	static int count; 

	/**
	 * Constructor
	 *
	 * @param fkt The remote procedure
	 * @param anonym Is anonymously accessible
	 * @param available Is currently available
	 */
	FAN_ProtocolCommand(void (*fkt)(FAN_Hash*, char**),bool anonym, bool available)
	{
		this->count ++;
		this->fkt = fkt;
		this->anonymous=anonym;
		this->available=available;
	}
};

#endif
