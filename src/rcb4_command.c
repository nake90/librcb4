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
 * @file rcb4_command.c
 * @brief Functions to create and set the command message contents.
 * 
 * @details These functions create and delete a command message for the RCB4
 * processor. Depending on the type of message you can set one or more values
 * into the message using the functions implemented.
 * 
 * @sa rcb4_command_dst.c, rcb4_command_src.c
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
#include "rcb4_config.h"
#include "rcb4_command.h"

#include <stdlib.h>
#include <string.h>

rcb4_comm* rcb4_command_create(enum e_rcb4_command_types type)
{
	rcb4_comm* comm;
	
	// Malloc the command
	comm = (rcb4_comm*)malloc(sizeof(rcb4_comm));
	if(!comm)
	{
		fprintf(stderr, "Memory error.\n");
		return NULL;
	}
	
	// Fill it
	if(rcb4_command_recreate(comm, type) != 0)
	{
		free(comm);
		return NULL;
	}
	
	return comm;
}

void rcb4_command_delete(rcb4_comm* comm)
{
	if(!comm)return;
	
	free(comm);
}

// Fills the command
int rcb4_command_recreate(rcb4_comm* comm, enum e_rcb4_command_types type)
{
	// First zero it
	memset(comm, 0, sizeof(rcb4_comm));
	
	// Set the type
	comm->type = (uint8_t)type;
	
	// Set the size of the ones with fixed size (or zero to the ones that are
	// variable to avoid sending uncomplete messages)
	switch(comm->type)
	{
		// Cases with variable message length:
		case RCB4_COMM_MOV:
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
		case RCB4_COMM_CONST:
		case RCB4_COMM_SERIES:
		case RCB4_COMM_SPEED:
			comm->size = 0; // To be set later on
			break;
		
		// Cases with constant message length:
		case RCB4_COMM_NOT:
			comm->size = 10;
			break;
		case RCB4_COMM_SHIFT:
			comm->size = 10;
			break;
		case RCB4_COMM_ICS:
			comm->size = 9;
			break;
		case RCB4_COMM_SINGLE:
			comm->size = 7;
			break;
		default:
			fprintf(stderr, "Unknown command type.\n");
			return -1;
	};
	
	return 0;
}



void rcb4_command_debug_print(const rcb4_comm* comm)
{
	if(!comm) // Don't sigfault! Print NULL instead.
	{
		printf("Command = NULL\n");
		return;
	}
	
	int i;
	const uint8_t checksum = rcb4_command_calculate_checksum(comm);
	const uint8_t* ptr = (const uint8_t*)comm;
	
	for(i = 0; i < comm->size - 1; i++)
	{
		printf("0x%02X ", (uint8_t)(*(ptr++))); // I love pointers :D
	}
	
	printf("Checksum = %u (0x%02X)\n",checksum, checksum);
}


uint8_t rcb4_command_calculate_checksum(const rcb4_comm* comm)
{
	int i;
	uint16_t sum = 0; // Maximum value = RCB4_COMM_MESSAGE_SIZE_ALLOWED * 0xFF = 32768 < 65536 -> uint16_t is enough
	const uint8_t* ptr = (uint8_t*)comm; // Check the message byte by byte

	assert(comm); // Make sure that the message is not null

	for(i = 0; i < comm->size - 1; i++) // For all bytes except the checksum byte
	{
		sum += *(ptr++); // Add the byte and point to the next one
	}

	return (uint8_t)sum; // We only need the lowest byte
}

