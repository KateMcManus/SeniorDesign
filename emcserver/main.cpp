#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define LIN 1

#include <iostream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <ctime>
#include <ncurses.h>

#include "EMCMessageDefinitions.hpp"
#include "PCSBSocket.hpp"
#include "PCSBSocketUDP.hpp"
#include "sp2Udp.h"
#include "spPacketStruct.h"

#if IBM
#include <windows.h>
#elif LIN || APL
#include <termios.h>
#include <unistd.h>
#else
#error YOU MUST DEFINE A PLATFORM!
#endif

#include "main.hpp"

using namespace std;

PCSBSocket * mSocket; //SocketTCPIP
PCSBSocketUDP * mSocketUDP = NULL; // SocketUDP
PCSBSocketUDP* clientSocketUDP = NULL;
sp2Udp *spider = new sp2Udp();
const char * inServerIP =  "127.0.0.1";
unsigned short inPort = 0;
//PUT BACK
//unsigned int flightModelIP;
unsigned int flightModelIP = ntohl(inet_addr("192.168.200.157"));
//unsigned int flightModelIP = ntohl(inet_addr("172.30.227.131"));
unsigned short flightModelPort = 8887;

UINT32 outgoingCount = 1;
bool quitPressed = false;

extern atomic<bool> stop_calls;
atomic<bool> stop_calls { false };
//thread call_back_thread;

//TEST CLIENT APP
UINT32 outgoingCount_client = 1;
bool testClient = 1;
unsigned int clientIP = ntohl(inet_addr("192.168.200.157")); //change later, this is my IP
//unsigned int clientIP = ntohl(inet_addr("172.30.227.131")); //UC
unsigned short clientPortSend = 49314;
unsigned short clientPortReceive = 49315;

bool initSocket()
{
	mSocket = NULL;
	mSocket->StartupNetworking(0);

	unsigned long	ip = PCSBSocket::LookupAddress(inServerIP);
	if (ip == 0)
		return 0;

	mSocket = new PCSBSocket(0, false);

	bool isConnected = false;

	mSocket->Connect(ip, inPort);
	while (!isConnected)
	{
		isConnected = mSocket->GetStatus() == PCSBSocket::status_Connected;
	}

	//mSocketUDP = NULL;
	mSocketUDP = new PCSBSocketUDP(8888);
	
	//Port to connect to client
	clientSocketUDP = new PCSBSocketUDP(49314);

	return 1;
}

// call the function periodically, once every 'period_in_milliseconds' milliseconds.
// repeat the call until 'stop_calls' is set to true
void periodic_call( int period_in_milliseconds, atomic<bool>& stop_calls, function< void() > function  )
{
	const auto period = chrono::milliseconds(period_in_milliseconds) ;

	while( !stop_calls )
	{
		auto start = chrono::steady_clock::now();
		function(); // assumes that the function won't take more than 'period_in_milliseconds' milliseconds
		auto end = chrono::steady_clock::now();

		// set delta to the time (in milliseconds) that was used to execute the function
		const auto delta = chrono::duration_cast<chrono::milliseconds>(end-start);

		// sleep till the wait time has elapsed
		this_thread::sleep_for( period - delta );
	}
}

void DrawDivider(int y)
{
	move(y,0);
	for (int i; i<80; i++)
	{
		addch(ACS_CKBOARD);
	}
}

void mvClearToEol(int y, int x)
{
	move(y,x);
	clrtoeol();
}

void ClearRunArea()
{
	mvClearToEol(2,0);
	mvClearToEol(3,0);
	mvClearToEol(4,0);
	mvClearToEol(5,0);
	mvClearToEol(6,0);
	mvClearToEol(7,0);
}

void ClearStateArea()
{
	mvClearToEol( 9,0);
	mvClearToEol(10,0);
	mvClearToEol(11,0);
	mvClearToEol(12,0);
	mvClearToEol(13,0);
	mvClearToEol(14,0);
	mvClearToEol(15,0);
	mvClearToEol(16,0);
}

