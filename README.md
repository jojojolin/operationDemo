# Operation Board Game

This repository contains a detail guide to set up the UR3, Gripper and Leap Motion sensor for operation board game. It also contains the source code.


## Purpose of this game
Since our client wants to showcase the significance of reduced latency of 5G network, we thought doing a game might be a fun way to go. By comparing games played over 4G and 5G, users can easily feel how responsive the robot has become as the latency's reduced. The game also serves as an example how a potential use case of the future 5G could be. 


## How to play?

1. Hover your hand above the LeapMotion sensor, if you can see the colored skeleton of your hand from the LeapMotion Visualiser, your hand is now tracked by the sensor. 

2. Do a peace sign to activate the robot. Now you can move your hand up, down, left, right to control the robot to pick up bones.

3. Here's some special gestures you can do : 
	1. Clench your fist to close the gripper
	2. Do a rock-and-roll sign to drop the bone to the basket

4. You need to grab as many bones you can from the board to win the game.



## Software Overview

This project can be separated into 3 isolated parts, a hand tracker, a UR3 Robot arm driver, and a server that speaks in between the two systems. 



## Hand tracker

I chose to develop in C because LeapMotion has the most complete API in C.

The main job of this piece of code is it calculates the distance moved by the hand between frames and maps that to the next relative joint positions of the robot. The program also checks for any meaningful gestures defined by the game for triggerring certain predefined movement on the robot, such as closing the gripper when user clenches the fist. The translated information will be saved in file "Coordinate.txt" where the socket will read from.


## Socket Server

The reason why socket server and hand tracker were not written together in a single program is that I wanted to make them isolated from each other. In this way it is easier to maintain and debug. I coded the server in Python for its simplicity.The job of this program is to read strings from "Coordinates.txt" and send it to the client/robot. 


## UR3 Robot arm driver

This driver is written in Universal Robot Programming language (urp). It's multi-threaded, socket and driver each takes one thread. The joint positions received by the the socket is read by the driver code which then moves the robot arm to the target position. This is how a user's hand movement gets translated to robot arm movement. 