// Returns the number of bytes to expect as answer (not including the size, command, checksum and ACK/NACK)
uint8_t rcb4_command_get_response_size(const rcb4_comm* comm)
{
	assert(comm);
	
	switch(comm->type)
	{
		// Cases with variable message length:
		case RCB4_COMM_MOV:
			if((comm->command.mov.type & COMM_DST_COM) == 0)return 0; // Not to COM
			
			switch(comm->command.mov.type & COMM_SRC_MASK)
			{
				case COMM_SRC_RAM:
					return comm->command.mov.src.ram.size;
				case COMM_SRC_ICS:
					return comm->command.mov.src.ics.size;
				case COMM_SRC_ROM:
					return comm->command.mov.src.rom.size;
				case COMM_SRC_LIT:
					return comm->size - 7;
				default:
					return 0;
			}
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			// Always send a copy to COM
			
			switch(comm->command.logic.type & COMM_SRC_MASK)
			{
				case COMM_SRC_RAM:
					return comm->command.logic.src.ram.size;
				case COMM_SRC_ICS:
					return comm->command.logic.src.ics.size;
				case COMM_SRC_ROM:
					return comm->command.logic.src.rom.size;
				case COMM_SRC_LIT:
					return comm->size - 7;
				default:
					return 0;
			}
		case RCB4_COMM_NOT:
			return comm->command.logic_not.size; // Always send a copy to COM
		case RCB4_COMM_SHIFT:
			return comm->command.shift.size; // Always send a copy to COM
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			// Always send a copy to COM
			
			switch(comm->command.math.type & COMM_SRC_MASK)
			{
				case COMM_SRC_RAM:
					return comm->command.math.src.ram.size;
				case COMM_SRC_ICS:
					return comm->command.math.src.ics.size;
				case COMM_SRC_ROM:
					return comm->command.math.src.rom.size;
				case COMM_SRC_LIT:
					return comm->size - 7;
				default:
					return 0;
			}
		case RCB4_COMM_ICS:
		case RCB4_COMM_SINGLE:
		case RCB4_COMM_CONST:
		case RCB4_COMM_SERIES:
		case RCB4_COMM_SPEED:
			return 0; // Do not return to COM
		default:
			fprintf(stderr, "Unknown command type.\n");
			return 0;
	};
}

static // Private one. shifts is the byte the robot expects (0~255)
int rcb4_command_set_shifts(rcb4_comm* comm, uint8_t shifts)
{
	assert(comm);
	
	if(comm->type != RCB4_COMM_SHIFT)
	{
		fprintf(stderr, "Invalid command type.\n");
		return -1;
	}
	
	comm->command.shift.shifts = shifts;
	
	return 0;
}

// Shift to the left. Shifts can only go from 0 to 127.
int rcb4_command_set_shift_left(rcb4_comm* comm, uint8_t shifts)
{
	assert(comm);
	
	if(comm->type != RCB4_COMM_SHIFT)
	{
		fprintf(stderr, "Invalid command type.\n");
		return -1;
	}
	
	if(shifts > 127)
	{
		fprintf(stderr, "Invalid parameter value. Allowed values [0~127].\n");
		return -1;
	}
	
	return rcb4_command_set_shifts(comm, shifts);
}

// Shift to the right. Shifts can only go from 0 to 127.
int rcb4_command_set_shift_right(rcb4_comm* comm, uint8_t shifts)
{
	assert(comm);
	
	if(comm->type != RCB4_COMM_SHIFT)
	{
		fprintf(stderr, "Invalid command type.\n");
		return -1;
	}
	
	if(shifts > 127)
	{
		fprintf(stderr, "Invalid parameter value. Allowed values [0~127].\n");
		return -1;
	}
	
	if(shifts == 0) // 256 - 0 > 255 so just write a 0
		return rcb4_command_set_shifts(comm, 0);
	
	return rcb4_command_set_shifts(comm, 256 - shifts);
}


