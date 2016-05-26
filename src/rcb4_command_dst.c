/*
 *  This file is part of librcb4.
 *
 *  librcb4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  librcb4 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with librcb4.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file rcb4_command_dst.c
 * @brief Functions to set the command's destination.
 * 
 * @details These functions set the command's destination to be RAM, ICS, COM,
 * ROM or none. Not all commands can use them, check each function's
 * documentation for details.
 * 
 * @sa rcb4_command.c, rcb4_command_src.c
 * 
 * @version 1.0
 * @date November 2015
 * 
 * @author Alfonso Arbona Gimeno (alargi@etsii.upv.es, alf@katolab.nitech.ac.jp).
 * 
 * @copyright Copyright 2015 Alfonso Arbona Gimeno.
 * This project is released under the GNU Public License v3.
 * See COPYING for a full copy of the license.
 */



#include "rcb4.h"
#include "rcb4_private.h"
#include "rcb4_command.h"

#include <stdlib.h>
#include <string.h>

//TODO: Endian!!!
int rcb4_command_set_dst_ram(rcb4_comm* comm, uint16_t addr)
{
	assert(comm);
	
	// Check the parameters
	if(addr > RCB4_MAX_RAM_ADDRESS)
	{
		fprintf(stderr, "Invalid RAM address. Allowed address: 0x0000~0x%04X\n", RCB4_MAX_RAM_ADDRESS);
		return -1;
	}
	
	// We do different things depending on the message type
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			comm->command.mov.type = (comm->command.mov.type & (COMM_SRC_MASK)) | COMM_DST_RAM;
			comm->command.mov.dst.ram.addr[0] = (uint8_t)addr;
			comm->command.mov.dst.ram.addr[1] = (uint8_t)(addr >> 8);
			break;
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			comm->command.logic.type = (comm->command.logic.type & (COMM_SRC_MASK)) | COMM_DST_RAM;
			comm->command.logic.dst.ram.addr[0] = (uint8_t)addr;
			comm->command.logic.dst.ram.addr[1] = (uint8_t)(addr >> 8);
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type = (comm->command.math.type & (COMM_SRC_MASK)) | COMM_DST_RAM;
			comm->command.math.dst.ram.addr[0] = (uint8_t)addr;
			comm->command.math.dst.ram.addr[1] = (uint8_t)(addr >> 8);
			break;
		case RCB4_COMM_ICS:
			comm->command.ics.dst = addr; //TODO: Endian!!!
			break;
		case RCB4_COMM_NOT:
			comm->command.logic_not.type = (comm->command.logic_not.type & (COMM_SRC_MASK)) | COMM_DST_RAM;
			comm->command.logic_not.dst.ram.addr[0] = (uint8_t)addr;
			comm->command.logic_not.dst.ram.addr[1] = (uint8_t)(addr >> 8);
			break;
		case RCB4_COMM_SHIFT:
			comm->command.shift.type = (comm->command.shift.type & (COMM_SRC_MASK)) | COMM_DST_RAM;
			comm->command.shift.dst.ram.addr[0] = (uint8_t)addr;
			comm->command.shift.dst.ram.addr[1] = (uint8_t)(addr >> 8);
			break;
		
		// Invalid messages
		case RCB4_COMM_SINGLE:
		case RCB4_COMM_CONST:
		case RCB4_COMM_SERIES:
		case RCB4_COMM_SPEED:
			fprintf(stderr, "Invalid command type.\n");
			return -2;
		default:
			fprintf(stderr, "Unknown command type.\n");
			return -1;
	};
	
	return 0;
}


int rcb4_command_set_dst_ics(rcb4_comm* comm, uint8_t offset, uint8_t ics)
{
	assert(comm);
	
	// Check the parameters
	if(ics == 0 || ics > RCB4_ICS_QTY)
	{
		fprintf(stderr, "Invalid ICS id. Allowed values: 1~%d\n", RCB4_ICS_QTY);
		return -1;
	}
	
	ics--; // The robot expects ICS from 0 to (RCB4_ICS_QTY - 1)
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			comm->command.mov.type = (comm->command.mov.type & (COMM_SRC_MASK)) | COMM_DST_ICS;
			comm->command.mov.dst.ics.offset = offset;
			comm->command.mov.dst.ics.ics = ics;
			break;
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			comm->command.logic.type = (comm->command.logic.type & (COMM_SRC_MASK)) | COMM_DST_ICS;
			comm->command.logic.dst.ics.offset = offset;
			comm->command.logic.dst.ics.ics = ics;
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type = (comm->command.math.type & (COMM_SRC_MASK)) | COMM_DST_ICS;
			comm->command.math.dst.ics.offset = offset;
			comm->command.math.dst.ics.ics = ics;
			break;
		case RCB4_COMM_NOT:
			comm->command.logic_not.type = (comm->command.logic_not.type & (COMM_SRC_MASK)) | COMM_DST_ICS;
			comm->command.logic_not.dst.ics.offset = offset;
			comm->command.logic_not.dst.ics.ics = ics;
			break;
		case RCB4_COMM_SHIFT:
			comm->command.shift.type = (comm->command.shift.type & (COMM_SRC_MASK)) | COMM_DST_ICS;
			comm->command.shift.dst.ics.offset = offset;
			comm->command.shift.dst.ics.ics = ics;
			break;
		
		// Invalid messages
		case RCB4_COMM_ICS:
		case RCB4_COMM_SINGLE:
		case RCB4_COMM_CONST:
		case RCB4_COMM_SERIES:
		case RCB4_COMM_SPEED:
			fprintf(stderr, "Invalid command type.\n");
			return -2;
		default:
			fprintf(stderr, "Unknown command type.\n");
			return -1;
	};
	
	return 0;
}


