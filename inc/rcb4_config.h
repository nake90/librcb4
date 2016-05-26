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
 * @file rcb4_config.h
 * @brief Constants that define the board and the protocol used.
 * 
 * @details This header defines all the constants that the board uses.
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

#ifndef RCB4_PRIV_H
#define RCB4_PRIV_H

#define RCB4_ACK 0x06 // Ack byte
#define RCB4_NCK 0x15 // Not ack byte

#define RCB4_MAX_RAM_ADDRESS 0x048F
#define RCB4_MAX_ROM_ADDRESS 0x03FFFF
#define RCB4_MAX_ICS_SRC_SIZE 64
#define RCB4_ICS_QTY 36 // Number of available ICS's
#define RCB4_MAX_ICS_ID (RCB4_ICS_QTY - 1) // ID of the highest ICS
#define RCB4_MAX_AD_ID 10 // ID of the highest analog-digital converter

#define RCB4_AD_BASE_ADDR 0x0022 // RAM base address of the Analog-Digital converter

#define RCB4_COMM_MESSAGE_SIZE_ALLOWED 128 // Max length of a message sent or received

#endif // RCB4_PRIV_H