int rcb4_command_set_data_size(rcb4_comm* comm, uint8_t size)
{
	uint8_t type;
	rcb4_src_addr* src;
	
	assert(comm);
	
	if(size == 0 || size > 128)
	{
		fprintf(stderr, "Invalid data size.\n");
		return -1;
	}
	
	switch(comm->type)
	{
		case RCB4_COMM_MOV:
			type = comm->command.mov.type;
			src = &comm->command.mov.src;
			break;
		case RCB4_COMM_AND:
		case RCB4_COMM_OR:
		case RCB4_COMM_XOR:
			type = comm->command.logic.type;
			src = &comm->command.logic.src;
			break;
		case RCB4_COMM_NOT:
			comm->command.logic_not.size = size;
			return 0;
		case RCB4_COMM_SHIFT:
			comm->command.shift.size = size;
			return 0;
		case RCB4_COMM_ADD:
		case RCB4_COMM_SUB:
		case RCB4_COMM_MUL:
		case RCB4_COMM_DIV:
		case RCB4_COMM_MOD:
			if(size != 1 && size != 2)
			{
				fprintf(stderr, "Invalid data size.\n");
				return -1;
			}
			type = comm->command.math.type;
			src = &comm->command.math.src;
			break;
		case RCB4_COMM_ICS:
			if(size > 64)
			{
				fprintf(stderr, "Invalid data size.\n");
				return -1;
			}
			comm->command.ics.data_size = size;
			return 0;
		
		// Invalid commands
		case RCB4_COMM_SINGLE:
		case RCB4_COMM_CONST:
		case RCB4_COMM_SERIES:
		case RCB4_COMM_SPEED:
		default:
			fprintf(stderr, "Invalid command type.\n");
			return -1;
	};
	
	switch(type & COMM_SRC_MASK)
	{
		case COMM_SRC_RAM:
			src->ram.size = size;
			break;
		case COMM_SRC_ICS:
			src->ics.size = size;
			break;
		case COMM_SRC_ROM:
			src->rom.size = size;
			break;
		default:
			break;
	}
	
	return 0;
}


int rcb4_command_set_ics(rcb4_comm* comm, uint8_t ics)
{
	assert(comm);
	
	if(comm->type == RCB4_COMM_ICS)
	{
		comm->command.ics.ics_id = ics;
		return 0;
	}
	
	fprintf(stderr, "Invalid command type.\n");
	return -1;
}


int rcb4_command_set_speed(rcb4_comm* comm, uint8_t speed)
{
	assert(comm);
	
	if(speed == 0)
	{
		fprintf(stderr, "Invalid speed value.\n");
		return -1;
	}
	
	// The robot thinks 1 is the fastest speed and 255 the slowest. Change that.
	speed = 256 - (int)speed;
	
	if(comm->type == RCB4_COMM_CONST)
	{
		comm->command.servo_const.speed = speed;
		return 0;
	}
	
	fprintf(stderr, "Invalid command type.\n");
	return -1;
}


