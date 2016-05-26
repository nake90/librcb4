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
 * @file rcb4_private.h
 * @brief Private definitions and structures shared in the library.
 * 
 * @details This header defines the structures and constants used in the library.
 * 
 * @version 1.0
 * @date November 2015
 * 
 * @author Alfonso Arbona Gimeno (alargi@etsii.upv.es, alf@katolab.nitech.ac.jp)
 * KATOLAB, Nagoya Institute of Technology.
 * 
 * @copyright Copyright 2015 Alfonso Arbona Gimeno.
 * This project is released under the GNU Public License v3.
 * See COPYING for a full copy of the license.
 */

#ifndef RCB4_PRIVATE_H
#define RCB4_PRIVATE_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>


#include "rcb4.h" // Typedefs and public functions
#include "rcb4_config.h" // Definitions for the RCB4

#define COMM_TIMEOUT_USECS 500000 // Timeout in microseconds to receive an answer from the robot
#define COMM_DELAY_USECS   50000 // Delay in microseconds from command to command
#define COMM_LITERAL_MAX_LEN (RCB4_COMM_MESSAGE_SIZE_ALLOWED-7)


// Destination types
#define COMM_DST_MASK 0x30
#define COMM_DST_RAM 0x00
#define COMM_DST_ICS 0x10
#define COMM_DST_COM 0x20
#define COMM_DST_ROM 0x30
// Source types
#define COMM_SRC_MASK 0x03
#define COMM_SRC_RAM 0x00
#define COMM_SRC_ICS 0x01
#define COMM_SRC_LIT 0x02
#define COMM_SRC_ROM 0x03
// Don't update flag
#define COMM_NUPDATE 0x80

union u_rcb4_dst_addr
{
	struct __attribute__((__packed__))
	{
		uint8_t addr[2];
	}ram;

	struct __attribute__((__packed__))
	{
		uint8_t offset;
		uint8_t ics;
	}ics;

	struct __attribute__((__packed__))
	{
		uint8_t addr[3];
	}rom;
}__attribute__((__packed__));


union u_rcb4_src_addr
{
	struct __attribute__((__packed__))
	{
		uint8_t addr[2];
		uint8_t size;
	}ram;

	struct __attribute__((__packed__))
	{
		uint8_t offset;
		uint8_t ics;
		uint8_t size;
	}ics;
		
	uint8_t lit[COMM_LITERAL_MAX_LEN];

	struct __attribute__((__packed__))
	{
		uint8_t addr[3];
		uint8_t size;
	}rom;
}__attribute__((__packed__));

typedef union u_rcb4_dst_addr rcb4_dst_addr;
typedef union u_rcb4_src_addr rcb4_src_addr;


#endif // RCB4_PRIVATE_H

 
