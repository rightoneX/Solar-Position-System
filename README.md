# Solar Position System

Sun Tracking System for a Solar Reflector 

 * Created by Alex Ustinov @2021/10
 * The code is design to control a solar mirror by horizontal and vertical position based on the signals from array of four Photo Sensors.
 * During the day time, when the lux level is high the system will track the sun movement and adjust the mirror position accordingly.
 * After the sun set the solar reflector will be return to start point.
 * Based on the special requirements received from client the solar reflector will be raised to the top position be for return it to the start point.
 * all names and numbers had been set based if you were stating at front of the solar reflector and looking on it.

## How to deploy the code

Install platformIO module for Visual Studio Code

### Board IO

The software design for Arduino Nano Board
The electrical circuit board drawing can be found /drawing


> Input
- А0	Photo Sensor 1		
- А1    Photo Sensor 2		
- А2	Photo Sensor 3		
- А3	Photo Sensor 4		
- А4	Head Temperature Sensor 		
- А5	Radiator Temperature Sensor	
- D2	Left Horizontal Stop Switch
- D3	Right Horizontal Stop Switch
- D4	Top Vertical Stop Switch
- D5	Bottom Vertical Stop Switch

>Output
- D6	Left Direction Horizontal Motor
- D7	Right Direction Horizontal Motor
- D8	Top Direction Vertical Motor
- D9	Bottom Direction Vertical Motor
- D10	Water Pump
- D11	Fan Control
