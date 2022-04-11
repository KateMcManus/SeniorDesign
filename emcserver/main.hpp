#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <string.h>

double planetaryOffset = 555;

double vdcVoltage = 28;
BYTE vdsBlackout = 0;

bool UseSpider = false;

struct DRD_STATE_STRUCT
{
    double horizontalState;
    double planetaryState;
    double heaveState;
    double rollState;
    double yawState;
    double pitchState;
    double vdcVoltage;
    BYTE   vdsBlackout;
};
DRD_STATE_STRUCT actualState, commandState;

const double COMFORT_PLANETARY = 150;

struct MC_EMC_MESSAGE_HEADER
{
    UINT32 messageNumber;
    UINT16 bodyLength;
    BYTE messageId;
    char unused;
};

MC_EMC_MESSAGE_HEADER incomingHeader;
MC_EMC_MESSAGE_HEADER outgoingHeader;
MC_RUN_SETUP_STRUCT mcrunsetup;
MC_COMMAND_STRUCT mcruncommand;

EMC_RUN_SETUP_STRUCT emcrunsetup;

enum RUN_SETUP_MOTION_TYPE_ENUM
{
	MOTION_OFF = 0,
	MOTION_POSITION = 1,
	MOTION_VELOCITY = 2
};

char* MOTION_TYPE_DISPLAY(BYTE motionType)
{
	char* ch = new char[4];
	switch (motionType)
	{
	case MOTION_OFF:
		strcpy(ch, "Off");
		break;
	case MOTION_POSITION:
		strcpy(ch, "Pos");
		break;
	case MOTION_VELOCITY:
		strcpy(ch, "Vel");
		break;
	default:
		strcpy(ch, "   ");
		break;
	}
	return ch;
}

BYTE ToggleMotionType(BYTE currentMotionType, bool velocity_available)
{
	if (currentMotionType == RUN_SETUP_MOTION_TYPE_ENUM::MOTION_OFF)
	{
		return RUN_SETUP_MOTION_TYPE_ENUM::MOTION_POSITION;
	}
	else if (currentMotionType == RUN_SETUP_MOTION_TYPE_ENUM::MOTION_POSITION)
	{
		if (velocity_available) {
			return RUN_SETUP_MOTION_TYPE_ENUM::MOTION_VELOCITY;
		}
		else {
			return RUN_SETUP_MOTION_TYPE_ENUM::MOTION_OFF;
		}
	}
	else if (currentMotionType == RUN_SETUP_MOTION_TYPE_ENUM::MOTION_VELOCITY)
	{
		return RUN_SETUP_MOTION_TYPE_ENUM::MOTION_OFF;
	}
	return RUN_SETUP_MOTION_TYPE_ENUM::MOTION_OFF;
}

#endif // MAIN_H_INCLUDED
