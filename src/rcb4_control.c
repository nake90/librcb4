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
 * @file rcb4_control.c
 * @brief Functions to control the flow of a program.
 * 
 * @details These functions allow to call different parts of the code inside the
 * robot.
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
#include "rcb4_connection.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Sends a string of bytes, expects no answer other than the 4 ACK/NACK bytes
static
int rcb4_send_command_private(rcb4_connection* conn, const uint8_t* command, uint8_t length)
{
	int err;
	uint8_t check;
	uint8_t lbuf[256];
	struct timeval timeout; // Timeout
	int rv;
	
	assert(conn);
	
	err = write(conn->fd, command, length);
	if(err != length)
	{
		fprintf(stderr, "Error sending the command. Write error.\n");
		return -1;
	}
	
	
	rcb4_util_usleep(COMM_DELAY_USECS);
	
	
	timeout.tv_sec = 0;
	timeout.tv_usec = COMM_TIMEOUT_USECS;
	
	FD_ZERO(&conn->fdset);
	FD_SET(conn->fd, &conn->fdset);
	rv = select(conn->fd + 1, &conn->fdset, NULL, NULL, &timeout);
	if(rv == -1)
	{
		fprintf(stderr, "Error sending the command. Select failed.\n");
		return -1;
	}
	else if(rv == 0)
	{
		fprintf(stderr, "Error sending the command. Timed out.\n");
		return -1;
	}
	
	
	err = read(conn->fd, lbuf, 4); // 0x04, CMD, ACK|NAK, SUM
	if(err != 4 || lbuf[0] != 0x04 || lbuf[1] != command[1] || lbuf[2] != RCB4_ACK || lbuf[3] != (check = (uint8_t)(0x04 + command[1] + RCB4_ACK)))
	{
		fprintf(stderr, "Error sending the command. Read error.\nReceived %d bytes, msg = 0x%02X, 0x%02X, 0x%02X, 0x%02X\n",
		        err, (err > 0) ? lbuf[0] : 0x00, (err > 1) ? lbuf[1] : 0x00, (err > 2) ? lbuf[2] : 0x00, (err > 3) ? lbuf[3] : 0x00);
		fprintf(stderr, "               Expected 0x04, 0x%02X, 0x%02X, 0x%02X\n", command[1], RCB4_ACK, check);
		return -1;
	}
	
	rcb4_util_usleep(COMM_DELAY_USECS);
	return 0;
}

int rcb4_jmp(rcb4_connection* conn, uint32_t addr, uint8_t conditions)
{
	//               size, comm, (2) , (3) , (4) , (5)
	uint8_t msg[] = {0x07, 0x0B, 0x00, 0x00, 0x00, 0x00};
	
	//TODO: Endian...
	msg[2] = 0xFF & (addr); // Address
	msg[3] = 0xFF & (addr >> 8);
	msg[4] = 0xFF & (addr >> 16);
	
	msg[5] = 0x0F & conditions; // Conditions
	
	// Checksum
	msg[6] = 0xFF & ((int)msg[0] + (int)msg[1] + (int)msg[2] + (int)msg[3] + (int)msg[4] + (int)msg[5]);
	
	return rcb4_send_command_private(conn, msg, sizeof(msg));
}

int rcb4_call(rcb4_connection* conn, uint32_t addr, uint8_t conditions)
{
	//               size, comm, (2) , (3) , (4) , (5) , (6)
	uint8_t msg[] = {0x07, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	//TODO: Endian...
	msg[2] = 0xFF & (addr); // Address
	msg[3] = 0xFF & (addr >> 8);
	msg[4] = 0xFF & (addr >> 16);
	
	msg[5] = 0x0F & conditions; // Conditions
	
	// Checksum
	msg[6] = 0xFF & ((int)msg[0] + (int)msg[1] + (int)msg[2] + (int)msg[3] + (int)msg[4] + (int)msg[5]);
	
	return rcb4_send_command_private(conn, msg, sizeof(msg));
}

int rcb4_ret(rcb4_connection* conn)
{
	//                     size, comm, checksum
	const uint8_t msg[] = {0x03, 0x0D, 0x10};
	
	return rcb4_send_command_private(conn, msg, sizeof(msg));
}

