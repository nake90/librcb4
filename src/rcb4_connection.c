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
 * @file rcb4_connection.c
 * @brief Functions to create, use and destroy a serial connection to RCB4.
 * 
 * @details These functions allow the creation and use of a connection to the
 * RCB4 processor.
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
#include "rcb4_connection.h"
#include "rcb4_command.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <linux/serial.h>
#include <fcntl.h>
#include <termio.h>



/* From http://cc.byexamples.com/2007/05/25/nanosleep-is-better-than-sleep-and-usleep/ */
static
int __nsleep(const struct timespec *req, struct timespec *rem)
{
	struct timespec temp_rem;
	if(nanosleep(req, rem) == -1)
		return __nsleep(rem, &temp_rem);
	else
		return 0;
}

// Sleeps for usec microseconds
void rcb4_util_usleep(uint32_t usec)
{
	struct timespec req, rem;
	
	if(usec <= 0)return;
	
	req.tv_sec = usec / 1000000;
	usec = usec - (req.tv_sec * 1000000);
	req.tv_nsec = usec * 1000;
	
	__nsleep(&req, &rem);
}



rcb4_connection* rcb4_init(const char* tty)
{
	struct termios cfg;
	
	if(!tty)return NULL;
	
	// Allocate the variable
	rcb4_connection* conn = (rcb4_connection*)malloc(sizeof(rcb4_connection));
	if(!conn)
	{
		fprintf(stderr, "Error opening %s for read/write.\nMemory error.\n", tty);
		return NULL;
	}
	
	// Check serial access
	if((conn->fd = open(tty, O_RDWR | O_NOCTTY | O_SYNC)) < 0)
	{
		fprintf(stderr, "Error opening %s for read/write.\nCheck that you have permission to access the device and that it is plugged correctly.\n", tty);
		free(conn);
		return NULL;
	}
	
	// Save old configuration
	tcgetattr(conn->fd, &conn->old_cfg);
	
	// Speed hack (https://stackoverflow.com/questions/4968529/how-to-set-baud-rate-to-307200-on-linux)
	const int speed = 1250000; // Fastest speed that the robot can go
	struct serial_struct ss;
	ss.reserved_char[0] = 0;
	if(ioctl(conn->fd, TIOCGSERIAL, &ss) < 0)
	{
		fprintf(stderr, "Cannot set serial port speed. ioctl failed.\n");
		close(conn->fd);
		free(conn);
		return NULL;
	}
	
	ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
	ss.custom_divisor = (ss.baud_base + (speed / 2)) / speed;
	
	if(ss.custom_divisor < 1) 
		ss.custom_divisor = 1;
	if(ioctl(conn->fd, TIOCSSERIAL, &ss) < 0 || ioctl(conn->fd, TIOCGSERIAL, &ss) < 0)
	{
		fprintf(stderr, "Cannot set serial port speed. ioctl failed.\n");
		close(conn->fd);
		free(conn);
		return NULL;
	}
	
	// Check that the speed is not too far away from what we want
	int closestSpeed = ss.baud_base / ss.custom_divisor;
	
	if(closestSpeed < speed * 98 / 100 || closestSpeed > speed * 102 / 100)
	{
	    fprintf(stderr, "Cannot set serial port speed to %d. Closest possible is %d\n", speed, closestSpeed);
	}
	
	// End of speed hack
	
	// Configure the terminal
	bzero(&cfg, sizeof(cfg));
	cfg.c_cflag = PARENB | CS8 | CLOCAL | CREAD; // Control flags: Parity (even), 8bits, ignore control lines, enable read
	cfg.c_iflag = 0; // Input flags
	cfg.c_oflag = 0; // Output flags
	cfg.c_lflag = 0; // Local flags
	cfg.c_cc[VTIME] = 0; // Inter-character timer off
	cfg.c_cc[VMIN] = 1; // Minimum 1 byte to return from read()

	//cfsetispeed(&cfg, B38400); // Custom speed
	//cfsetospeed(&cfg, B38400); // Custom speed
	cfsetispeed(&cfg, RCB4_BAUD_RATE); // Input speed
	cfsetospeed(&cfg, RCB4_BAUD_RATE); // Output speed
	
	tcflush(conn->fd, TCIFLUSH); // Flush old messages
	
	if(tcsetattr(conn->fd, TCSANOW, &cfg) != 0) // Apply the configuration (slow speed)
	{
		fprintf(stderr, "Error configuring the terminal.\n");
		close(conn->fd);
		free(conn);
		return NULL;
	}
	
	//fcntl(conn->fd, F_SETFL, FNDELAY); // Non-blocking
	fcntl(conn->fd, F_SETFL, 0); // Blocking mode
	
	rcb4_util_usleep(COMM_DELAY_USECS); // Wait a bit
	
	// Try pinging to see if the speed is correct
	if(rcb4_command_ping(conn) == 0)
	{
		printf("Baudrate set to %d [Error = %.2f%%].\n", 115200, 0.0f);
		return conn;
	}
	rcb4_util_usleep(2*COMM_DELAY_USECS);
	if(rcb4_command_ping(conn) == 0)
	{
		printf("Baudrate set to %d [Error = %.2f%%].\n", 115200, 0.0f);
		return conn;
	}
	
	// ping failed, try changing the device's speed
	
	cfsetispeed(&cfg, B38400); // Custom speed
	cfsetospeed(&cfg, B38400); // Custom speed
	
	tcflush(conn->fd, TCIFLUSH); // Flush old messages
	
	if(tcsetattr(conn->fd, TCSANOW, &cfg) != 0) // Apply the configuration (fast speed)
	{
		fprintf(stderr, "Error configuring the terminal.\n");
		close(conn->fd);
		free(conn);
		return NULL;
	}
	
	// Try pinging to see if the speed is correct
	if(rcb4_command_ping(conn) == 0)
	{
		printf("Baudrate set to %d [Error = %.2f%%].\n", closestSpeed, 100.0*abs(closestSpeed - speed)/speed);
		return conn;
	}
	rcb4_util_usleep(2*COMM_DELAY_USECS);
	if(rcb4_command_ping(conn) == 0)
	{
		printf("Baudrate set to %d [Error = %.2f%%].\n", closestSpeed, 100.0*abs(closestSpeed - speed)/speed);
		return conn;
	}
	
	// None of the speeds allowed us to ping. Maybe the robot is using another speed or there is a problem with the connection?
	
	fprintf(stderr, "Connection failed.\n");
	close(conn->fd);
	free(conn);
	return NULL;
}

