## Milestones:
1.	The application can receive UDP packets over the correct port.
a.	Any data sent over the specified port is received by the application. Essentially, this is just getting the app to listen for datagrams and read them properly.
2.	The application can label and display data it has received in the UI.
a.	Information from the datagrams received in milestone 1 is displayed to the user in the GUI. The program should parse the data to look for specific values and properly label them in the GUI (looking for pitch, roll, yaw, heave, planetary, and horizontal values from MC and EMC).
3.	The user can open the EMC settings menu and select options. Their choices are saved by the application.
a.	These settings are for setting up the EMC before data collection. Part of this step will be getting a list of settings needed and finding the most user-friendly way to display them. These will also only need to be set once, at a specific time in the process (refer to design diagrams). I must make sure this is shown only at the proper time (don’t allow settings changes mid-flight).
4.	The application can send UDP packets to the server over the correct port.
a.	The client app will need to send several commands to the server. First, I will implement this by sending the user-selected settings to the EMC server.
5.	The user can stop and resume data collection using a button on the GUI.
a.	Like the settings, this should only be allowed to be used at a specific time. I have to make sure certain conditions are met before the user can press the button. Once pressed, the client should send a signal to the server (using UDP again) to stop or run the motion control. 
6.	The full application has been successfully tested with the EMC server on site.
a.	A full test run of everything the project should do. The client should be able to successfully interact with the server and run motion control using button commands. I will check for bugs or any unexpected behavior (very likely) from the client and get feedback. Running this test will require being in the office to connect to the EMC.
7.	Feedback has been collected from the test run and implemented. This is the finished product.
a.	Fix any unexpected behavior found in the test run. If time allows, implement extra features as requested. After this step, I should have a finished product. 
