# Project Description
NAMRU-Dayton currently has a motion system (MC) that can be controlled externally using an external motion control (EMC). Data for the pitch, roll, yaw, heave, planetary, and horizontal positions of the MC are sent to the EMC server. Run and stop commands are sent from the EMC to the MC. The goal of this project is to create a client application that simultaneously displays both sets of data in a single program, while also allowing the user to connect to the EMC server and send commands.

# Problem Statement:
Researchers at NAMRU-Dayton want to connect to the EMC server through a client that can run EMC configuration settings, collect MC and EMC data, and pause data collection. However, there is currently no single software that fulfills this need, and the current method of running the EMC is inconvenient.

# Inadequacy of Current Solutions:
The current EMC Server solution is a console application that requires the operator to remotely shell into the server via SSH terminal and manually run the EMC Server app as well as interact with it in the terminal window directly. The current app is not cohesive with the flow of operations for External Motion Control based research runs.

# Background Skills and Interest:

•	C++/C#
•	Python

•	Windows

•	Linux

•	Qt Creator
	oCross platform development 
	oUI design
	
•	User Datagram Protocol (UDP)

# Approach, Goals, and Experience:
I will create an application that allows the user to interact with the EMC server by sending commands and receiving data. Communication between the client and server will be done using UDP datagram packets. In the application, the user will be able to choose startup settings and start and stop data collection using the application’s GUI. Data received from the EMC server will be displayed to the user in the application and will include data collected from both the EMC server and the MC. The software will work on both Linux and Windows operating systems.