void rcb4_deinit(rcb4_connection* conn)
{
	if(!conn)return;
	
	// Disable speed hack
	struct serial_struct ss;
	ioctl(conn->fd, TIOCGSERIAL, &ss);
	ss.flags &= ~ASYNC_SPD_MASK;
	ioctl(conn->fd, TIOCSSERIAL, &ss);
	
	tcsetattr(conn->fd, TCSANOW, &conn->old_cfg); // Pop back the original configuration
	close(conn->fd);
	free(conn);
}

// Sends the message via serial, returns size of the reply if all ok, < 0 if something went wrong
int rcb4_send_command(rcb4_connection* conn, const rcb4_comm* comm, uint8_t* reply)
{
	int err;
	uint8_t check;
	uint8_t lbuf[256];
	//uint8_t checksum;
	uint8_t command[128];
	uint8_t ret_size;
	struct timeval timeout; // Timeout
	int rv;
	
	assert(conn);
	assert(comm);
	
	// Copy the command to a buffer and append the checksum
	memcpy(command, (const uint8_t*)comm, comm->size - 1);
	command[comm->size-1] =  rcb4_command_calculate_checksum(comm);
	
#ifdef DEBUG_COMMANDS
	printf("COMMAND TO SEND:\n");
	int i;
	for(i=0; i<comm->size; i++)
		printf("0x%02X ", command[i]);
	printf("\nEND OF COMMAND\n");
#endif // DEBUG_COMMANDS
	
	// Send the message
	err = write(conn->fd, command, comm->size);
	if(err != comm->size)
	{
		fprintf(stderr, "Error sending the command. Write error.\n");
		return -1;
	}
	
	// Wait a bit
	rcb4_util_usleep(COMM_DELAY_USECS);
	
	
	timeout.tv_sec = 0;
	timeout.tv_usec = COMM_TIMEOUT_USECS;
	
	FD_ZERO(&conn->fdset);
	FD_SET(conn->fd, &conn->fdset);
	rv = select(conn->fd + 1, &conn->fdset, NULL, NULL, &timeout); // Receive the message, or timeout
	if(rv == -1)
	{
		fprintf(stderr, "Error sending the command. Select failed.\n");
		return -1;
	}
	else if(rv == 0)
	{
		fprintf(stderr, "Error sending the command. Timed out.\n");
		return -1;
	}
	
	
	ret_size = rcb4_command_get_response_size(comm); // Get how long the response should be based on the command we sent
	if(ret_size == 0) // Does not expect a reply. Only the default ACK/NACK message
	{
		err = read(conn->fd, lbuf, 4); // 0x04, CMD, ACK|NAK, SUM
		if(err != 4 || lbuf[0] != 0x04 || lbuf[1] != comm->type || lbuf[2] != RCB4_ACK || lbuf[3] != (check = (uint8_t)(0x04 + comm->type + RCB4_ACK)))
		{
			fprintf(stderr, "Error sending the command. Read error.\nReceived %d bytes, msg = 0x%02X, 0x%02X, 0x%02X, 0x%02X\n",
			        err, (err > 0) ? lbuf[0] : 0x00, (err > 1) ? lbuf[1] : 0x00, (err > 2) ? lbuf[2] : 0x00, (err > 3) ? lbuf[3] : 0x00);
			fprintf(stderr, "               Expected 0x04, 0x%02X, 0x%02X, 0x%02X\n", comm->type, RCB4_ACK, check);
			return -1;
		}
	}
	else // Does expect a reply
	{
		//TODO: Checksum check?
		err = read(conn->fd, lbuf, ret_size + 3); // 0x04, CMD, RET, SUM
		if(err != ret_size + 3 || lbuf[0] != ret_size + 3 || lbuf[1] != comm->type)
		{
			fprintf(stderr, "Error sending the command. Read error.\nReceived %d bytes, msg = 0x%02X, 0x%02X, 0x%02X, ...\n",
			        err, (err > 0) ? lbuf[0] : 0x00, (err > 1) ? lbuf[1] : 0x00, (err > 2) ? lbuf[2] : 0x00);
			fprintf(stderr, "Expected %d bytes , msg = 0x%02X, 0x%02X, ...\n", ret_size + 3, ret_size + 3, comm->type);
			return -2;
		}
		
		if(reply != NULL) // Maybe the user doesn't want the reply
			memcpy(reply, lbuf + 2, ret_size); // Does not include the checksum nor the headers. Only the data.
	}
	
	rcb4_util_usleep(COMM_DELAY_USECS); // Wait a bit
	return ret_size; // Return the size of the reply
}


