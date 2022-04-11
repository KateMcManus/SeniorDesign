//#include "stdafx.h"
#include "sp2Udp.h"

sp2Udp::sp2Udp()
{
	m_DataCount = 0;
	m_MulticastMode = false;	// default to localhost
	m_RecvPacket.Size = 0;		// set packet size to 0 initialize

	m_TimeoutSecs = 1;
	m_TimeoutMicroSecs = 0;
}

sp2Udp::~sp2Udp()
{
#ifdef _WIN32
	closesocket(m_SendSocket);
	closesocket(m_RecvSocket);
	WSACleanup();
#else
	close(m_SendSocket);
	close(m_RecvSocket);
#endif
}

bool sp2Udp::Init()
{
#ifdef _WIN32
	WSADATA wsaData;
	int retval;

	if ((retval = WSAStartup(0x202, &wsaData)) != 0)
	{
		throw spException("Init() - WSAStartup() failed with error", retval);
		return false;
	}
#endif

	InitSend();
	//InitRead();

	return true;
}

void sp2Udp::InitRead()
{
	//---------------------------------------------
	// Create a socket for receiving data
	m_RecvSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
	if (m_RecvSocket == INVALID_SOCKET) {
		throw spException("InitRead() - recv socket() failed" , WSAGetLastError() );
		return;
	}
#else
if (m_RecvSocket < 0) {
	throw spException("InitRead() - recv socket() failed", m_RecvSocket);
	return;
}
#endif
	bool bOptVal = true;
	int bOptLen = sizeof (bool);
	
	int iResultopt = setsockopt(m_RecvSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal, bOptLen);
#ifdef _WIN32
	if ( iResultopt == SOCKET_ERROR )
	{
		throw spException("InitRead() - setsockopt() error" , WSAGetLastError() );
		return;
	}
#else
	if (iResultopt <0)
	{
		throw spException("InitRead() - setsockopt() error", iResultopt);
		return;
	}
#endif
		
	bool multicast = m_MulticastMode;
	
	if ( multicast )
	{
		m_AddrSrv.sin_addr.s_addr = htonl(INADDR_ANY);
		m_AddrSrv.sin_port = htons (SPIDER_MULTICAST_PORT);
	}
	else
	{
		inet_pton(AF_INET, "127.0.0.1", &(m_AddrSrv.sin_addr));
		m_AddrSrv.sin_port = htons (SPIDER_LOCAL_PORT);
	}
	
	m_AddrSrv.sin_family = AF_INET;
		
	m_AddrSize = sizeof( m_AddrSrv );
	
	m_PacketSize = sizeof(spPacketStruct);
	
	int iResult = bind(m_RecvSocket, (sockaddr *) & m_AddrSrv, sizeof (m_AddrSrv));
#ifdef _WIN32
	if (iResult != 0) {
		throw spException("InitRead() -  recv bind() failed" , WSAGetLastError() );
		return;
	}
#else
	if (iResult != 0) {
		throw spException("InitRead() -  recv bind() failed", iResult);
		return;
	}
#endif
	
	// join a multicast group
	if ( multicast ) 
	{
		struct ip_mreq mreq;
		//mreq.imr_multiaddr.s_addr=inet_addr(SPIDER_MULTICAST_ADDRESS);
		inet_pton(AF_INET, SPIDER_MULTICAST_ADDRESS, &(mreq.imr_multiaddr.s_addr));
		mreq.imr_interface.s_addr=htonl(INADDR_ANY);
		int iResult = setsockopt(m_RecvSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
		if ( iResult < 0) {
			throw spException("InitRead() - setsockopt join multicast failed" , iResult );
			return;
		}
	}
}

void sp2Udp::Read()
{
	fd_set readFDS;
	struct timeval selectTimeOut;
	selectTimeOut.tv_sec  = m_TimeoutSecs;		// seconds
	selectTimeOut.tv_usec = m_TimeoutMicroSecs;	// microseconds
	FD_ZERO(&readFDS);
	FD_SET(m_RecvSocket,&readFDS);
	int t = select(-1, &readFDS, NULL, NULL, &selectTimeOut);
#ifdef _WIN32
	if (t == SOCKET_ERROR) {
		throw spException("Read() - select failed", t);
		return;
    }
	else if (t != 0) {
        if (FD_ISSET(m_RecvSocket, &readFDS)) {
			BlockRead();
        }
    }
#else
	if (t < 0) {
		throw spException("Read() - select failed", t);
		return;
}
	else if (t != 0) {
		if (FD_ISSET(m_RecvSocket, &readFDS)) {
			BlockRead();
		}
	}
#endif
}

void sp2Udp::BlockRead()
{
	int iResult = recv(m_RecvSocket,(char *)&m_RecvPacket,sizeof(spPacketStruct),0 );
#ifdef _WIN32
	if (iResult == SOCKET_ERROR) {
		throw spException("BlockRead() - recv failed", WSAGetLastError());
		return;
    }
#else
	if (iResult < 0) {
		throw spException("BlockRead() - recv failed", iResult);
		return;
	}
#endif
}

void sp2Udp::InitSend()
{
	bool multicast = m_MulticastMode;
	std::string ipAddress;
	std::string port;

	if (multicast)
	{
		ipAddress = SPIDER_MULTICAST_ADDRESS;
		port = std::to_string(SPIDER_MULTICAST_PORT + 1);
	}
	else
	{
		ipAddress = "127.0.0.1";
		port = std::to_string(SPIDER_LOCAL_PORT + 1);
	}

	struct addrinfo hints;
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;

	memset(&hints, 0, sizeof(hints));
	//ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	// Resolve the server address and port
	int iResult = getaddrinfo(ipAddress.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		throw spException("InitSend() - getaddrinfo failed with error" , iResult);
#ifdef _WIN32
		WSACleanup();
#endif
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		m_SendSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
#ifdef _WIN32
		if (m_SendSocket == INVALID_SOCKET) {
			throw spException("InitSend() - socket failed with error" , WSAGetLastError() );
			WSACleanup();
			return;
		}
#else
		if (m_SendSocket < 0) {
			throw spException("InitSend() - socket failed with error", m_SendSocket);
			return;
		}
#endif

		// Connect to server.
		iResult = connect(m_SendSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
#ifdef _WIN32
		if (iResult == SOCKET_ERROR) {
			closesocket(m_SendSocket);
			m_SendSocket = INVALID_SOCKET;
			continue;
		}
#else
		if (iResult < 0) {
			continue;
		}
#endif

		break;
	}

	freeaddrinfo(result);

#ifdef _WIN32
	if (m_SendSocket == INVALID_SOCKET) {
		throw spException("InitSend() - unable to connect" , 0 );
		WSACleanup();
		return;
	}
#else
	if (m_SendSocket < 0) {
		throw spException("InitSend() - unable to connect", m_SendSocket);
		return;
	}
#endif
}

void sp2Udp::Data(std::string name , double value)
{
	int i = m_DataCount;

	strncpy(m_Packet.Name[i], name.c_str(), name.length());
	m_Packet.Name[i][name.length()] = '\0';
	m_Packet.Value[i] = value;

	m_DataCount++;
}

void sp2Udp::Send()
{
	m_Packet.Size = m_DataCount;
	m_DataCount = 0;

	send(m_SendSocket, (char *)&m_Packet, sizeof(spPacketStruct), 0);
}


