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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

rcb4_connection* con; // Connection to the robot
rcb4_comm* comm = NULL; // Command to be sent

void deinit(void)
{
	rcb4_command_delete(comm);
	rcb4_deinit(con);
	
	printf("Exit correctly.\n");
}

int main(int argc, char *argv[])
{
	uint8_t buffer[256]; // Data from the robot
	int len; // Length of the data
	
	printf("Connecting to the robot\n");
	con = rcb4_init("/dev/ttyUSB0");
	if(!con)return -1;
	atexit(deinit);
	
	printf("Ping: %d\n", rcb4_command_ping(con));
	
	
	printf("Reading system configuration (RAM 0x0000).\n");
	comm = rcb4_command_create(RCB4_COMM_MOV);
	rcb4_command_set_src_ram(comm, 0x0000, 2);
	rcb4_command_set_dst_com(comm);
	if((len = rcb4_send_command(con, comm, buffer)) < 0)
		return -1;
	
	if(len >= 2)printf("Configuration word = 0x%04X.\n", *(uint16_t*)buffer);
	else printf("Could not read the configuration word correctly.\n");
	
	
	printf("Setting a series of servos.\n");
	rcb4_command_recreate(comm, RCB4_COMM_CONST);
	rcb4_command_set_servo(comm, 1, 100, 0x2040);
	rcb4_send_command(con, comm, buffer);
	
	sleep(2);
	
	
	uint16_t pos;
	int errors = 0;
	while(errors < 20)
	{
		for(pos = 0x1900; pos < 0x3200; pos += 0x100)
		{
			rcb4_command_recreate(comm, RCB4_COMM_SINGLE);
			rcb4_command_set_servo(comm, 2, 0xD0, pos);
			printf("Send 0x%04X\t", pos);
			if(rcb4_send_command(con, comm, buffer) < 0)
			{
				printf("ERROR\n");
				errors++;
			}
			else
			{
				printf("OK\n");
				errors = 0;
			}
			
			sleep(1);
		}
		for(pos = 0x3200; pos > 0x1900; pos -= 0x100)
		{
			rcb4_command_recreate(comm, RCB4_COMM_SINGLE);
			rcb4_command_set_servo(comm, 2, 0xD0, pos);
			printf("Send 0x%04X\t", pos);
			if(rcb4_send_command(con, comm, buffer) < 0)
			{
				printf("ERROR\n");
				errors++;
			}
			else
			{
				printf("OK\n");
				errors = 0;
			}
			
			sleep(1);
		}
	}
	
	
	
	return 0;
}