int rcb4_command_ping(rcb4_connection* conn)
{
	int err;
	uint8_t lbuf[4];
	uint8_t command[] = {0x03, 0xFE, 0x01}; // New ping, old one is 0x04, 0xFE, 0x06, 0x08
	//uint8_t command[] = {0x04, 0xFE, 0x06, 0x08}; // Old ping, new one is 0x03, 0xFE, 0x01
	struct timeval timeout; // Timeout
	int rv;
	
	// Send the message
	err = write(conn->fd, command, sizeof(command));
	if(err != sizeof(command))
	{
		fprintf(stderr, "Error sending the command. Write error.\n");
		return -1;
	}
	
	timeout.tv_sec = 0;
	timeout.tv_usec = COMM_TIMEOUT_USECS;
	
	rcb4_util_usleep(COMM_DELAY_USECS); // Wait a bit
	
	FD_ZERO(&conn->fdset);
	FD_SET(conn->fd, &conn->fdset);
	rv = select(conn->fd + 1, &conn->fdset, NULL, NULL, &timeout); // Receive the message or die waiting, like when you invite out a japanese girl and she never shows up
	if(rv == -1)
	{
		fprintf(stderr, "Error sending the command. Select failed.\n");
		return -1;
	}
	else if(rv == 0)
	{
		// Timeout (silent)
		// I'm starting to think that even the compiler ignores my comments...
		return -10;
	}
	
	err = read(conn->fd, lbuf, 4); // 0x04, CMD, ACK|NAK, SUM
	if(err != 4 || lbuf[0] != 0x04 || lbuf[1] != command[1])
	{
		fprintf(stderr, "Error sending the command. Read error.\nReceived %d bytes, msg = 0x%02X, 0x%02X, 0x%02X, 0x%02X\n",
		        err, (err > 0) ? lbuf[0] : 0x00, (err > 1) ? lbuf[1] : 0x00, (err > 2) ? lbuf[2] : 0x00, (err > 3) ? lbuf[3] : 0x00);
		fprintf(stderr, "               Expected 0x04, 0x%02X, 0x%02X, ...\n", command[1], RCB4_ACK);
		return -1;
	}
	
	
	rcb4_util_usleep(COMM_DELAY_USECS);
	
	// Did we receive an ACK?
	if(lbuf[2] == RCB4_ACK && lbuf[3] == (uint8_t)(0x04 + command[1] + RCB4_ACK))
		return 0;
	
	// Or was it a NACK?
	if(lbuf[2] == RCB4_NCK && lbuf[3] == (uint8_t)(0x04 + command[1] + RCB4_NCK))
		return 1;
	
	// Or just random data?
	fprintf(stderr, "Error sending the command. Invalid checksum.\n");
	return -1;
}

