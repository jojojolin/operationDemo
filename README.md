# Operation Board Game Demo

This repository contains a detail guide to set up the UR3, Gripper and Leap Motion sensor for operation board game. It also contains the source code.


## How to play?

1. Hover your hand above the LeapMotion sensor, if you can see the colored skeleton of your hand from the LeapMotion Visualiser, your hand is now tracked by the sensor. 

2. Do a peace sign to activate the robot. Now you can move your hand up, down, left, right to control the robot on the other side to pick up bones.

3. Here's some special gestures you can do : 
	1. Clench your fist to close the gripper
	2. Do a rock & roll sign to drop the bone

4. You need to grab as many bones you can from the board to win the game.



## Software Overview

This project can be separated into 3 isolated parts, an infrared hand-gesture interpreter, a UR3 Robot arm driver, and a server that speaks in between the two systems. 



## Infrared hand-gesture interpreter

I chose to develop in C because it has the most complete API for LeapMotion - an infrared sensor used in this game to track a user's hand. 

What this piece of code does is it translates detected hand movements and meaningful gestures (part of the game design) into instructions that the robot arm can understand. The instruction, which is essentially a set of joint angles in radians that the robot will move to next, will be saved in a file "Coordinate.txt" where the socket can read from.


## Socket Server

The reason why socket was not coded together with the gesture interpreter program is that I wanted to make them isolated. It is easier to maintain and debug in this manner. Python is chosen to use for socket server development because of the simplicity of this language.This program will read strings in "Coordinates.txt" and send it to the client/robot. 


## UR3 Robot arm driver

Due to the limited programming flexibility on the operating system of robot arm controller, program is written in Universal Robot Programming language (urp). This is a multi-threaded program, one thread for socket and the other for driver. The new coordinates received by the the socket is read by the driver code which then moves the robot arm to the target position.