void EMC_Run()
{	
	EMC_RUN_TIME_DATA_STRUCT emcRunTime;
	// Send Runtime EMC Data
/* 	emcRunTime.vdcSetPoint_0_30V = commandState.vdcVoltage;
	emcRunTime.vdsBlackoutStatus = commandState.vdsBlackout;
	emcRunTime.planetaryCommand = commandState.planetaryState;
	emcRunTime.horizontalCommand = commandState.horizontalState;
	emcRunTime.heaveCommand = commandState.heaveState;
	emcRunTime.rollCommand = commandState.rollState;
	emcRunTime.yawCommand = commandState.yawState;
	emcRunTime.pitchCommand = commandState.pitchState; */
	
	//ONLY FOR DEMONSTRATION
 	emcRunTime.vdcSetPoint_0_30V = rand()%(360);
	emcRunTime.vdsBlackoutStatus = rand()%(360);
	emcRunTime.planetaryCommand = rand()%(360);
	emcRunTime.horizontalCommand = rand()%(360);
	emcRunTime.heaveCommand = rand()%(360);
	emcRunTime.rollCommand = rand()%(360);
	emcRunTime.yawCommand = rand()%(360);
	emcRunTime.pitchCommand = rand()%(360);

	outgoingHeader.bodyLength = sizeof(emcRunTime);
	outgoingHeader.messageId = MC_EMC_MESSAGE_ID_ENUM::MT_EMC_MC_RUN_TIME;
	outgoingHeader.messageNumber = outgoingCount;
	//mSocket->WriteData(&outgoingHeader, sizeof(MC_EMC_MESSAGE_HEADER));
	//mSocket->WriteData(&emcRunTime, outgoingHeader.bodyLength);
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "%f,%i,%f,%f,%f,%f,%f,%f\n",
		emcRunTime.vdcSetPoint_0_30V,
		emcRunTime.vdsBlackoutStatus,
		emcRunTime.planetaryCommand,
		emcRunTime.horizontalCommand,
		emcRunTime.heaveCommand,
		emcRunTime.rollCommand,
		emcRunTime.yawCommand,
		emcRunTime.pitchCommand);
	clientSocketUDP->WriteData(buffer, sizeof(buffer), clientIP, clientPortSend);

	outgoingCount++;

}

void checkLimits()
{
	if (commandState.pitchState > 180) commandState.pitchState -=360;
	if (commandState.pitchState < -180) commandState.pitchState +=360;
	if (commandState.rollState > 180) commandState.rollState -=360;
	if (commandState.rollState < -180) commandState.rollState +=360;
	if (commandState.yawState > 180) commandState.yawState -=360;
	if (commandState.yawState < -180) commandState.yawState +=360;
	if (commandState.heaveState > 3) commandState.heaveState =3;
	if (commandState.heaveState < -3) commandState.heaveState =-3;
	if (commandState.horizontalState > 16.5) commandState.horizontalState =16.5;
	if (commandState.horizontalState < -16.5) commandState.horizontalState =-16.5;
	if (commandState.planetaryState > 180) commandState.planetaryState -=360;
	if (commandState.planetaryState < -180) commandState.planetaryState +=360;
}

void SendEMCRunSetup()
{
	bool configuringEMCRunSetup = true;

	while (configuringEMCRunSetup)
	{
		char ch = getch();
		if (ch=='1') {
			emcrunsetup.planetaryMotionType = ToggleMotionType(emcrunsetup.planetaryMotionType, true);
		}
		if (ch=='2') {
			emcrunsetup.horizontalMotionType = ToggleMotionType(emcrunsetup.horizontalMotionType, false);
		}
		if (ch=='3') {
			emcrunsetup.heaveMotionType = ToggleMotionType(emcrunsetup.heaveMotionType, false);
		}
		if (ch=='4') {
			emcrunsetup.rollMotionType = ToggleMotionType(emcrunsetup.rollMotionType, true);
		}
		if (ch=='5') {
			emcrunsetup.yawMotionType = ToggleMotionType(emcrunsetup.yawMotionType, true);
		}
		if (ch=='6') {
			emcrunsetup.pitchMotionType = ToggleMotionType(emcrunsetup.pitchMotionType, true);
		}
		if (ch=='7') {
			vdcVoltage += 1;
			if (vdcVoltage > 30) { vdcVoltage = 0; }
		}
		if (ch=='9' && configuringEMCRunSetup) {
			configuringEMCRunSetup = false;
		}
		ClearStateArea();
		mvprintw( 9,0,"Please Configure Run Parameters"); clrtoeol();
		mvprintw(10,0,"<1>Planetary: %s", MOTION_TYPE_DISPLAY(emcrunsetup.planetaryMotionType)); clrtoeol();
		mvprintw(11,0,"<2>Horizontal: %s", MOTION_TYPE_DISPLAY(emcrunsetup.horizontalMotionType)); clrtoeol();
		mvprintw(12,0,"<3>Heave: %s", MOTION_TYPE_DISPLAY(emcrunsetup.heaveMotionType)); clrtoeol();
		mvprintw(13,0,"<4>Roll: %s", MOTION_TYPE_DISPLAY(emcrunsetup.rollMotionType)); clrtoeol();
		mvprintw(14,0,"<5>Yaw: %s", MOTION_TYPE_DISPLAY(emcrunsetup.yawMotionType)); clrtoeol();
		mvprintw(15,0,"<6>Pitch: %s", MOTION_TYPE_DISPLAY(emcrunsetup.pitchMotionType)); clrtoeol();
		mvprintw(16,0,"<7>Voltage: %f", vdcVoltage); clrtoeol();
		mvprintw(17,0,"<9>Confirm"); clrtoeol();
		refresh();
	}


	mvprintw(4,0,"Sending EMC Run Setup");
	refresh();

	outgoingHeader.bodyLength = sizeof(emcrunsetup);
	outgoingHeader.messageId = MC_EMC_MESSAGE_ID_ENUM::MT_EMC_MC_RUN_SETUP;
	outgoingHeader.messageNumber = outgoingCount;

	mSocket->WriteData(&outgoingHeader, sizeof(MC_EMC_MESSAGE_HEADER));
	mSocket->WriteData(&emcrunsetup, sizeof(EMC_RUN_SETUP_STRUCT));

	outgoingCount++;
}

