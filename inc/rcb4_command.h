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
 * @file rcb4_command.h
 * @brief Private functions for the commands and its main hidden structure.
 * 
 * @details This header defines the real command structure and the main
 * functions to use it.
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

#ifndef RCB4_COMMAND_H
#define RCB4_COMMAND_H

#include "rcb4.h"
#include "rcb4_private.h"

// Structures
#include "rcb4_comm_mov.h"
#include "rcb4_comm_logic.h"
#include "rcb4_comm_math.h"
#include "rcb4_comm_servo.h"

struct s_rcb4_comm
{
	uint8_t size;
	uint8_t type;
	union
	{
		struct s_rcb4_mov    mov;          // MOVE
		struct s_rcb4_logic  logic;        // AND, OR, XOR
		struct s_rcb4_not    logic_not;    // NOT
		struct s_rcb4_shift  shift;        // RSHIFT, LSHIFT
		struct s_rcb4_math   math;         // +, -, *, /, %
		struct s_rcb4_ics    ics;          // ICS
		struct s_rcb4_single servo_single; // Single servo command
		struct s_rcb4_const  servo_const;  // Multiple servos, single speed
		struct s_rcb4_series servo_series; // Multiple servos, multiple speeds
		struct s_rcb4_speed  servo_speed;  // Speed, stretch
	}command;
} __attribute__((__packed__));

// Private functions
uint8_t rcb4_command_calculate_checksum(const rcb4_comm* comm);
uint8_t rcb4_command_get_response_size(const rcb4_comm* comm);


#endif // RCB4_COMMAND_H

