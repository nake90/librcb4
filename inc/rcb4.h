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
 * @mainpage
 * 
 * @version 1.0
 * @date November 2015
 * 
 * librcb4 is an open source linux library to control the board RCB4 from KONDO
 * used in the KHR-3 humanoid robot.
 * 
 * It is able to read and write to RAM, ROM and ICS; perform logical and
 * arithmetic operations; execute jump, call and return functions, control
 * all the servos and read data from the sensors.
 * 
 * It has been written from scratch using only the information in the datasheet
 * (freely downloadable from KONDO's website, in Japanese), so it is safe to
 * use for any purpose that the GNU-GPLv3 allows.
 * 
 * 
 * To compile just call @c make in the root folder.
 * 
 * To use just include the file rcb4.h (in the 'inc' folder) to your project,
 * and add the library to your linker.
 * For example:
 * @code{.sh}
 * gcc -I/path/to/librcb4/inc -L/path/to/librcb4/lib -lrcb4 -o my_program my_source.c
 * @endcode
 * 
 * Remember to change the path to the real path where you have installed the
 * library.
 * 
 * To compile all sample projects <tt>make samples</tt> and to compile the
 * documentation do <tt>make docs</tt>.
 * 
 * To read the documentation just open index.html in the doc/html folder.
 * 
 * 
 * You can read the whole documentation in the rcb4.h documentation file.
 * 
 * 
 * @author Alfonso Arbona Gimeno (alargi@etsii.upv.es, alf@katolab.nitech.ac.jp)
 * KATOLAB, Nagoya Institute of Technology.
 * 
 * @copyright Copyright 2015 Alfonso Arbona Gimeno.
 * This project is released under the GNU Public License v3.
 * See COPYING for a full copy of the license.
 */

/**
 * @file rcb4.h
 * @brief Main header of librcb4.
 * 
 * @details This header defines all the functions available to an external
 * program using the library.
 * These are: Creating and destroying a connection, creating and sending
 * a command message, and a few helper functions.
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



#ifndef RCB4_H
#define RCB4_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Private structure that holds all the connection information.
 * 
 * The structure must be created using rcb4_init() and deleted using
 * rcb4_deinit() when you are no longer going to use it.
 * Once created you can send the message to the robot using rcb4_send_command().
 * 
 * @sa rcb4_init(), rcb4_deinit(), rcb4_send_command()
 */
typedef struct s_rcb4_connection rcb4_connection;

/**
 * @brief Private structure that holds the data for a command message.
 * 
 * The structure must be created using rcb4_command_create() and deleted using
 * rcb4_command_delete() when you are no longer going to use it.
 * Once used you must not reuse the variable. Create a new one instead.
 * 
 * Not all functions are available for all command types. Check the different
 * functions for more details.
 * 
 * @sa rcb4_command_create(), rcb4_command_delete(), rcb4_send_command(), enum e_rcb4_command_types
 */
typedef struct s_rcb4_comm rcb4_comm;


enum e_rcb4_command_types
{
	/**
	 * @brief Copy a value from source to destination.
	 * 
	 * Source must be set with rcb4_command_set_src_*() functions to be RAM,
	 * ICS, a literal value or ROM.
	 * 
	 * Destination must be set with rcb4_command_set_dst_*() functions to RAM,
	 * ICS, COM or ROM, but rcb4_command_set_dst_do_not_save() can't be used.
	 * 
	 * If destination is set to be COM the result will be saved in the reply
	 * argument in rcb4_send_command(). The size of the reply is the same as
	 * asked in the rcb4_command_set_src_*() functions.
	 * 
	 * @warning Only the rcb4_command_set_src_* and the rcb4_command_set_dst_*()
	 * functions are allowed.
	 * @warning rcb4_command_set_dst_do_not_save() is not allowed.
	 */
	RCB4_COMM_MOV    = 0x00,
	
	/**
	 * @brief Saves the result of the bitwise operation (source AND destination)
	 * into destination.
	 * 
	 * Source must be set with rcb4_command_set_src_*() functions to be RAM,
	 * ICS, a literal value or ROM.
	 * 
	 * Destination must be set with rcb4_command_set_dst_*() functions to RAM,
	 * ICS or ROM, but rcb4_command_set_dst_com() can't be used.
	 * 
	 * rcb4_command_set_dst_do_not_save() can be called to avoid saving the
	 * result in destination. That will, however, set the corresponding CPU
	 * flags.
	 * Note that you still need to set the destination using another
	 * rcb4_command_set_dst_*() function because the operation is still
	 * (source AND destination), even if the result is not saved.
	 * 
	 * @warning Only the rcb4_command_set_src_* and the rcb4_command_set_dst_*()
	 * functions are allowed.
	 * @warning rcb4_command_set_dst_com() is not allowed.
	 */
	RCB4_COMM_AND    = 0x01,
	
	/**
	 * @brief Saves the result of the bitwise operation (source OR destination)
	 * into destination.
	 * 
	 * Source must be set with rcb4_command_set_src_*() functions to be RAM,
	 * ICS, a literal value or ROM.
	 * 
	 * Destination must be set with rcb4_command_set_dst_*() functions to RAM,
	 * ICS or ROM, but rcb4_command_set_dst_com() can't be used.
	 * 
	 * rcb4_command_set_dst_do_not_save() can be called to avoid saving the
	 * result in destination. That will, however, set the corresponding CPU
	 * flags.
	 * Note that you still need to set the destination using another
	 * rcb4_command_set_dst_*() function because the operation is still
	 * (source OR destination), even if the result is not saved.
	 * 
	 * @warning Only the rcb4_command_set_src_* and the rcb4_command_set_dst_*()
	 * functions are allowed.
	 * @warning rcb4_command_set_dst_com() is not allowed.
	 */
	RCB4_COMM_OR     = 0x02,
	
	/**
	 * @brief Saves the result of the bitwise operation (source XOR destination)
	 * into destination.
	 * 
	 * Source must be set with rcb4_command_set_src_*() functions to be RAM,
	 * ICS, a literal value or ROM.
	 * 
	 * Destination must be set with rcb4_command_set_dst_*() functions to RAM,
	 * ICS or ROM, but rcb4_command_set_dst_com() can't be used.
	 * 
	 * rcb4_command_set_dst_do_not_save() can be called to avoid saving the
	 * result in destination. That will, however, set the corresponding CPU
	 * flags.
	 * Note that you still need to set the destination using another
	 * rcb4_command_set_dst_*() function because the operation is still
	 * (source XOR destination), even if the result is not saved.
	 * 
	 * @warning Only the rcb4_command_set_src_* and the rcb4_command_set_dst_*()
	 * functions are allowed.
	 * @warning rcb4_command_set_dst_com() is not allowed.
	 */
	RCB4_COMM_XOR    = 0x03,
	
	/**
	 * @brief Saves the result of the bitwise operation (NOT destination) into
	 * destination.
	 * 
	 * Destination must be set with rcb4_command_set_dst_*() functions to be
	 * RAM, ICS or ROM.
	 * 
	 * The size of the variable must be set with rcb4_command_set_data_size().
	 * 
	 * rcb4_command_set_dst_do_not_save() can be called to avoid saving the
	 * result in destination. That will, however, set the corresponding CPU
	 * flags.
	 * Note that you still need to set the destination using another
	 * rcb4_command_set_dst_*() function because the operation is still
	 * (NOT destination), even if the result is not saved.
	 * 
	 * @warning Only the rcb4_command_set_dst_*() functions and
	 * rcb4_command_set_data_size() are allowed.
	 * @warning rcb4_command_set_dst_com() is not allowed.
	 */
	RCB4_COMM_NOT    = 0x04,
	
	/**
	 * @brief Saves the result of the bitwise operation (destination << shifts)
	 * into destination. Shifts can be negative for right shifts.
	 * 
	 * Destination must be set with rcb4_command_set_dst_*() functions to be
	 * RAM, ICS or ROM.
	 * 
	 * The size of the variable must be set with rcb4_command_set_data_size(),
	 * and the number of shifts must be chosen using rcb4_command_set_shifts().
	 * 
	 * rcb4_command_set_dst_do_not_save() can be called to avoid saving the
	 * result in destination. That will, however, set the corresponding CPU
	 * flags.
	 * Note that you still need to set the destination using another
	 * rcb4_command_set_dst_*() function because the operation is still
	 * (SHIFT destination), even if the result is not saved.
	 * 
	 * @warning Only the rcb4_command_set_dst_*() functions,
	 * rcb4_command_set_data_size() and rcb4_command_set_shift_*() functions are
	 * allowed.
	 * @warning rcb4_command_set_dst_com() is not allowed.
	 */
	RCB4_COMM_SHIFT  = 0x05,
	
	/**
	 * @brief Saves the result of the arithmetic operation (destination +
	 * source) into destination.
	 * 
	 * Source must be set with rcb4_command_set_src_*() functions to be RAM,
	 * ICS, a literal value or ROM.
	 * 
	 * Destination must be set with rcb4_command_set_dst_*() functions to RAM,
	 * ICS or ROM, but rcb4_command_set_dst_com() can't be used.
	 * 
	 * rcb4_command_set_dst_do_not_save() can be called to avoid saving the
	 * result in destination. That will, however, set the corresponding CPU
	 * flags.
	 * Note that you still need to set the destination using another
	 * rcb4_command_set_dst_*() function because the operation is still
	 * (source AND destination), even if the result is not saved.
	 * 
	 * The result will always be sent to COM too.
	 * 
	 * @warning Only the rcb4_command_set_src_* and the rcb4_command_set_dst_*()
	 * functions are allowed.
	 * @warning rcb4_command_set_dst_com() is not allowed.
	 */
	RCB4_COMM_ADD    = 0x06,
	
	/**
	 * @brief Saves the result of the arithmetic operation (destination -
	 * source) into destination.
	 * 
	 * Same rules as in RCB4_COMM_ADD apply.
	 * 
	 * @sa RCB4_COMM_ADD
	 */
	RCB4_COMM_SUB    = 0x07,
	
	/**
	 * @brief Saves the result of the arithmetic operation (destination *
	 * source) into destination.
	 * 
	 * Same rules as in RCB4_COMM_ADD apply.
	 * 
	 * @sa RCB4_COMM_ADD
	 */
	RCB4_COMM_MUL    = 0x08,
	
	/**
	 * @brief Saves the result of the arithmetic operation (destination /
	 * source) into destination.
	 * 
	 * Same rules as in RCB4_COMM_ADD apply.
	 * 
	 * @sa RCB4_COMM_ADD
	 */
	RCB4_COMM_DIV    = 0x09,
	
	/**
	 * @brief Saves the result of the arithmetic operation (destination %
	 * source) into destination.
	 * 
	 * Same rules as in RCB4_COMM_ADD apply.
	 * 
	 * @sa RCB4_COMM_ADD
	 */
	RCB4_COMM_MOD    = 0x0A,
	
	// TODO: Write this
	/**
	 * @brief Sets ICS using data already loaded in RAM.
	 * 
	 * @warning TO BE WRITTEN!!!!
	 * 
	 * Source must be set with rcb4_command_set_src_*() functions to be RAM,
	 * ICS, a literal value or ROM.
	 * 
	 * Destination must be set with rcb4_command_set_dst_*() functions to RAM,
	 * ICS or ROM, but rcb4_command_set_dst_com() can't be used.
	 * 
	 * rcb4_command_set_dst_do_not_save() can be called to avoid saving the
	 * result in destination. That will, however, set the corresponding CPU
	 * flags.
	 * Note that you still need to set the destination using another
	 * rcb4_command_set_dst_*() function because the operation is still
	 * (source AND destination), even if the result is not saved.
	 * 
	 * @warning Only the rcb4_command_set_src_* and the rcb4_command_set_dst_*()
	 * functions are allowed.
	 * @warning rcb4_command_set_dst_com() is not allowed.
	 */
	RCB4_COMM_ICS    = 0x0E,
	
	/**
	 * @brief Moves a single servo.
	 * 
	 * You must set the servo, it's speed and position using the function
	 * rcb4_command_set_servo().
	 * Position goes from 0 to 0xFFFF and speed from 1 to 255.
	 * 
	 * @warning Only the rcb4_command_set_servo() function is allowed.
	 * 
	 * @sa rcb4_command_set_servo()
	 */
	RCB4_COMM_SINGLE = 0x0F,
	
	/**
	 * @brief Moves a series of servos all at the same speed.
	 * 
	 * You must set the speed that all servos will use by calling the function
	 * rcb4_command_set_speed(), and then call rcb4_command_set_servo() once
	 * for each servo you want to move. The speed parameter of the function is
	 * ignored.
	 * Position goes from 0 to 0xFFFF and speed from 1 to 255.
	 * 
	 * @warning Only the rcb4_command_set_speed() and rcb4_command_set_servo()
	 * functions are allowed.
	 * 
	 * @sa rcb4_command_set_speed(), rcb4_command_set_servo()
	 */
	RCB4_COMM_CONST  = 0x10,
	
	/**
	 * @brief Moves a series of servos each with a different speed.
	 * 
	 * For each servo you want to move, call rcb4_command_set_servo()
	 * Position goes from 0 to 0xFFFF and speed from 1 to 255.
	 * 
	 * @warning Only the rcb4_command_set_servo() functions are allowed.
	 * 
	 * @warning <b>Doesn't seem to work. Resets the robot.</b>
	 * 
	 * @sa rcb4_command_set_servo()
	 */
	RCB4_COMM_SERIES = 0x11,
	
	/**
	 * @brief Sets the speed or the stretch of a series of servos.
	 * 
	 * @warning This function is not well documented so <b>it has not been
	 * implemented yet</b>.
	 */
	RCB4_COMM_SPEED  = 0x12  // NOTE: Docummentation says 0x11. Maybe it is 0x12? ****Not implemented for now****
};


/*******************************************************************************
 * Public functions                                                            *
 *******************************************************************************/


/**************
 * CONNECTION *
 **************/

/**
 * @brief Opens and setups a connection to the robot using the serial interface.
 * 
 * This function tries to open the serial device tty for read/write and setup
 * the serial to work with the robot. After that it tries to guess the speed
 * of the robot by issuing pings to it. If the ping succeeds it returns, if the
 * ping fails it tries another speed.
 * 
 * After opening the device you must call rcb4_deinit() to close and restore the
 * old configuration of the serial, and to free the memory.
 * 
 * The speed setting of the serial are tampered to allow linux to use the serial
 * at high speeds so it is imperative to call rcb4_deinit() to revert the
 * configuration to its initial state.
 * 
 * @param tty is the device to connect. Usually "/dev/ttyUSB0".
 * @return A new allocated rcb4_connection structure or NULL if something
 * failed (memory or connection).
 * @sa rcb4_deinit().
 */
rcb4_connection* rcb4_init(const char* tty); //! Creates a new connection to the robot. Returns NULL on failure. Tries to guess the baudrate.

/**
 * @brief Closes and resets the serial port and frees conn.
 * 
 * This function deinitializes the connection by restoring the original serial
 * configuration, closing the connection and freeing the memory used by conn.
 * conn must not be used from here on unless it is recreated by calling
 * rcb4_init() again.
 * 
 * This function must be called always in order to reconfigure the serial.
 * 
 * @param conn is the connection to the robot.
 * @sa rcb4_init().
 */
void rcb4_deinit(rcb4_connection* conn); // Disconnects, reconfigures the terminal and frees the connection. You must call this function when finished.

/**
 * @brief Sends a ping command to the robot to see if the connection is still
 * operative.
 * 
 * @param conn is the connection to the robot.
 * @return 0 if the robot answered correctly (ACK).
 * @return 1 if the robot answered with an error message (NACK).
 * @return -10 if there was a timeout.
 * @return < 0 if there was an error.
 */
int rcb4_command_ping(rcb4_connection* conn); // 0 = ACK, 1 = NACK, < 0 = Error (-10 = timeout)


/************
 * COMMANDS *
 ************/
// Return 0 on success.

//------------------------------//
// Create a new command message //
//------------------------------//

/**
 * @brief Creates a new command.
 * 
 * This function allocates the memory for a new command of the selected type.
 * Remember to free the memory by calling rcb4_command_delete() when you are
 * finished with the command.
 * 
 * If you want to send more than one command using the same variable, instead of
 * calling rcb4_command_create() and rcb4_command_delete() each time you can
 * call rcb4_command_create() the first time to allocate the variable and then
 * rcb4_command_recreate() each time you want a new empty command (it doesn't
 * even have to be the same type). Remember to call rcb4_command_delete() once
 * at the end when you are finished with the variable.
 * 
 * @param type is the type of command to create.
 * @return A new rcb4_comm structure.
 * @return NULL on error.
 * @sa e_rcb4_command_types, rcb4_command_recreate(), rcb4_command_delete().
 */
rcb4_comm* rcb4_command_create(enum e_rcb4_command_types type); // Create a new command. Returns NULL on error.

/**
 * @brief Resets an already allocated command.
 * 
 * This function clears a command and sets its type. It does <b>NOT allocate</b>
 * the variable so you can only use it to reset an already created command, for
 * example when you have already sent the command.
 * 
 * If you want to send more than one command using the same variable, instead of
 * calling rcb4_command_create() and rcb4_command_delete() each time you can
 * call rcb4_command_create() the first time to allocate the variable and then
 * rcb4_command_recreate() each time you want a new empty command (it doesn't
 * even have to be the same type). Remember to call rcb4_command_delete() once
 * at the end when you are finished with the variable.
 * 
 * @param comm is the already allocated command variable to be reset.
 * @param type is the type of command to create.
 * @return 0 on success.
 * @sa e_rcb4_command_types, rcb4_command_create(), rcb4_command_delete().
 */
int rcb4_command_recreate(rcb4_comm* comm, enum e_rcb4_command_types type); // Clears a command to be used again in another message.

/**
 * @brief Frees the memory allocated for the command.
 * 
 * This function deletes the memory used by the command. You must call it every
 * time you use rcb4_command_create() and don't want to use that variable again.
 * 
 * @param comm is the command to free.
 * @sa rcb4_command_create().
 */
void rcb4_command_delete(rcb4_comm* comm); // Frees the command

//--------//
// Source //
//--------//

/**
 * @brief Sets the source of the data to be a RAM address of the robot.
 * 
 * This function sets the source of the data to be a value in the RAM inside the
 * robot and specifies the size of the data.
 * 
 * Can only be set for MOV, logic and arithmetic messages (except NOT and SHIFT).
 * 
 * Full list of commands accepted: RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV and
 * RCB4_COMM_MOD.
 * 
 * Note that RCB4_COMM_NOT and RCB4_COMM_SHIFT are not allowed so instead you'll
 * have to call rcb4_command_set_data_size() to set the size of the data.
 * 
 * @param comm is the allocated command to set.
 * @param addr is the address of the RAM from where to take the data. [0~0x048F]
 * @param size is the size in bytes of the data.
 * @return 0 on success.
 * @sa rcb4_command_create(), RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV,
 * RCB4_COMM_MOD.
 */
int rcb4_command_set_src_ram(rcb4_comm* comm, uint16_t addr, uint8_t size);

/**
 * @brief Sets the source of the data to be an ICS block of the robot.
 * 
 * This function sets the source of the data to be an ICS block inside the
 * robot and specifies the size of the data.
 * 
 * Can only be set for MOV, logic and arithmetic messages (except NOT and SHIFT).
 * 
 * Full list of commands accepted: RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV and
 * RCB4_COMM_MOD.
 * 
 * Note that RCB4_COMM_NOT and RCB4_COMM_SHIFT are not allowed so instead you'll
 * have to call rcb4_command_set_data_size() to set the size of the data.
 * 
 * @param comm is the allocated command to set.
 * @param offset is the offset. [0~0xFF]
 * @param ics is the ics id. [0~35]
 * @param size is the size in bytes of the data.
 * @return 0 on success.
 * @sa rcb4_command_create(), RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV,
 * RCB4_COMM_MOD.
 */
int rcb4_command_set_src_ics(rcb4_comm* comm, uint8_t offset, uint8_t ics, uint8_t size);

/**
 * @brief Sets the source of the data to be a literal value.
 * 
 * This function sets the source of the data to be a literal value inside the
 * COM message and specifies the size of the data.
 * 
 * Can only be set for MOV, logic and arithmetic messages (except NOT and SHIFT).
 * 
 * Full list of commands accepted: RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV and
 * RCB4_COMM_MOD.
 * 
 * Note that RCB4_COMM_NOT and RCB4_COMM_SHIFT are not allowed so instead you'll
 * have to call rcb4_command_set_data_size() to set the size of the data.
 * 
 * @param comm is the allocated command to set.
 * @param literal is a pointer to the data to send. Note that numbers are little endian.
 * @param length is the size in bytes of the literal. [1~128]
 * @return 0 on success.
 * @sa rcb4_command_create(), RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV,
 * RCB4_COMM_MOD.
 */
int rcb4_command_set_src_literal(rcb4_comm* comm, const void* literal, uint8_t length);

/**
 * @brief Sets the source of the data to be an address in the ROM memory.
 * 
 * This function sets the source of the data to be a value inside the
 * ROM memory and specifies the size of the data.
 * 
 * Can only be set for MOV, logic and arithmetic messages (except NOT and SHIFT).
 * 
 * Full list of commands accepted: RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV and
 * RCB4_COMM_MOD.
 * 
 * Note that RCB4_COMM_NOT and RCB4_COMM_SHIFT are not allowed so instead you'll
 * have to call rcb4_command_set_data_size() to set the size of the data.
 * 
 * @param comm is the allocated command to set.
 * @param addr is the address of the ROM from where to take the data. [0~0x03FFFF]
 * @param size is the size in bytes of the data.
 * @return 0 on success.
 * @sa rcb4_command_create(), RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV,
 * RCB4_COMM_MOD.
 */
int rcb4_command_set_src_rom(rcb4_comm* comm, uint32_t addr, uint8_t size);




//-------------//
// Destination //
//-------------//


/**
 * @brief Sets the destination of the data to be an address in the RAM memory.
 * 
 * This function sets the destination of the data to be a value inside the
 * RAM memory.
 * 
 * Can only be set for MOV, logic and arithmetic messages (including NOT and
 * SHIFT).
 * 
 * Full list of commands accepted: RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_NOT, RCB4_COMM_SHIFT, RCB4_COMM_ADD, RCB4_COMM_SUB,
 * RCB4_COMM_MUL, RCB4_COMM_DIV and RCB4_COMM_MOD.
 * 
 * If you don't want to write the results to this destination you can later call
 * rcb4_command_set_dst_do_not_save().
 * 
 * @param comm is the allocated command to set.
 * @param addr is the address of the RAM from where to save the data. [0~0x048F]
 * @return 0 on success.
 * @sa rcb4_command_create(), rcb4_command_set_dst_do_not_save(), RCB4_COMM_MOV,
 * RCB4_COMM_NOT, RCB4_COMM_SHIFT, RCB4_COMM_AND, RCB4_COMM_OR, RCB4_COMM_XOR,
 * RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV, RCB4_COMM_MOD.
 */
int rcb4_command_set_dst_ram(rcb4_comm* comm, uint16_t addr);

/**
 * @brief Sets the destination of the data to be an ICS block.
 * 
 * This function sets the destination of the data to be an ICS block inside the
 * robot.
 * 
 * Can only be set for MOV, logic and arithmetic messages (including NOT and
 * SHIFT).
 * 
 * Full list of commands accepted: RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_NOT, RCB4_COMM_SHIFT, RCB4_COMM_ADD, RCB4_COMM_SUB,
 * RCB4_COMM_MUL, RCB4_COMM_DIV and RCB4_COMM_MOD.
 * 
 * If you don't want to write the results to this destination you can later call
 * rcb4_command_set_dst_do_not_save().
 * 
 * @param comm is the allocated command to set.
 * @param offset is the offset in bytes.
 * @param ics is the ID of the ics block. [0~35]
 * @sa rcb4_command_create(), rcb4_command_set_dst_do_not_save(), RCB4_COMM_MOV,
 * RCB4_COMM_NOT, RCB4_COMM_SHIFT, RCB4_COMM_AND, RCB4_COMM_OR, RCB4_COMM_XOR,
 * RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV, RCB4_COMM_MOD.
 */
int rcb4_command_set_dst_ics(rcb4_comm* comm, uint8_t offset, uint8_t ics);

/**
 * @brief Sets the destination of the data to be COM.
 * 
 * This function sets the destination of the data to be the serial port, so the
 * value can be readed as the reply to the rcb4_send_command() function.
 * 
 * Can only be set for MOV. The logical and arithmetic commands are not allowed
 * to use this function because they always send a copy of the result to COM.
 * 
 * Full list of commands accepted: RCB4_COMM_MOV.
 * 
 * @param comm is the allocated command to set.
 * @return 0 on success.
 * @sa rcb4_command_create(), RCB4_COMM_MOV.
 */
int rcb4_command_set_dst_com(rcb4_comm* comm);

/**
 * @brief Sets the destination of the data to be an address in the ROM memory.
 * 
 * This function sets the destination of the data to be a value inside the
 * ROM memory.
 * 
 * Can only be set for MOV, logic and arithmetic messages (including NOT and
 * SHIFT).
 * 
 * Full list of commands accepted: RCB4_COMM_MOV, RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_NOT, RCB4_COMM_SHIFT, RCB4_COMM_ADD, RCB4_COMM_SUB,
 * RCB4_COMM_MUL, RCB4_COMM_DIV and RCB4_COMM_MOD.
 * 
 * If you don't want to write the results to this destination you can later call
 * rcb4_command_set_dst_do_not_save().
 * 
 * @param comm is the allocated command to set.
 * @param addr is the address of the ROM from where to save the data. [0~0x03FFFF]
 * @return 0 on success.
 * @sa rcb4_command_create(), rcb4_command_set_dst_do_not_save(), RCB4_COMM_MOV,
 * RCB4_COMM_NOT, RCB4_COMM_SHIFT, RCB4_COMM_AND, RCB4_COMM_OR, RCB4_COMM_XOR,
 * RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL, RCB4_COMM_DIV, RCB4_COMM_MOD.
 */
int rcb4_command_set_dst_rom(rcb4_comm* comm, uint32_t addr);

/**
 * @brief Tells the processor not to really save the data to the destination.
 * 
 * Calling this function makes the results of logical and arithmetic operations
 * not to be written to destination.
 * 
 * This is useful if you just want to get the results and not save them in the
 * robot (you can always read the result as the reply to rcb4_send_command()).
 * 
 * The zero and carry flags will be set acordingly even if this function is
 * called.
 * 
 * Can only be set for logic and arithmetic messages (including NOT and
 * SHIFT, <b>but MOV is not allowed</b>).
 * 
 * Full list of commands accepted: RCB4_COMM_AND, RCB4_COMM_OR,
 * RCB4_COMM_XOR, RCB4_COMM_NOT, RCB4_COMM_SHIFT, RCB4_COMM_ADD, RCB4_COMM_SUB,
 * RCB4_COMM_MUL, RCB4_COMM_DIV and RCB4_COMM_MOD.
 * 
 * @param comm is the allocated command to set.
 * @return 0 on success.
 * @sa rcb4_command_create(), RCB4_COMM_AND, RCB4_COMM_OR, RCB4_COMM_XOR,
 * RCB4_COMM_NOT, RCB4_COMM_SHIFT, RCB4_COMM_ADD, RCB4_COMM_SUB, RCB4_COMM_MUL,
 * RCB4_COMM_DIV, RCB4_COMM_MOD,
 * rcb4_command_set_dst_ram(), rcb4_command_set_dst_ics(),
 * rcb4_command_set_dst_com(), rcb4_command_set_dst_rom().
 */
int rcb4_command_set_dst_do_not_save(rcb4_comm* comm); // Doesn't write the result in destination, but does update the cpu flags.


//------//
// Data //
//------//


/**
 * @brief Set the number of bit shifts to the left.
 * 
 * Sets the number of shifts to the left to be done to the data.
 * 
 * Only RCB4_COMM_SHIFT can be used.
 * 
 * @param comm is the allocated command to set.
 * @param shifts is the number of bits to shift. [1~127]
 * @return 0 on success.
 * @sa RCB4_COMM_SHIFT.
 */
int rcb4_command_set_shift_left(rcb4_comm* comm, uint8_t shifts); // Only for: RCB4_COMM_SHIFT (shifts = 1 ~ 127)

/**
 * @brief Set the number of bit shifts to the right.
 * 
 * Sets the number of shifts to the right to be done to the data.
 * 
 * Only RCB4_COMM_SHIFT can be used.
 * 
 * @param comm is the allocated command to set.
 * @param shifts is the number of bits to shift. [1~127]
 * @return 0 on success.
 * @sa RCB4_COMM_SHIFT.
 */
int rcb4_command_set_shift_right(rcb4_comm* comm, uint8_t shifts); // Only for: RCB4_COMM_SHIFT (shifts = 1 ~ 127)

/**
 * @brief Set the size of the data to be manipulated.
 * 
 * Sets the number of bytes of the data to be used in the command.
 * For example, for a mov command it indicates the number of bytes to be copied.
 * 
 * All except RCB4_COMM_SINGLE, RCB4_COMM_CONST, RCB4_COMM_SERIES,
 * RCB4_COMM_SPEED can be used, but the only ones that are useful are
 * RCB4_COMM_NOT and RCB4_COMM_SHIFT because the size is not set with the
 * rcb4_command_set_src_*() functions.
 * 
 * @param comm is the allocated command to set.
 * @param size is the number of bytes to affect.
 * @return 0 on success.
 * @sa RCB4_COMM_NOT, RCB4_COMM_SHIFT.
 */
int rcb4_command_set_data_size(rcb4_comm* comm, uint8_t size); // All except: RCB4_COMM_SINGLE, RCB4_COMM_CONST, RCB4_COMM_SERIES, RCB4_COMM_SPEED

/**
 * @brief Set the ICS ID to be used.
 * 
 * Sets the ICS ID for the command RCB4_COMM_ICS.
 * 
 * @param comm is the allocated command to set.
 * @param ics is the ICS ID.
 * @return 0 on success.
 * @sa RCB4_COMM_ICS.
 */
int rcb4_command_set_ics(rcb4_comm* comm, uint8_t ics); // Only for: RCB4_COMM_ICS

/**
 * @brief Set the speed of all servos.
 * 
 * Sets the speed that all servos will use in RCB4_COMM_CONST. Note that this
 * is the only speed that will be used in RCB4_COMM_CONST as the individual
 * speeds are ignored in rcb4_command_set_servo().
 * 
 * @param comm is the allocated command to set.
 * @param speed is the speed to use, from 1 to 255 being 1 the slowest.
 * @return 0 on success.
 * @sa RCB4_COMM_CONST.
 */
int rcb4_command_set_speed(rcb4_comm* comm, uint8_t speed); // Only for: RCB4_COMM_CONST

/**
 * @brief Set the speed and position of a servo.
 * 
 * Sets the desired speed and position of a servo. Note that the speed is
 * ignored for RCB4_COMM_CONST.
 * 
 * For RCB4_COMM_SINGLE, multiple calls to this function will overwrite the data
 * to send.
 * 
 * In RCB4_COMM_CONST and RCB4_COMM_SERIES you can call multiple times this
 * function to configure various servos in the same command. If you call it more
 * than once for the same servo, it will be overwritten.
 * 
 * @param comm is the allocated command to set.
 * @param ics is the ID of the servo to control. From 1 to 36, but the KHR-3
 * has only servos 1 to 22 physically available.
 * @param speed is the speed to use, from 1 to 255 being 1 the slowest.
 * @param position is the position you want the servo to move. Note that the
 * robot will interpolate between positions by itself. The robot accepts from
 * 0 to 0xFFFF, but the servos can only move in a narrower range. Check
 * khr3_servo.ods (created by Kazuki Terabe) for more information.
 * @return 0 on success.
 * @sa RCB4_COMM_SINGLE, RCB4_COMM_CONST and RCB4_COMM_SERIES.
 */
int rcb4_command_set_servo(rcb4_comm* comm, uint8_t ics, uint8_t speed, uint16_t position); // Speed ignored in RCB4_COMM_CONST

/**
 * @brief Set the stretch of all servos.
 * 
 * @warning Not implemented. Do not use.
 * 
 * @param comm is the allocated command to set.
 * @param stretch the strech to apply to the servos.
 * @return 0 on success.
 * @sa RCB4_COMM_SPEED.
 */
int rcb4_command_set_stretch(rcb4_comm* comm, uint8_t stretch); // Not implemented yet


// Conditions for rcb4_jmp() and rcb4_call()
#define RCB4_CONDITION_ALWAYS 0 //!< Always executes the jump or call.
#define RCB4_CONDITION_C_SET ((1 << 3) | (1 << 1)) //!< Carry flag must be set to 1. @sa rcb4_jmp(), rcb4_call()
#define RCB4_CONDITION_C_CLR ((1 << 3) | (0 << 1)) //!< Carry flag must be set to 0. @sa rcb4_jmp(), rcb4_call()
#define RCB4_CONDITION_C_IGN ((0 << 3) | (0 << 1)) //!< Ignore carry flag (default). @sa rcb4_jmp(), rcb4_call()
#define RCB4_CONDITION_Z_SET ((1 << 2) | (1 << 0)) //!< Zero flag must be set to 1. @sa rcb4_jmp(), rcb4_call()
#define RCB4_CONDITION_Z_CLR ((1 << 2) | (0 << 0)) //!< Zero flag must be set to 0. @sa rcb4_jmp(), rcb4_call()
#define RCB4_CONDITION_Z_IGN ((0 << 2) | (0 << 1)) //!< Ignore zero flag (default). @sa rcb4_jmp(), rcb4_call()

// Send

/**
 * @brief Sends the command to the robot.
 * 
 * This function sends the command to the robot via the supplied connection. It
 * can optionally copy the data received to the buffer provided in the argument
 * reply.
 * 
 * Reply should be already allocated and the size of the expected data. For
 * example, if you do a MOV to copy 2 bytes from RAM to COM, expect those 2
 * bytes in reply.
 * 
 * Example:
 * @code
 * // Initialize and configure everything
 * rcb4_connection* conn = rcb4_init("/dev/ttyUSB0"); // Connect to the robot
 * rcb4_comm* comm = rcb4_command_create(RCB4_COMM_MOV); // Copy command
 * rcb4_command_set_src_ram(comm, 0x0014, 2); // Take 2 bytes from 0x0014 (A/D #4)
 * rcb4_command_set_dst_com(comm); // To COM
 * 
 * // OPTION 1
 * uint8_t buffer[2]; // 2 bytes in a buffer
 * rcb4_send_command(conn, comm, buffer); // Send the command and receive the reply (returned value = 2)
 * 
 * // OPTION 2
 * uint16_t word; // 2 bytes in a single variable
 * rcb4_send_command(conn, comm, &word); // Send the command and receive the reply (returned value = 2)
 * 
 * // Deinitialize
 * rcb4_command_delete(comm); // Delete the command
 * rcb4_deinit(conn); // Disconnect from the robot
 * @endcode
 * 
 * If you don't expect any reply from the robot or you don't care about the
 * reply, set reply to NULL.
 * 
 * @param conn is the allocated connection to the robot.
 * @param comm is the allocated and configured command to send.
 * @param reply is an optional pointer to a buffer where the reply from the
 * robot can be written. Can be NULL if you don't care about the reply. Only the
 * data will be copied to this buffer, not the real reply (ie. the size,
 * command, ACK and checksum bytes are not copied, only the real data).
 * @return >= 0 on success.
 * @return < 0 on error.
 * @return 0 if there was no data in the reply but the message was correct (ACK received).
 * @return > 0 bytes of the reply (only the received data, does not include the
 * protocol (size, command, ACK, checksum...).
 */
int rcb4_send_command(rcb4_connection* conn, const rcb4_comm* comm, uint8_t* reply);

/**
 * @brief Jump to an address in ROM.
 * 
 * A JMP instruction will be executed in the robot.
 * By default the jump will be unconditional but you can pass the following
 * flags if you want to make a conditional jump:
 * RCB4_CONDITION_C_SET, RCB4_CONDITION_C_CLR, RCB4_CONDITION_Z_SET and
 * RCB4_CONDITION_Z_CLR.
 * 
 * For example, if you want to jump ONLY when the zero flag is set to 1 but you
 * don't care about the carry flag:
 * @code
 * rcb4_jmp(conn, addr, RCB4_CONDITION_Z_SET);
 * @endcode
 * 
 * If you want to jump ONLY when the zero flag is set to 0 and the carry flag is
 * set to 1.
 * @code
 * rcb4_jmp(conn, addr, RCB4_CONDITION_Z_CLR | RCB4_CONDITION_C_SET);
 * @endcode
 * 
 * If you don't care about any flag, just pass a 0 to conditions.
 * 
 * @param conn is the connection to the robot.
 * @param addr the ROM address to go to.
 * @param conditions are optional conditions to make a conditional jump.
 * @return 0 on success.
 * @sa RCB4_CONDITION_C_SET, RCB4_CONDITION_C_CLR, RCB4_CONDITION_Z_SET and
 * RCB4_CONDITION_Z_CLR.
 */
int rcb4_jmp(rcb4_connection* conn, uint32_t addr, uint8_t conditions); // Execute a JMP instruction to the ROM address (goto address)

/**
 * @brief Call to an address in ROM.
 * 
 * A CALL instruction will be executed in the robot.
 * By default the jump will be unconditional but you can pass the following
 * flags if you want to make a conditional jump:
 * RCB4_CONDITION_C_SET, RCB4_CONDITION_C_CLR, RCB4_CONDITION_Z_SET and
 * RCB4_CONDITION_Z_CLR.
 * 
 * For example, if you want to jump ONLY when the zero flag is set to 1 but you
 * don't care about the carry flag:
 * @code
 * rcb4_jmp(conn, addr, RCB4_CONDITION_Z_SET);
 * @endcode
 * 
 * If you want to jump ONLY when the zero flag is set to 0 and the carry flag is
 * set to 1.
 * @code
 * rcb4_jmp(conn, addr, RCB4_CONDITION_Z_CLR | RCB4_CONDITION_C_SET);
 * @endcode
 * 
 * If you don't care about any flag, just pass a 0 to conditions.
 * 
 * @param conn is the connection to the robot.
 * @param addr the ROM address to go to.
 * @param conditions are optional conditions to make a conditional jump.
 * @return 0 on success.
 * @sa RCB4_CONDITION_C_SET, RCB4_CONDITION_C_CLR, RCB4_CONDITION_Z_SET and
 * RCB4_CONDITION_Z_CLR.
 */
int rcb4_call(rcb4_connection* conn, uint32_t addr, uint8_t conditions); // Execute a CALL instruction to the ROM address (execute function)

/**
 * @brief Return from current function.
 * 
 * A RET (return) instruction will be executed in the robot returning from the
 * current function.
 * 
 * @param conn is the connection to the robot.
 * @return 0 on success.
 */
int rcb4_ret(rcb4_connection* conn); // Execute a RET instruction (return from function)

// Utilities

/**
 * @brief Print the hexadecimal dump of the command.
 * 
 * Prints to stdout the command in hexadecimal followed by the checksum.
 * 
 * @param comm is the allocated command to print.
 */
void rcb4_command_debug_print(const rcb4_comm* comm);

/**
 * @brief Sleeps in microseconds.
 * 
 * Sleeps the execution of the program (NOT the robot). Uses the unix standard
 * nanosleep call (in a loop to ignore premature exits due to alarms).
 * 
 * @param usec is the number of microseconds to sleep.
 */
void rcb4_util_usleep(uint32_t usec); // Sleep for usec microseconds

/**
 * @brief Get the reading from the analog-digital conversor of the robot.
 * 
 * Automagically asks the robot the readings of the sensor values.
 * 
 * @param conn is the connection to the robot.
 * @param ad_id is the ID of the sensor. From 0 to 10.
 * @param value specifies where to save the result (pointer to a 2byte variable).
 * @return 0 if OK.
 */
int rcb4_ad_read(rcb4_connection* conn, uint8_t ad_id, uint16_t* value); // ID from 0 to 10. Returns 0 if ok, AD value in "value"

#ifdef __cplusplus
}
#endif


#endif // RCB4_H