int rcb4_command_set_servo(rcb4_comm* comm, uint8_t ics, uint8_t speed, uint16_t position)
{
	int block, new_block, servos, bit, i, overwrite;
	uint8_t check_byte;
	assert(comm);
	
	if(speed == 0 && comm->type != RCB4_COMM_CONST) // RCB4_COMM_CONST ignores the speed
	{
		fprintf(stderr, "Invalid speed value.\n");
		return -1;
	}
	
	if(ics == 0 || ics > RCB4_ICS_QTY)
	{
		fprintf(stderr, "Invalid ics id. Accepted values: [1~%d].\n", RCB4_ICS_QTY);
		return -1;
	}
	
	// The robot thinks 1 is the fastest speed and 255 the slowest. Change that.
	speed = 256 - (int)speed;
	
	if(comm->type == RCB4_COMM_SINGLE)
	{
		comm->command.servo_single.ics_id = ics-1;
		comm->command.servo_single.speed = speed;
		comm->command.servo_single.pos = position; // TODO: Endian??
		return 0;
	}
	
	
	// First there are 5 blocks of 1 byte where each bit decides if the command
	// wants to change that servo.
	// Then there is a variable length list of the servos data. This list *must*
	// be in order, so we have to calculate the position of the new servo inside
	// the list and move the older ones if necessary to make room for it.
	
	if(comm->type == RCB4_COMM_CONST)
	{
		// IGNORE SPEED
		
		new_block = (ics-1) / 8; // Select the corresponding block
		assert(new_block < 5); // Should never happen, but just in case :)
		
		// Get the position (i) of the new servo in the positions list
		overwrite = 0;
		i = 0;
		for(block = 0; block < 5; ++block)
		{
			check_byte = comm->command.servo_const.ics_set[block];
			for(bit = 0; bit < 8 && (block * 8 + bit + 1) <= ics; ++bit) // Loop until we find the current position
			{
				if((check_byte >> bit) & 1) // Is this servo set?
				{
					if(block * 8 + bit + 1 == ics) // The new servo was already set?
					{
						overwrite = 1; // The same servo was already set. Update the old data with the new one
					}
					else
						++i;
				}
			}
		}
		
		if(!overwrite)
		{
			// Move the rest of the list one position to the right
			for(block = RCB4_ICS_QTY - 1; block > i; --block)
			{
				comm->command.servo_const.pos[block] = comm->command.servo_const.pos[block - 1];
			}
		}
		
		// Set the bit on the block for the new servo
		comm->command.servo_const.ics_set[new_block] |= ((uint8_t)1) << ((ics - 1) % 8); // Set the bit
		// Set the position of the new one
		comm->command.servo_const.pos[i] = position; // TODO: Endian??
		
		// Get the number of servos set
		uint8_t cnt[5]; // Speed hack. See http://danluu.com/assembly-intrinsics/
		cnt[0] = __builtin_popcount(comm->command.servo_const.ics_set[0]); // Returns the number of bits set
		cnt[1] = __builtin_popcount(comm->command.servo_const.ics_set[1]);
		cnt[2] = __builtin_popcount(comm->command.servo_const.ics_set[2]);
		cnt[3] = __builtin_popcount(comm->command.servo_const.ics_set[3]);
		cnt[4] = __builtin_popcount(comm->command.servo_const.ics_set[4]);
		servos = cnt[0] + cnt[1] + cnt[2] + cnt[3] + cnt[4];
		
		// Update the size of the message
		comm->size = 9 + 2 * servos;
		
		return 0;
	}
	
	if(comm->type == RCB4_COMM_SERIES)
	{
		new_block = (ics-1) / 8; // Select the corresponding block
		assert(new_block < 5); // Should never happen, but just in case :)
		
		// Get the position (i) of the new servo in the positions list
		overwrite = 0;
		i = 0;
		for(block = 0; block < 5; ++block)
		{
			check_byte = comm->command.servo_series.ics_set[block];
			for(bit = 0; bit < 8 && (block * 8 + bit + 1) <= ics; ++bit)
			{
				if((check_byte >> bit) & 1)
				{
					if(block * 8 + bit + 1 == ics) // The new servo was already set?
					{
						overwrite = 1;
					}
					else
						++i;
				}
			}
		}
		
		if(!overwrite)
		{
			// Move the rest of the list one position to the right
			for(block = RCB4_ICS_QTY - 1; block > i; --block)
			{
				comm->command.servo_series.speedpos[block] = comm->command.servo_series.speedpos[block - 1];
			}
		}
		
		// Set the bit on the block for the new servo
		comm->command.servo_series.ics_set[new_block] |= ((uint8_t)1) << ((ics - 1) % 8); // Set the bit
		// Set the position of the new one
		comm->command.servo_series.speedpos[i].speed = speed;
		// Set the position of the new one
		comm->command.servo_series.speedpos[i].pos = position; // TODO: Endian??
		
		// Get the number of servos set
		uint8_t cnt[5]; // Speed hack. See http://danluu.com/assembly-intrinsics/
		cnt[0] = __builtin_popcount(comm->command.servo_series.ics_set[0]); // Returns the number of bits set
		cnt[1] = __builtin_popcount(comm->command.servo_series.ics_set[1]);
		cnt[2] = __builtin_popcount(comm->command.servo_series.ics_set[2]);
		cnt[3] = __builtin_popcount(comm->command.servo_series.ics_set[3]);
		cnt[4] = __builtin_popcount(comm->command.servo_series.ics_set[4]);
		servos = cnt[0] + cnt[1] + cnt[2] + cnt[3] + cnt[4];
		
		// Update the size of the message
		comm->size = 8 + 3 * servos;
		
		return 0;
	}
	
	
	fprintf(stderr, "Invalid command type.\n");
	return -1;
}


int rcb4_command_set_stretch(rcb4_comm* comm, uint8_t stretch __attribute__((__unused__)))
{
	assert(comm);
	
	fprintf(stderr, "Not implemented.\n");
	return -1;
}


