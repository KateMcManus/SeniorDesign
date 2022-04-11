/*!
* @file       spPacketStruct.h
* @brief      Structure UDP data tranfers
* @author     Trey Arthur
* @version    2.0
* @date       7/31/2015
*
* This structure defines UDP data packets
*
*/
#ifndef SPPACKETSTRUCT_H
#define SPPACKETSTRUCT_H

#define SPIDER_MULTICAST_ADDRESS "239.255.3.14"
#define SPIDER_MULTICAST_PORT 48628

#define SPIDER_LOCAL_PORT 46455
#define SPIDER_USER_PORT  47455

/**
* @def SP_PACKET_SIZE
* @brief defines the size of a data packet
*/
#define SP_PACKET_SIZE 200

/**
* @def NAME_SIZE
* @brief defines the maximum number of characters in an packet data label
*/
#define NAME_SIZE 64

/**
* @struct spPacketStruct
* @brief Data stream structure format
*/
struct spPacketStruct
{
	int Size;                               ///< Number of items in the data packet
	char Name[SP_PACKET_SIZE][NAME_SIZE];   ///< Data labels
	double Value[SP_PACKET_SIZE];           ///< Data packet values
};

// DO NOT USE THE PORT NUMBERS BELOW
//   These port numbers are the originial ports for Spider 1.0
//   They are here for legacy compatibility for LC plugins
#define SPIDER_SEND_PORT 45454
#define SPIDER_RECV_PORT 38692

#endif // SPPACKETSTRUCT_H

