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
 * @file rcb4_helpers.c
 * @brief Functions to help the user do common tasks.
 * 
 * @details These functions are not necessary because the library already
 * provides a way to do these tasks but they help develop programs.
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

#include <stdlib.h>
#include <string.h>

int rcb4_ad_read(rcb4_connection* conn, uint8_t ad_id, uint16_t* value)
{
	rcb4_comm* comm;
	
	assert(conn);
	
	if(ad_id > RCB4_MAX_AD_ID)
	{
		fprintf(stderr, "Invalid parameter value. Allowed values [0~%d].\n", RCB4_MAX_AD_ID);
		return -1;
	}
	
	comm = rcb4_command_create(RCB4_COMM_MOV);
	if(!comm)
	{
		fprintf(stderr, "Error creating the command.\n");
		return -1;
	}
	
	if(rcb4_command_set_src_ram(comm, RCB4_AD_BASE_ADDR + 2*ad_id, 2) != 0)
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
	
	// Reply will be 2 bytes (we asked for 2 bytes in rcb4_command_set_src_ram()
	// TODO: Endian...
	if(rcb4_send_command(conn, comm, (uint8_t*)value) != 2)
	{
		fprintf(stderr, "Error sending the command.\n");
		rcb4_command_delete(comm);
		return -1;
	}
	
	rcb4_command_delete(comm);
	
	return 0;
}
