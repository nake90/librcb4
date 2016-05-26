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

#define FILTER_SIZE 5

rcb4_connection* con; // Connection to the robot
rcb4_comm* comm = NULL; // Command to be sent

void deinit(void)
{
	rcb4_command_delete(comm);
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
	float pitch, roll;
	uint16_t pitch_f[FILTER_SIZE], roll_f[FILTER_SIZE];
	int filter_pos = 0;
	float pitch_int = 0, roll_int = 0;
	
	printf("Connecting to the robot\n");
	con = rcb4_init("/dev/ttyUSB0");
	if(!con)return -1;
	atexit(deinit);
	
	printf("Ping: %d\n", rcb4_command_ping(con));
	
	comm = rcb4_command_create(RCB4_COMM_CONST);
	
	int i;
	for(i = 0; i < FILTER_SIZE; ++i)
	{
		pitch_f[i] = 271;
		roll_f[i] = 270;
	}
	
	int errors = 0;
	while(errors < 20)
	{
		pitch_f[filter_pos % FILTER_SIZE] = 271;
		roll_f[filter_pos % FILTER_SIZE] = 270; // Defaults (if the read call fails)
		if(fast_ad_read(con, &pitch_f[filter_pos % FILTER_SIZE], &roll_f[filter_pos % FILTER_SIZE]) != 0)
			++errors;
		else
			errors = 0;
		printf("%d, %d\n", pitch_f[filter_pos % FILTER_SIZE], roll_f[filter_pos % FILTER_SIZE]);
		
		pitch = roll = 0;
		for(i = 0; i < FILTER_SIZE; ++i)
		{
			/**
			 * The multiplication part makes the average to be ponderated with
			 * the distance to "filter_pos". That means that if i == filter_pos,
			 * the ponderation will be FILTER_SIZE. if i == filter_pos -1 then
			 * it will be FILTER_SIZE-1, and so on until 1.
			 * 
			 * The objective is to give more weight to the last value, and then
			 * to the second last value obtained, and so on. Making the filter
			 * more responsive than a simple average.
			 */
			
			pitch += pitch_f[i] * (((i - filter_pos + FILTER_SIZE -1) % FILTER_SIZE) + 1);
			roll += roll_f[i]   * (((i - filter_pos + FILTER_SIZE -1) % FILTER_SIZE) + 1);
		}
		
		pitch /= ((((float)FILTER_SIZE) + 1.0)*((float)FILTER_SIZE)) / 2.0; // Arithmetic series sum (1 + 2 + ... + FILTER_SIZE-1)
		roll /= ((((float)FILTER_SIZE) + 1.0)*((float)FILTER_SIZE)) / 2.0;
		
		pitch_int += pitch - 270.95; // Obtained by testing
 		roll_int += roll - 270.08; // Obtained by testing
		
		// The precission is quite bad... in a few seconds it has a big offset
		//printf("%.2f, %.2f\n", pitch_int, roll_int);
		//rcb4_util_usleep(10000);
		
		rcb4_command_recreate(comm, RCB4_COMM_CONST);
		rcb4_command_set_speed(comm, 0x20);
		rcb4_command_set_servo(comm, 19, 0x20, 7500 - pitch_int*70);
		rcb4_command_set_servo(comm, 20, 0x20, 7500 + pitch_int*70);
		rcb4_command_set_servo(comm, 21, 0x20, 7500 + roll_int*70);
		rcb4_command_set_servo(comm, 22, 0x20, 7500 + roll_int*70);
		rcb4_send_command(con, comm, NULL);
		
		
		++filter_pos;
		filter_pos %= FILTER_SIZE;
	}
	
	
	return 0;
}

