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

void deinit(void)
{
	rcb4_deinit(con);
	
	printf("Exit correctly.\n");
}

int main(int argc, char *argv[])
{
	uint16_t ad_value_1, ad_value_2, ad_value_3;
	
	printf("Connecting to the robot\n");
	con = rcb4_init("/dev/ttyUSB0");
	if(!con)return -1;
	atexit(deinit);
	
	printf("Ping: %d\n", rcb4_command_ping(con));
	
	
	int errors = 0;
	while(errors < 20)
	{
		ad_value_1 = ad_value_2 = ad_value_3 = 0;
		/*if(rcb4_ad_read(con, 0, &ad_value_1) == 0) // Battery
		{
			errors = 0;
		}
		else
		{
			errors++;
		}//*/
		if(rcb4_ad_read(con, 1, &ad_value_2) == 0) // Front / back
		{
			errors = 0;
		}
		else
		{
			errors++;
		}//*/
		if(rcb4_ad_read(con, 2, &ad_value_3) == 0) // Left / right
		{
			errors = 0;
		}
		else
		{
			errors++;
		}//*/
		
		printf("%d, %d, %d\n", ad_value_1, ad_value_2, ad_value_3);
		//rcb4_util_usleep(10000);
	}
	
	
	return 0;
}

