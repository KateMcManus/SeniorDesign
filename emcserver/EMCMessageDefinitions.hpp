/******************************************************************************
*                        NOTICE OF CONFIDENTIALITY                            *
*                                                                             *
* This source code is proprietary to ETC and is confidential in nature.       *
* This source code may not be used in any form for any purpose other than     *
* its intended use without written permission from ETC.                       *
*                                                                             *
*         Copyright (c) 2000, Environmental Tectonics Corporation             *
******************************************************************************/

#ifndef EMCMESSAGEDEFINITIONS_H
#define EMCMESSAGEDEFINITIONS_H

#include <stdint.h>

typedef unsigned char                           BYTE;      // unsigned char, 1 byte
typedef uint32_t                                UINT32;
typedef unsigned short                          UINT16;
/*!
\brief EMC interface message definitions.

\author Seyit Sabri Seyhan
\date   02/02/2011
*/

// MC <-> EMC message definitions
enum MC_EMC_MESSAGE_ID_ENUM
{
	MT_EMC_MC_RUN_SETUP = 0,
	MT_EMC_MC_RUN_TIME = 1,
	MT_EMC_MC_COMMAND = 2,

	MT_MC_EMC_RUN_SETUP = 10,
	MT_MC_EMC_RUN_TIME = 11,
	MT_MC_EMC_COMMAND = 12,

	MC_EMC_MESSAGE_NUMBER
};

//-------------------- MC Run setup message definitions ----------------------

struct EMC_MOTION_LIMITS_STRUCT
{
	double pitchVelocity;
	double pitchAcceleration;
	double rollVelocity;
	double rollAcceleration;
	double yawVelocity;
	double yawAcceleration;
	double horizontalMinPosition;
	double horizontalMaxPosition;
	double horizontalVelocity;
	double horizontalAcceleration;
	double heaveMinPosition;
	double heaveMaxPosition;
	double heaveVelocity;
	double heaveAcceleration;
	double planetaryVelocity;
	double planetaryAcceleration;
};

struct MC_RUN_SETUP_STRUCT
{
	EMC_MOTION_LIMITS_STRUCT   motionLimits;
	BYTE                       emcTrainingTypeId;  // [ 0 : 255 ]. Represents EMC training type. Candidate types are listed in configuration file in [id, name, isGMode] format.
	char                       unused[7];
};

//-------------------- EMC Run setup message definitions ----------------------

struct EMC_RUN_SETUP_STRUCT
{
	BYTE accepted;                  // [ 0 : 1 ] => 0 : FALSE, 1 : TRUE. True if MC run setup parameters are accepted. False, otherwise.
	BYTE pitchMotionType;           // [0:2] => 0 : No Motion, 1: Position, 2 : Velocity
	BYTE rollMotionType;            // [0:2] => 0 : No Motion, 1: Position, 2 : Velocity
	BYTE yawMotionType;             // [0:2] => 0 : No Motion, 1: Position, 2 : Velocity
	BYTE horizontalMotionType;      // [0:1] => 0 : No Motion, 1: Position
	BYTE heaveMotionType;           // [0:1] => 0 : No Motion, 1: Position
	BYTE planetaryMotionType;       // [0:2] => 0 : No Motion, 1: Position, 2 : Velocity
	char unused;
};

//-------------------- MC Runtime data message definitions ----------------------

struct MC_RUN_TIME_DATA_STRUCT
{
	double elapsedRuntime;              // elapsed time after the EMC run has started
	double pitchActualPosition;         // [deg]. Actual pitch position.
	double pitchActualVelocity;         // [deg/s]. Actual pitch velocity.
	double rollActualPosition;          // [deg]. Actual roll position.
	double rollActualVelocity;          // [deg/s]. Actual roll velocity.
	double yawActualPosition;           // [deg]. Actual yaw position.
	double yawActualVelocity;           // [deg/s]. Actual yaw velocity.
	double horizontalActualPosition;    // [ft]. Actual horizontal position.
	double horizontalActualVelocity;    // [ft/s]. Actual horizontal velocity.
	double heaveActualPosition;         // [ft]. Actual heave position.
	double heaveActualVelocity;         // [ft/s]. Actual heave velocity.
	double planetarylActualPosition;    // [deg]. Actual planetary position.
	double planetaryActualVelocity;     // [deg/s]. Actual planetary velocity.
};


//-------------------- EMC Runtime data message definitions ----------------------

struct EMC_RUN_TIME_DATA_STRUCT
{
	double pitchCommand;        //  Depending on the motion type defined in the RunSetup, one of these values:
								//  • No Motion : There is no motion at this axis.
								//  • [ 0 : 360 ) [deg]. Position command that controls pitch motion.
								//  • [-180 : 180] [deg/s]. Velocity command that controls the pitch motion.
	double rollCommand;         //  Depending on the motion type defined in the RunSetup, one of these values:
								//  • No Motion : There is no motion at this axis.
								//  • [ 0 : 360 ) [deg]. Position command that controls roll motion.
								//  • [-180 : 180] [deg/s]. Velocity command that controls the roll motion.
	double yawCommand;          //  Depending on the motion type defined in the RunSetup, one of these values:
								//  • No Motion : There is no motion at this axis.
								//  • [ 0 : 360 ) [deg]. Position command that controls yaw motion.
								//  • [-180 : 180] [deg/s]. Velocity command that controls the yaw  motion.
	double horizontalCommand;   //  Depending on the motion type defined in the RunSetup, one of these values:
								//  • No Motion : There is no motion at this axis.
								//  • [ -17 : 17 ] [ft]. Position command that controls horizontal motion.
								//  • [-11 : 11] [ft/s]. Velocity command that controls the horizontal motion.
	double heaveCommand;        //  Depending on the motion type defined in the RunSetup, one of these values:
								//  • No Motion : There is no motion at this axis.
								//  • [ -3 : 3 ] [ft]. Valid for non-G modes. Position command that controls heave motion.
								//  • [-6 : 6] [ft/s]. Velocity command that controls the heave motion.
	double planetaryCommand;    //  Depending on the motion type defined in the RunSetup, one of these values:
								//  • No Motion : There is no motion at this axis.
								//  • [ 0 : 360 ) [deg]. Position command that controls planetary  motion.
								//  • [ -150 : 150 ] [deg/s]. Velocity command that controls planetary motion.
	double vdcSetPoint_0_30V;	//  VDC Power Supply Voltage
								//	• [ 0 : 30 ] [volts].
	BYTE vdsBlackoutStatus;		//  VDS Blackout state
								//	• [ 0 : 1 ] 0 : Not Blackout 1 : Blackout
	char unused[7];
};


//-------------------- MC Command message definitions ----------------------

struct MC_COMMAND_STRUCT
{
	BYTE command;       // [ 0 : 1 ] => 0 : STOP, 1: RUN.
	char unused[7];
};

//-------------------- EMC Command message definitions ----------------------

struct EMC_COMMAND_STRUCT
{
	BYTE command;       // [ 0 ] => 0 : STOP.
	char unused[7];
};



#endif  /* EMCMESSAGEDEFINITIONS_H */
