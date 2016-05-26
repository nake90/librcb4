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

#include "rcb4_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>


#ifdef AUTO_TESTS_WRITTEN // I'm working on it, but it's long and I'm not sure if necessary
uint8_t get_random_8(void){return rand() % 256;}
uint32_t get_random_32max(uint32_t max){return rand() % max;}

enum test_src
{
	TEST_SRC_RAM,
	TEST_SRC_ICS,
	TEST_SRC_LIT,
	TEST_SRC_ROM,
	
	TEST_SRC_QTY
};

enum test_dst
{
	TEST_DST_RAM,
	TEST_DST_ICS,
	TEST_DST_COM,
	TEST_DST_ROM,
	
	TEST_DST_QTY
};

// Sets the source of the message to a random valid value. Writes the expected
// output bytes to result and returns the number of bytes written to result.
int set_src(rcb4_comm* msg, enum test_src src_type, uint8_t* result)
{
	uint16_t addr16_src;
	uint32_t addr32_src;
	uint8_t ics_src, offset_src, data_size;
	uint8_t *literal;
	
	data_size = get_random_32max(RCB4_COMM_MESSAGE_SIZE_ALLOWED) + 1; // 1 ~ RCB4_COMM_MESSAGE_SIZE_ALLOWED
	
	switch(src_type)
	{
		case TEST_SRC_RAM:
			addr16_src = get_random_32max(RCB4_MAX_RAM_ADDRESS +1); // 0 ~ RCB4_MAX_RAM_ADDRESS
			if(rcb4_command_set_src_ram(msg, addr16_src, data_size) != 0)
			{
				fprintf(stderr, "Error setting TEST_SRC_RAM\n");
				return -1;
			}
			
}

int test_mov(void)
{
	unsigned int passed = 0;
	
	// Variables
	uint16_t addr16_src, addr16_dst;
	uint32_t addr32_src, addr32_dst;
	uint8_t ics_src, ics_dst, offset_src, offset_dst, data_size;
	uint8_t *literal;
	
	
	// -------- TEST -------- //
	printf("MOV RAM -> RAM\t");
	rcb4_comm* msg = rcb4_command_create(RCB4_COMM_MOV);
	if(msg)
	{
		
		
		rcb4_command_delete(msg);
		passed++;
	}
	else
	{
		fprintf(stderr, "Error creating the message: RCB4_COMM_MOV\n");
	}
	
	
	
	
	return passed;
}

#endif // AUTO_TESTS_WRITTEN

int main(int argc, char *argv[])
{
	srand(time(NULL));
	
	printf("Testing the message creation\n");
	rcb4_comm* msg = rcb4_command_create(RCB4_COMM_SERIES);
	
	rcb4_command_set_servo(msg, 2, 0x82, 0x5544);
	rcb4_command_set_servo(msg, 10, 0x8A, 0x1111);
	rcb4_command_set_servo(msg, 1, 0x81, 0xAA88);
	rcb4_command_set_servo(msg, 2, 0x82, 0xBB99);
	rcb4_command_set_servo(msg, 1, 0x81, 0xFFFF);
	
	
	printf("Message contents:\n");
	rcb4_command_debug_print(msg);
	
	
	rcb4_command_delete(msg);
	return 0;
}
