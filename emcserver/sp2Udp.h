/**
 * @file	sp2Udp.h
 * @brief	Class for sending UDP packets to Spider 2.0 on Windows
 * @author	Trey Arthur
 * @version 2.1
 * @date	3/22/2017
 *
 */

#ifndef _SP2_UDP_
#define _SP2_UDP_

#include "spPacketStruct.h"
//
#include <iostream>
#include <sstream>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Winsock2.h> // before Windows.h, else Winsock 1 conflict
#include <Ws2tcpip.h> // needed for ip_mreq definition for multicast
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for sleep()
#include <netdb.h>
#endif

class spException : public std::exception
{
public:
	spException(std::string message, const int code)
	{
		m_Message = message;
		m_Code    = code;
	}

	virtual const char* what() const throw()
	{
		std::string outStr;
		outStr = "!! sp2Udp::" + m_Message;
		outStr += ": ";
		outStr += std::to_string(m_Code);
		return outStr.c_str();
	}

	virtual ~spException() throw() {};

private:
	std::string m_Message;
	int m_Code;
};

/**
  * @class sp2Udp
  * @brief class to send data to Spider 2.0
  *
  * This class sends data packets to Spider2.0.  Spider 2.0 is a data server that accepts data via UDP packets.
  * The UDP structure Spider expects is in the spPacketStruct.h.
  * 
  */
class sp2Udp
{
public:
	//struct spException : public std::exception {
	//	const char * what() const throw () {
	//		return "C++ Exception";
	//	}
	//};

	/**
    * Constructor
    */
	sp2Udp();

	/**
    * Destructor
    */
	~sp2Udp();

	/**
	* Initialize
	*/
	bool Init();

	/**
	* Set multicast mode
	*/
	void SetMulticastMode(const bool mm) { m_MulticastMode = mm; }

	/**
	* Set timeout values for non blocking read
	*/
	void SetTimeout(const long secs, const long microSecs) { m_TimeoutSecs = secs; m_TimeoutMicroSecs = microSecs; }

	/**
	* Add data to packet
	*  @param[in]	name	string name of the data to send
	*  @param[in]	value	value of the data
	*/
	void Data(std::string name, double value);

	/**
	* send data down UDP port
	*/
	void Send();

	/**
	* read data from spider
	*/
	void Read();

	/**
	* read data from spider and block
	*/
	void BlockRead();

	/**
	* return data packet received from spider
	*/
	spPacketStruct Packet() { return m_RecvPacket; }

private:
	void InitSend();
	void InitRead();

	int  m_SendSocket;		///< Socket to send data
	int  m_RecvSocket;		///< Socket to receive data
	spPacketStruct m_Packet;	///< Packet of data to send to Spider
	spPacketStruct m_RecvPacket;///< Packet of data to received from Spider
	int m_DataCount;			///< Running count of data items

	sockaddr_in m_AddrSrv;
	int m_AddrSize;
	int m_PacketSize;

	long m_TimeoutSecs;			///< read time out in seconds
	long m_TimeoutMicroSecs;	///< read time out in microseconds

	bool m_MulticastMode;		///< use multicast mode with spider rather than local (default)
};

#endif 
