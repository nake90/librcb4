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
 * @file rcb4_comm_mov.h
 * @brief Private structures of the move commands.
 * 
 * @details This header defines all the private structures that define the
 * move (copy) commands.
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

#ifndef RCB4_COMM_MOV_H
#define RCB4_COMM_MOV_H

struct s_rcb4_mov
{
	uint8_t type; // Source and destination. 7th bit not allowed
	rcb4_dst_addr dst;
	rcb4_src_addr src;
}__attribute__((__packed__));


#endif // RCB4_COMM_MOV_H