int rcb4_command_set_dst_com(rcb4_comm* comm) //TODO: Write zeros
{
	assert(comm);
	
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			comm->command.mov.type = (comm->command.mov.type & (COMM_SRC_MASK)) | COMM_DST_COM;
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type = (comm->command.math.type & (COMM_SRC_MASK)) | COMM_DST_COM;
			break;
		
		// Invalid messages
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
		case RCB4_COMM_ICS:
		case RCB4_COMM_SINGLE:
		case RCB4_COMM_CONST:
		case RCB4_COMM_SERIES:
		case RCB4_COMM_SPEED:
		case RCB4_COMM_NOT:
		case RCB4_COMM_SHIFT:
			fprintf(stderr, "Invalid command type.\n");
			return -2;
		default:
			fprintf(stderr, "Unknown command type.\n");
			return -1;
	};
	
	return 0;
}


int rcb4_command_set_dst_rom(rcb4_comm* comm, uint32_t addr)
{
	assert(comm);
	
	if(addr > RCB4_MAX_ROM_ADDRESS)
	{
		fprintf(stderr, "Invalid ROM address. Allowed address: 0x000000~0x%06X\n", RCB4_MAX_RAM_ADDRESS);
		return -1;
	}
	
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			comm->command.mov.type = (comm->command.mov.type & (COMM_SRC_MASK)) | COMM_DST_ROM;
			comm->command.mov.dst.rom.addr[0] = (uint8_t)addr;
			comm->command.mov.dst.rom.addr[1] = (uint8_t)(addr >> 8);
			comm->command.mov.dst.rom.addr[2] = (uint8_t)(addr >> 16);
			break;
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			comm->command.logic.type = (comm->command.logic.type & (COMM_SRC_MASK)) | COMM_DST_ROM;
			comm->command.logic.dst.rom.addr[0] = (uint8_t)addr;
			comm->command.logic.dst.rom.addr[1] = (uint8_t)(addr >> 8);
			comm->command.logic.dst.rom.addr[2] = (uint8_t)(addr >> 16);
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type = (comm->command.math.type & (COMM_SRC_MASK)) | COMM_DST_ROM;
			comm->command.math.dst.rom.addr[0] = (uint8_t)addr;
			comm->command.math.dst.rom.addr[1] = (uint8_t)(addr >> 8);
			comm->command.math.dst.rom.addr[2] = (uint8_t)(addr >> 16);
			break;
		case RCB4_COMM_NOT:
			comm->command.logic_not.type = (comm->command.logic_not.type & (COMM_SRC_MASK)) | COMM_DST_ROM;
			comm->command.logic_not.dst.rom.addr[0] = (uint8_t)addr;
			comm->command.logic_not.dst.rom.addr[1] = (uint8_t)(addr >> 8);
			comm->command.logic_not.dst.rom.addr[2] = (uint8_t)(addr >> 16);
			break;
		case RCB4_COMM_SHIFT:
			comm->command.shift.type = (comm->command.shift.type & (COMM_SRC_MASK)) | COMM_DST_ROM;
			comm->command.shift.dst.rom.addr[0] = (uint8_t)addr;
			comm->command.shift.dst.rom.addr[1] = (uint8_t)(addr >> 8);
			comm->command.shift.dst.rom.addr[2] = (uint8_t)(addr >> 16);
			break;
		
		// Invalid messages
		case RCB4_COMM_ICS:
		case RCB4_COMM_SINGLE:
		case RCB4_COMM_CONST:
		case RCB4_COMM_SERIES:
		case RCB4_COMM_SPEED:
			fprintf(stderr, "Invalid command type.\n");
			return -2;
		default:
			fprintf(stderr, "Unknown command type.\n");
			return -1;
	};
	
	return 0;
}


int rcb4_command_set_dst_do_not_save(rcb4_comm* comm)
{
	assert(comm);
	
	switch(comm->type)
	{
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			comm->command.logic.type |= COMM_NUPDATE;
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type |= COMM_NUPDATE;
			break;
		case RCB4_COMM_NOT:
			comm->command.logic_not.type |= COMM_NUPDATE;
			break;
		case RCB4_COMM_SHIFT:
			comm->command.shift.type |= COMM_NUPDATE;
			break;
		
		// Invalid messages
		case RCB4_COMM_MOV:
		case RCB4_COMM_ICS:
		case RCB4_COMM_SINGLE:
		case RCB4_COMM_CONST:
		case RCB4_COMM_SERIES:
		case RCB4_COMM_SPEED:
			fprintf(stderr, "Invalid command type.\n");
			return -2;
		default:
			fprintf(stderr, "Unknown command type.\n");
			return -1;
	};
	
	return 0;
}