void ProcessKeyPresses(char ch)
{
	// Handle keyboard movement of device
	if (ch=='w')
	{
		commandState.pitchState +=.1;
	}
	if (ch=='s')
	{
		commandState.pitchState -=.1;
	}
	if (ch=='a')
	{
		commandState.rollState +=.1;
	}
	if (ch=='d')
	{
		commandState.rollState -=.1;
	}
	if (ch=='q')
	{
		commandState.yawState -=.1;
	}
	if (ch=='e')
	{
		commandState.yawState +=.1;
	}
	if (ch=='r')
	{
		commandState.heaveState +=.005;
	}
	if (ch=='f')
	{
		commandState.heaveState -=.005;
	}
	if (ch=='x')
	{
		commandState.horizontalState +=.05;
	}
	if (ch=='z')
	{
		commandState.horizontalState -=.05;
	}
	if (ch=='v')
	{
		commandState.planetaryState +=.1;
	}
	if (ch=='c')
	{
		commandState.planetaryState -=.1;
	}
	checkLimits();
}

bool ReadUDPCommand() // Get command from flightmodel
{
	EMC_RUN_TIME_DATA_STRUCT udpRunTimeIn;
	//unsigned int inIP;
	//unsigned short inPort;
	
	int udpInBytes = mSocketUDP->ReadData(&udpRunTimeIn, sizeof(EMC_RUN_TIME_DATA_STRUCT), &flightModelIP, &flightModelPort);
	while (udpInBytes > 0)
	{
		mvprintw(17, 0, "%d", udpInBytes);
		commandState.planetaryState = udpRunTimeIn.planetaryCommand;
		commandState.horizontalState = udpRunTimeIn.horizontalCommand;
		commandState.heaveState = udpRunTimeIn.heaveCommand;
		commandState.rollState = udpRunTimeIn.rollCommand;
		commandState.yawState = udpRunTimeIn.yawCommand;
		commandState.pitchState = udpRunTimeIn.pitchCommand;
		
		udpInBytes = mSocketUDP->ReadData(&udpRunTimeIn, sizeof(EMC_RUN_TIME_DATA_STRUCT), &flightModelIP, &flightModelPort);
	// TODO: Make sure there doesn't need to be an exit condition to return false.
	}
	return true;

	//if (udpInBytes > 0)
	//{
	//	commandState.planetaryState = udpRunTimeIn.planetaryCommand;
	//	commandState.horizontalState = udpRunTimeIn.horizontalCommand;
	//	commandState.heaveState = udpRunTimeIn.heaveCommand;
	//	commandState.rollState = udpRunTimeIn.rollCommand;
	//	commandState.yawState = udpRunTimeIn.yawCommand;
	//	commandState.pitchState = udpRunTimeIn.pitchCommand;
	//	return true;
	//} else {
	//	return false;
	//}

}

