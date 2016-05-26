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
 * @file rcb4_comm_servo.h
 * @brief Private structures of the servo commands.
 * 
 * @details This header defines all the private structures that define the
 * servo-related commands.
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

#ifndef RCB4_COMM_SERVO_H
#define RCB4_COMM_SERVO_H

#include "rcb4_private.h"

#define RCB4_SERVO_STRETCH_MODE 0x01
#define RCB4_SERVO_SPEED_MODE 0x02

struct s_rcb4_ics
{
	uint8_t ics_id;
	uint8_t data_size;
	uint16_t src;
	uint16_t dst;
}__attribute__((__packed__));

struct s_rcb4_single
{
	uint8_t ics_id;
	uint8_t speed;
	uint16_t pos;
}__attribute__((__packed__));

struct s_rcb4_const
{
	uint8_t ics_set[5];
	uint8_t speed;
	uint16_t pos[RCB4_ICS_QTY];
}__attribute__((__packed__));

struct s_rcb4_speed_pos
{
	uint8_t speed;
	uint16_t pos;
}__attribute__((__packed__));

struct s_rcb4_series
{
	uint8_t ics_set[5];
	struct s_rcb4_speed_pos speedpos[RCB4_ICS_QTY];
}__attribute__((__packed__));

struct s_rcb4_speed
{
	uint8_t ics_set[5];
	uint8_t spst; // RCB4_SERVO_STRETCH_MODE, RCB4_SERVO_SPEED_MODE
	uint8_t parameters[RCB4_ICS_QTY];
}__attribute__((__packed__));


#endif // RCB4_COMM_SERVO_H
