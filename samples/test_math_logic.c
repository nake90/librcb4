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
	rcb4_command_delete(comm); // If comm is NULL it doesn't do anything
	rcb4_deinit(con);
	
	printf("Program closed.\n");
}

int main(int argc, char *argv[])
{
	uint16_t word, addr, param;
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
	
	addr = 0x048E;
	
	printf("Write.\n");
	rcb4_command_recreate(comm, RCB4_COMM_MOV);
	word = 0x0100;
	printf("%u [%lu bytes] -> 0x%04X@RAM\n", word, sizeof(word), addr);
	rcb4_command_set_src_literal(comm, &word, sizeof(word));
	rcb4_command_set_dst_ram(comm, addr);
	rcb4_send_command(con, comm, NULL);
	
	printf("Read.\n");
	rcb4_command_recreate(comm, RCB4_COMM_MOV);
	rcb4_command_set_src_ram(comm, addr, sizeof(word));
	rcb4_command_set_dst_com(comm);
	if(rcb4_send_command(con, comm, buffer) < 2)
	{
		printf("0x%04X@RAM -> READ ERROR [%lu bytes]\n", addr, sizeof(word));
		return -1;
	}
	else
	{
		word = *(uint16_t*)&buffer; // :D I <3 pointers
		printf("0x%04X@RAM -> %u [%lu bytes]\n", addr, word, sizeof(word));
	}
	
	param = 1;
	printf("SHIFT (0x%04X >> %u).\n", word, param);
	rcb4_command_recreate(comm, RCB4_COMM_SHIFT);
	rcb4_command_set_shift_left(comm, param);
	rcb4_command_set_data_size(comm, sizeof(param));
	rcb4_command_set_dst_ram(comm, addr);
	if(rcb4_send_command(con, comm, buffer) < 2)
	{
		printf("ARITHMETIC -> READ ERROR\n");
		return -1;
	}
	else
	{
		word = *(uint16_t*)&buffer; // :D I <3 pointers
		printf("RESULT -> 0x%04X\n", word);
	}
	
	
	printf("Read.\n");
	rcb4_command_recreate(comm, RCB4_COMM_MOV);
	rcb4_command_set_src_ram(comm, addr, sizeof(word));
	rcb4_command_set_dst_com(comm);
	if(rcb4_send_command(con, comm, buffer) < 2)
	{
		printf("0x%04X@RAM -> READ ERROR [%lu bytes]\n", addr, sizeof(word));
		return -1;
	}
	else
	{
		word = *(uint16_t*)&buffer; // :D I <3 pointers
		printf("0x%04X@RAM -> %u [%lu bytes]\n", addr, word, sizeof(word));
	}
	
	
	return 0;
}