void WriteUDPcommand() // Send current device state back to flightmodel
{
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "%f,%f,%f,%f,%f,%f",
		actualState.planetaryState,
		actualState.horizontalState,
		actualState.heaveState,
		actualState.rollState,
		actualState.yawState,
		actualState.pitchState);
	mSocketUDP->WriteData(buffer, sizeof(buffer), flightModelIP, flightModelPort);

	if (UseSpider)
	{
		spider->Data("DRDActualPlanetery", actualState.planetaryState);
		spider->Data("DRDActualHorizontal", actualState.horizontalState);
		spider->Data("DRDActualHeave", actualState.heaveState);
		spider->Data("DRDActualRoll", actualState.rollState);
		spider->Data("DRDActualYaw", actualState.yawState);
		spider->Data("DRDActualPitch", actualState.pitchState);
		
		spider->Data("DRDCommandPlanetery", commandState.planetaryState);
		spider->Data("DRDCommandHorizontal", commandState.horizontalState);
		spider->Data("DRDCommandHeave", commandState.heaveState);
		spider->Data("DRDCommandRoll", commandState.rollState);
		spider->Data("DRDCommandYaw", commandState.yawState);
		spider->Data("DRDCommandPitch", commandState.pitchState);
		spider->Send();
	}
}

void ShowCurrentState()
{
	ClearStateArea();
	mvprintw( 9,0,"Command");
	mvprintw(10,0,"Planet: %f", commandState.planetaryState);
	mvprintw(11,0,"Horiz:  %f", commandState.horizontalState);
	mvprintw(12,0,"Heave:  %f", commandState.heaveState);
	mvprintw(13,0,"Roll:   %f", commandState.rollState);
	mvprintw(14,0,"Yaw:    %f", commandState.yawState);
	mvprintw(15,0,"Pitch:  %f", commandState.pitchState);
	mvprintw( 9,20,"Actual");
	mvprintw(10,20,"%f", actualState.planetaryState); clrtoeol();
	mvprintw(11,20,"%f", actualState.horizontalState); clrtoeol();
	mvprintw(12,20,"%f", actualState.heaveState); clrtoeol();
	mvprintw(13,20,"%f", actualState.rollState); clrtoeol();
	mvprintw(14,20,"%f", actualState.yawState); clrtoeol();
	mvprintw(15,20,"%f", actualState.pitchState); clrtoeol();
}

