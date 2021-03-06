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
 * @file rcb4_comm_logic.h
 * @brief Private structures of the logic commands.
 * 
 * @details This header defines all the private structures that define the
 * logical commands.
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

#ifndef RCB4_COMM_LOGIC_H
#define RCB4_COMM_LOGIC_H

#include "rcb4_private.h"

struct s_rcb4_logic
{
	uint8_t type; // Source and destination. COM not allowed
	rcb4_dst_addr dst;
	rcb4_src_addr src;
}__attribute__((__packed__));

struct s_rcb4_not
{
	uint8_t type; // Source and destination. COM not allowed
	rcb4_dst_addr dst;
	uint8_t zero[2]; // Not used. Must be zero.
	uint8_t size;
}__attribute__((__packed__));

struct s_rcb4_shift // Not 'logic' tho... ;)
{
	uint8_t type; // Source and destination. COM not allowed
	rcb4_dst_addr dst;
	uint8_t zero; // Not used. Must be zero.
	uint8_t shifts;
	uint8_t size;
}__attribute__((__packed__));


#endif // RCB4_COMM_LOGIC_H
