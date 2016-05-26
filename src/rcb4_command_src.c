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
 * @file rcb4_command_src.c
 * @brief Functions to set the command's source.
 * 
 * @details These functions set the command's source to be RAM, ICS, a literal
 * or ROM. Not all commands can use them, check each function's documentation
 * for details.
 * 
 * @sa rcb4_command.c, rcb4_command_dst.c
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
int rcb4_command_set_src_ram(rcb4_comm* comm, uint16_t addr, uint8_t size)
{
	assert(comm);
	
	// Check the parameters
	if(size == 0 || size > RCB4_COMM_MESSAGE_SIZE_ALLOWED - 10)
	{
		fprintf(stderr, "Invalid data size. Allowed values: 1~%d\n", RCB4_COMM_MESSAGE_SIZE_ALLOWED - 10);
		return -1;
	}
	if(addr > RCB4_MAX_RAM_ADDRESS)
	{
		fprintf(stderr, "Invalid RAM address. Allowed address: 0x0000~0x%04X\n", RCB4_MAX_RAM_ADDRESS);
		return -1;
	}
	
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			comm->command.mov.type = (comm->command.mov.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_RAM;
			comm->command.mov.src.ram.addr[0] = (uint8_t)addr;
			comm->command.mov.src.ram.addr[1] = (uint8_t)(addr >> 8);
			comm->command.mov.src.ram.size = size;
			break;
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			comm->command.logic.type = (comm->command.logic.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_RAM;
			comm->command.logic.src.ram.addr[0] = (uint8_t)addr;
			comm->command.logic.src.ram.addr[1] = (uint8_t)(addr >> 8);
			comm->command.logic.src.ram.size = size;
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type = (comm->command.math.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_RAM;
			comm->command.math.src.ram.addr[0] = (uint8_t)addr;
			comm->command.math.src.ram.addr[1] = (uint8_t)(addr >> 8);
			comm->command.math.src.ram.size = size;
			break;
		case RCB4_COMM_ICS:
			if(size > RCB4_MAX_ICS_SRC_SIZE)
			{
				fprintf(stderr, "Invalid data size. Allowed values: 1~%d\n", RCB4_MAX_ICS_SRC_SIZE);
				return -1;
			}
			comm->command.ics.data_size = size;
			comm->command.ics.src = addr; //TODO: Endian!!!
			comm->size = 9;
			return 0; // Return to avoid changing the size to 10
		
		// Invalid messages
		case RCB4_COMM_NOT:
		case RCB4_COMM_SHIFT:
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
	
	// Note that RCB4_COMM_ICS does not execute this (has a return instead of a break)
	comm->size = 10;
	
	return 0;
}


int rcb4_command_set_src_ics(rcb4_comm* comm, uint8_t offset, uint8_t ics, uint8_t size)
{
	assert(comm);
	
	// Check the parameters
	if(size == 0 || size > RCB4_COMM_MESSAGE_SIZE_ALLOWED)
	{
		fprintf(stderr, "Invalid data size. Allowed values: 1~%d\n", RCB4_COMM_MESSAGE_SIZE_ALLOWED);
		return -1;
	}
	if(ics == 0 || ics > RCB4_ICS_QTY)
	{
		fprintf(stderr, "Invalid ICS id. Allowed values: 1~%d\n", RCB4_ICS_QTY);
		return -1;
	}
	
	ics--; // The robot expects ICS from 0 to (RCB4_ICS_QTY - 1)
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			comm->command.mov.type = (comm->command.mov.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_ICS;
			comm->command.mov.src.ics.offset = offset;
			comm->command.mov.src.ics.ics = ics;
			comm->command.mov.src.ics.size = size;
			break;
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			comm->command.logic.type = (comm->command.logic.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_ICS;
			comm->command.logic.src.ics.offset = offset;
			comm->command.logic.src.ics.ics = ics;
			comm->command.logic.src.ics.size = size;
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type = (comm->command.math.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_ICS;
			comm->command.math.src.ics.offset = offset;
			comm->command.math.src.ics.ics = ics;
			comm->command.math.src.ics.size = size;
			break;
		
		// Invalid messages
		case RCB4_COMM_NOT:
		case RCB4_COMM_SHIFT:
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
	
	comm->size = 10;
	
	return 0;
}


int rcb4_command_set_src_literal(rcb4_comm* comm, const void* literal, uint8_t length)
{
	uint8_t *ptr;
	assert(comm);
	assert(literal);
	
	if(length == 0 || length >= COMM_LITERAL_MAX_LEN)
	{
		fprintf(stderr, "Invalid length. Allowed range: 1~%d bytes.\n", COMM_LITERAL_MAX_LEN -1);
		return -1;
	}
	
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			comm->command.mov.type = (comm->command.mov.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_LIT;
			ptr = comm->command.mov.src.lit;
			break;
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			comm->command.logic.type = (comm->command.logic.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_LIT;
			ptr = comm->command.logic.src.lit;
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type = (comm->command.math.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_LIT;
			ptr = comm->command.math.src.lit;
			break;
		
		// Invalid messages
		case RCB4_COMM_NOT:
		case RCB4_COMM_SHIFT:
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
	
	
	memcpy(ptr, literal, length);
	comm->size = length + 7;
	
	return 0;
}


int rcb4_command_set_src_rom(rcb4_comm* comm, uint32_t addr, uint8_t size)
{
	assert(comm);
	
	// Check the parameters
	if(size == 0 || size > RCB4_COMM_MESSAGE_SIZE_ALLOWED)
	{
		fprintf(stderr, "Invalid data size. Allowed values: 1~%d\n", RCB4_COMM_MESSAGE_SIZE_ALLOWED);
		return -1;
	}
	if(addr > RCB4_MAX_ROM_ADDRESS)
	{
		fprintf(stderr, "Invalid ROM address. Allowed address: 0x000000~0x%06X\n", RCB4_MAX_RAM_ADDRESS);
		return -1;
	}
	
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			comm->command.mov.type = (comm->command.mov.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_ROM;
			comm->command.mov.src.rom.addr[0] = (uint8_t)addr;
			comm->command.mov.src.rom.addr[1] = (uint8_t)(addr >> 8);
			comm->command.mov.src.rom.addr[2] = (uint8_t)(addr >> 16);
			comm->command.mov.src.rom.size = size;
			break;
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			comm->command.logic.type = (comm->command.logic.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_ROM;
			comm->command.logic.src.rom.addr[0] = (uint8_t)addr;
			comm->command.logic.src.rom.addr[1] = (uint8_t)(addr >> 8);
			comm->command.logic.src.rom.addr[2] = (uint8_t)(addr >> 16);
			comm->command.logic.src.rom.size = size;
			break;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			comm->command.math.type = (comm->command.math.type & (COMM_DST_MASK | COMM_NUPDATE)) | COMM_SRC_ROM;
			comm->command.math.src.rom.addr[0] = (uint8_t)addr;
			comm->command.math.src.rom.addr[1] = (uint8_t)(addr >> 8);
			comm->command.math.src.rom.addr[2] = (uint8_t)(addr >> 16);
			comm->command.math.src.rom.size = size;
			break;
		
		// Invalid messages
		case RCB4_COMM_NOT:
		case RCB4_COMM_SHIFT:
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
	
	comm->size = 11;
	
	return 0;
}