int main(int argc, char *argv[])
{
	WINDOW *w = initscr(); //initialize ncurses
	raw();
	cbreak();
	noecho();
	nodelay(w, TRUE);

	if (argc == 3)
	{
		mvprintw(0,0,"IP/Port Passed: %s:%s", argv[1], argv[2]);
		inPort = atoi(argv[2]);
		inServerIP = argv[1];
		refresh();
	}

	// Initialize Socket Call
	if (initSocket()) {
		mvprintw(1,0,"Socket Initialized");
	}
	else {
		mvprintw(1,0,"Socket Not Initialized");
	}

	//Initialize Spider for multicast
	spider->SetMulticastMode(true);
	bool initSuccess;
	try {
		initSuccess = spider->Init();
		UseSpider = true;
	}
	catch (std::exception &e)
	{
		//std::cerr << e.what() << std::endl;
		mvprintw(1,25,"Spider Not Initialized %s", e.what());
		UseSpider = false;
	}
	
	refresh();
	// Initial EMC Run Setup (Accepted and Position modes)
	emcrunsetup.accepted = 1;
	emcrunsetup.planetaryMotionType = 1;
	emcrunsetup.horizontalMotionType = 1;
	emcrunsetup.heaveMotionType = 1;
	emcrunsetup.rollMotionType = 1;
	emcrunsetup.yawMotionType = 1;
	emcrunsetup.pitchMotionType = 1;

	bool isConnected = mSocket->GetStatus() == PCSBSocket::status_Connected;
	bool isRunning = isConnected;
	while (isRunning)
	{
		ClearRunArea();

		bool initBufferFull = ReadUDPCommand();
		//default command state
		commandState.heaveState = 0;
		commandState.horizontalState = 0;
		commandState.pitchState = 0;
		commandState.planetaryState = 0;
		commandState.rollState = 0;
		commandState.yawState = 0;
		commandState.vdcVoltage = vdcVoltage;
		commandState.vdsBlackout = vdsBlackout;

		mvClearToEol(2,0);
		mvprintw(2,0,"Waiting for Run Setup (Esc to quit)");
		bool gotRunSetup = false;
		while (!gotRunSetup)
		{
			//PUT BACK
			//mSocket->ReadData(&incomingHeader, sizeof(MC_EMC_MESSAGE_HEADER));

			//TODO: PUT BACK TO NORMAL
			if (incomingHeader.messageId == MC_EMC_MESSAGE_ID_ENUM::MT_MC_EMC_RUN_SETUP || testClient == 1)
			{
				while (mSocket->ReadData(&mcrunsetup, incomingHeader.bodyLength) == 0)
				{
					//nothing
				}
				mvprintw(3,0,"Received MC Run Setup Message");
				gotRunSetup = true;

				//Send to client
				char msg[] = "Received MC Run Setup Message.";

				clientSocketUDP->WriteData(msg, sizeof(msg), clientIP, clientPortSend);

			} //else break; //did not receive Run Setup, exit loop???
			//allow user to cancel run
			if (getch()==27)
			{
				mvClearToEol(7,0);
				mvprintw(7,0,"Quit Key Pressed");
				isRunning = false;
				quitPressed = false;
				break;
			}
			refresh();
		}
		if (!isRunning) { break; } //exit isRunning Loop

		DrawDivider(8);
		DrawDivider(17);
		// Send EMC Run Setup
		//SendEMCRunSetup();

		///////////RECEIVE EMC SETUP///////////////////
		mvprintw(4,0,"Waiting for EMC Setup");
		char settings[18];
		refresh();
		unsigned int inIP;
		unsigned short inPort;

		int udpInBytes = clientSocketUDP->ReadData(settings, sizeof(settings), &inIP, &inPort);
		while (udpInBytes == -1)
		{			
			udpInBytes = clientSocketUDP->ReadData(settings, sizeof(settings), &inIP, &inPort);
			int a,b,c,d,e,f,g;
			sscanf(settings, "%i,%i,%i,%i,%i,%i,%i", &a,&b,&c,&d,&e,&f,&g);
			emcrunsetup.accepted = a;
			emcrunsetup.pitchMotionType = b;
			emcrunsetup.rollMotionType = c;
			emcrunsetup.yawMotionType = d;
			emcrunsetup.horizontalMotionType = e;
			emcrunsetup.heaveMotionType = f;
			emcrunsetup.planetaryMotionType = g;
		}

		mvprintw(10,0,"Planetary: %s", MOTION_TYPE_DISPLAY(emcrunsetup.planetaryMotionType)); clrtoeol();
		mvprintw(11,0,"Horizontal: %s", MOTION_TYPE_DISPLAY(emcrunsetup.horizontalMotionType)); clrtoeol();
		mvprintw(12,0,"Heave: %s", MOTION_TYPE_DISPLAY(emcrunsetup.heaveMotionType)); clrtoeol();
		mvprintw(13,0,"Roll: %s", MOTION_TYPE_DISPLAY(emcrunsetup.rollMotionType)); clrtoeol();
		mvprintw(14,0,"Yaw: %s", MOTION_TYPE_DISPLAY(emcrunsetup.yawMotionType)); clrtoeol();
		mvprintw(15,0,"Pitch: %s", MOTION_TYPE_DISPLAY(emcrunsetup.pitchMotionType)); clrtoeol();
		mvprintw(16,0,"Voltage: %f", vdcVoltage); clrtoeol();

		mvprintw(4,0,"Sending EMC Run Setup");
		refresh();

		outgoingHeader.bodyLength = sizeof(emcrunsetup);
		outgoingHeader.messageId = MC_EMC_MESSAGE_ID_ENUM::MT_EMC_MC_RUN_SETUP;
		outgoingHeader.messageNumber = outgoingCount;

		//TODO: Send data to MC
		//mSocket->WriteData(&outgoingHeader, sizeof(MC_EMC_MESSAGE_HEADER));
		//mSocket->WriteData(&emcrunsetup, sizeof(EMC_RUN_SETUP_STRUCT));
		/////////////////////////////////////////////////////

		mvprintw(5,0,"Waiting for MC Run Command");
		refresh();
		bool gotRunCmd = false;
		mcruncommand.command = 0;
		while (!gotRunCmd)
		{
			//PUT BACK
			//mSocket->ReadData(&incomingHeader, sizeof(MC_EMC_MESSAGE_HEADER));
			if (incomingHeader.messageId == MC_EMC_MESSAGE_ID_ENUM::MT_MC_EMC_COMMAND || testClient == 1)
			{
				// while (mSocket->ReadData(&mcruncommand, incomingHeader.bodyLength) == 0)FIX
				// {
				// 	//nothing
				// }
				//Remove later
				if(testClient == 1)
					mcruncommand.command = 1;
			} //else break; //did not receive run command, exit loop???
			if (mcruncommand.command == 1) { gotRunCmd = true; }
		}
		
		if (mcruncommand.command == 1)
		{
			mvprintw(6,0,"Received Run Command");

				//Send to client
			char runMsg[] = "Received Run Command.";

			clientSocketUDP->WriteData(runMsg, sizeof(runMsg), clientIP, clientPortSend);
			refresh();

			//default command state
			commandState.heaveState = 0;
			commandState.horizontalState = 0;
			commandState.pitchState = 0;
			commandState.planetaryState = 0;
			commandState.rollState = 0;
			commandState.yawState = 0;
			commandState.vdcVoltage = vdcVoltage;
			commandState.vdsBlackout = vdsBlackout;

			//Start EMC_Run loop thread 8ms
			stop_calls = false;
			thread call_back_thread( periodic_call, 8, ref(stop_calls), bind( EMC_Run )  );

			MC_RUN_TIME_DATA_STRUCT mcRunTime;

			bool isTraining = true;

			//Clearing input buffer
			bool isBufferFull = ReadUDPCommand();
			
			while (isTraining)
			{
				// Receive External Motion via UDP
				if (ReadUDPCommand()) { // if received a command, then send current state
					WriteUDPcommand();
				}
				// Receive Runtime MC Data
				while (mSocket->ReadData(&incomingHeader, sizeof(MC_EMC_MESSAGE_HEADER)) == 0 || testClient == 1)
				{
					//nothing
				}
				//PUT BACK
				if (incomingHeader.messageId == MC_EMC_MESSAGE_ID_ENUM::MT_MC_EMC_RUN_TIME)
				{
					// while (mSocket->ReadData(&mcRunTime, incomingHeader.bodyLength) == 0)
					// {
					// 	//nothing
					// }
					// Grab Current Actual State of DRD
					actualState.planetaryState = mcRunTime.planetarylActualPosition;
					actualState.horizontalState = mcRunTime.horizontalActualPosition;
					actualState.heaveState = mcRunTime.heaveActualPosition;
					actualState.rollState = mcRunTime.rollActualPosition;
					actualState.yawState = mcRunTime.yawActualPosition;
					actualState.pitchState = mcRunTime.pitchActualPosition;
				}
				//if(stop == stopma)
				if (incomingHeader.messageId == MC_EMC_MESSAGE_ID_ENUM::MT_MC_EMC_COMMAND)
				{
					while (mSocket->ReadData(&mcruncommand, incomingHeader.bodyLength) == 0)
					{
						//nothing
					}
					if (mcruncommand.command == 0)
					{
						mvprintw(7,0,"Received Stop Command");
						isTraining = false;
						stop_calls = true; // stop the calls
						call_back_thread.join(); // wait for the thread to finish
					}
				}
				char ch = getch();
				if (ch == 27)
				{
					mvprintw(7,0,"Sending Stop Command");
					isTraining = false;
					stop_calls = true; // stop the calls
					call_back_thread.join(); // wait for the thread to finish

					EMC_COMMAND_STRUCT emcStopCommand;
					emcStopCommand.command = 0;
					outgoingHeader.bodyLength = sizeof(emcStopCommand);
					outgoingHeader.messageId = MC_EMC_MESSAGE_ID_ENUM::MT_EMC_MC_COMMAND;
					outgoingHeader.messageNumber = outgoingCount;
					mSocket->WriteData(&outgoingHeader, sizeof(MC_EMC_MESSAGE_HEADER));
					mSocket->WriteData(&emcStopCommand, sizeof(EMC_COMMAND_STRUCT));

					outgoingCount++;
					quitPressed = false; //reset quit checker
				}
				ShowCurrentState();
				ProcessKeyPresses(ch);
			} //isTraining loop
		} else {
			mvprintw(7,0,"Received Stop Command");
			refresh();
		}

	} //isRunning loop
	if (!isConnected)
	{
		mvprintw(20,0,"Disconnected");
	} else {
		mvprintw(20,0,"Disconnecting");
		if (mSocket) mSocket->Disconnect();
	}
	mSocket->ShutdownNetworking(0);
	mSocketUDP->~PCSBSocketUDP();//?
	clientSocketUDP->~PCSBSocketUDP();
	mSocket = NULL;
	delete mSocket;
	spider = NULL;
	delete spider;
	mvprintw(22,0,"Exiting");
	refresh();
	endwin();
}

