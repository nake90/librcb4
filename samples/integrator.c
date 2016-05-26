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
 * 
 *  Copyright 2015 Alfonso Arbona Gimeno
 */

#include "rcb4.h"
#include "rcb4_config.h" // For RCB4_AD_BASE_ADDR

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

rcb4_connection* con; // Connection to the robot

void deinit(void)
{
	rcb4_deinit(con);
	
	printf("Exit correctly.\n");
}

int fast_ad_read(rcb4_connection* conn, uint16_t* pitch, uint16_t* roll)
{
	rcb4_comm* comm;
	uint16_t value[2];
	
	assert(conn);
	
	comm = rcb4_command_create(RCB4_COMM_MOV);
	if(!comm)
	{
		fprintf(stderr, "Error creating the command.\n");
		return -1;
	}
	
	if(rcb4_command_set_src_ram(comm, RCB4_AD_BASE_ADDR + 2*1, 4) != 0)
	{
		fprintf(stderr, "Error creating the command.\n");
		rcb4_command_delete(comm);
		return -1;
	}
	if(rcb4_command_set_dst_com(comm) != 0)
	{
		fprintf(stderr, "Error creating the command.\n");
		rcb4_command_delete(comm);
		return -1;
	}
	
	// Reply will be 4 bytes (we asked for 4 bytes in rcb4_command_set_src_ram()
	// TODO: Endian...
	if(rcb4_send_command(conn, comm, (uint8_t*)value) != 4)
	{
		fprintf(stderr, "Error sending the command.\n");
		rcb4_command_delete(comm);
		return -1;
	}
	
	rcb4_command_delete(comm);
	
	*pitch = value[0];
	*roll = value[1];
	
	return 0;
}

int main(int argc, char *argv[])
{
	uint16_t pitch, roll;
	int pitch_int = 0, roll_int = 0;
	
	printf("Connecting to the robot\n");
	con = rcb4_init("/dev/ttyUSB0");
	if(!con)return -1;
	atexit(deinit);
	
	printf("Ping: %d\n", rcb4_command_ping(con));
	
	
	int errors = 0;
	while(errors < 20)
	{
		pitch = 271; roll = 270; // Defaults (if the read call fails)
		if(fast_ad_read(con, &pitch, &roll) != 0)
			++errors;
		else
			errors = 0;
		
		pitch_int += (int)pitch - 271; // It seems that the sensor outputs 271 instead of 270 when idle.
		roll_int += (int)roll - 270;
		
		// The precission is quite bad... in a few seconds it has a big offset
		printf("%d, %d\n", pitch_int, roll_int);
		//rcb4_util_usleep(10000);
	}
	
	
	return 0;
}

