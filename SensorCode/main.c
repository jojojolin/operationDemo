//#undef __cplusplus
#define _CRT_SECURE_NO_WARNINGS
#include"main.h"

// To write to the file so that server can read recent data
void writeFile(int w, int s, int e, int wr1, int wr2, int wr3,int grip) {
	FILE *f = fopen("Coordinates.txt", "w");
	if (f == NULL) {
		printf("Unable to open the file");
		exit(1);
	}
	float waist = degToRad(w);
	float shoulder = degToRad(s);
	float elbow = degToRad(e);
	float wrist1 = degToRad(wr1);
	float wrist2 = degToRad(wr2);
	float wrist3 = degToRad(wr3);
	
	fprintf(f, "(%f,%f,%f,%f,%f,%f,%d,%f)", waist, shoulder, elbow, wrist1, wrist2, wrist3, grip, delay);
	printf("(%f,%f,%f,%f,%f,%f,%d,%f)", waist, shoulder, elbow, wrist1, wrist2, wrist3, grip, delay);
	fclose(f);
}

void drop() {
	printf("\nDrop the bone to the basket.");
	//move upwards a bit to avoid collision with other bones
	writeFile(ur3.waist,ur3.shoulder,ur3.elbow + 40, ur3.wrist1, ur3.wrist2, ur3.wrist3, 0);
	//set a longer delay to ensure smooth movement
	//move to basket with a delay of 2
	//millisleep(1000);
	delay = 3;
	//writeFile(74.22, -86.20, -77.05, -120.85, 98.62, 263.74, 0);
	ur3.grip = 0;
	//millisleep(3000);

	// drop/release the grip
	delay = 1;
	ur3.waist = 74.22;
	ur3.shoulder = -86.20;
	ur3.elbow = -78.0;
	ur3.wrist1 = -120.85;
	ur3.wrist2 = 98.62;// 93.25;
	ur3.wrist3 = 263.74;
	ur3.grip = 0;
	writeFile(74.22, -86.20, -78.0, -120.85, 98.62, 263.74, 0);
	millisleep(1000);//1000
	ur3.grip = 1;
	//move back to initial position with a delay of 2
	delay = 1.5;//2
	writeFile(2.26, -87.6, -91.5, -90.38, 93.25, 181.86, 1);
	millisleep(1500);//2000
	delay = 0;
	
}

/*Creating the Start Position*/
void init_pos() {
	printf("\nSetting Base Position");
	//writeFile(0, -182, 121, -120, 90, 270, 1);
	writeFile(2.26, -87.6, -91.5, -90.38, 93.25, 181.86, 1);
	ur3 = { 2.26f, -87.6f, -91.5f, -90.38f, 93.25f, 181.86f, 1 };   //old {3.80, -95, -82, -90, 85, 270, 1}
}


/*Converting To Radians, This is for the robot input values*/
float degToRad(int val) {
	return val * (M_PI / 180);
}

// to verify if the variable is within the range
float check_range(float value, float max_val, float min_val) {
	if (value > max_val)
		return max_val;
	else if (value < min_val)
		return min_val;
	else
		return value;
}

// mapping Leap Motion Space to UR3 Space
float map(float value, float istart, float istop, float ostart, float ostop) {
	value = ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
	return value;
}

//Calculating the Distance between the points.
float distanceMove(LEAP_HAND* hand) {
	float distx = pow((hand->palm.position.x - prevx),2);
	float disty = pow((hand->palm.position.y - prevy), 2);
	float distz = pow((hand->palm.position.z - prevz), 2);
	return sqrtf(distx+disty+distz);
}

void setUR3Values(LEAP_HAND* hand) {
	ur3.grip = chkGrip(hand);
	//ur3.waist = getWaist(hand->palm.position.x);
	//ur3.shoulder = getShoulder(hand->palm.position.z);
	//ur3.elbow = getElbow(hand,ur3.shoulder);
	//ur3.wrist1 = getWrist1(ur3.elbow, ur3.shoulder, hand);
	//ur3.wrist2 = getWrist2(hand);
	//ur3.wrist3 = getWrist3(hand);

	ur3.waist = getWaist(hand->palm.position.x);//2.26; //static first for testing, later can use the old function and modify based on it

	if (ur3.shoulder - prev_shoulder > 10) prev_shoulder = ur3.shoulder;
	ur3.shoulder = getShoulder(hand->palm.position.z);
	if (ur3.elbow - prev_elbow > 10) prev_elbow = ur3.elbow;
	ur3.elbow = getElbow(hand);
	
	ur3.wrist1 = getWrist1(ur3.elbow, ur3.shoulder, hand);
	ur3.wrist2 = getWrist2(hand);// 93.25;
	ur3.wrist3 = setWrist3(hand);

	//return true;
}


boolean chkGrip(LEAP_HAND* hand) {
	/***v2**/
	int count = 0;
	if (is_extended == true) {
		for (int i = 0; i < 5; i++) {
			if (hand->digits[i].is_extended == 1) {
				count++;
			}
		}
		if (count < 1) {
			is_extended = false;
		}
	}
	else {
		for (int i = 0; i < 5; i++) {
			if (hand->digits[i].is_extended == 1) {
				count++;
			}
		}
		if (count == 5) {
			is_extended = true;
		}
	}
	return is_extended;
}

int getWaist(float HandX) {
	HandX = check_range(HandX, LM_MAX_X, LM_MIN_X);
	return map(HandX,LM_MAX_X,LM_MIN_X, WAIST_MIN, WAIST_MAX);
}

int getShoulder(float HandZ) {
	HandZ = check_range(HandZ, LM_MAX_Z, LM_MIN_Z);
	float value = map(HandZ, LM_MAX_Z, LM_MIN_Z, SHOULDER_MAX, SHOULDER_MIN);
	return value;
}

int getElbow(LEAP_HAND* hand) {
	int elbow = check_range(hand->palm.position.y, LM_MAX_Y, LM_MIN_Y);
	elbow = map(elbow, LM_MAX_Y, LM_MIN_Y, ELBOW_MAX, ELBOW_MIN);
	float moved = ur3.shoulder - prev_shoulder; //added to compensate for shoulder movement
	return elbow-moved;//(elbow - (shoulder + 100));
	//return elbow;
}

int getWrist1(int elbow, int shoulder, LEAP_HAND* hand) {

	float wrist_palm_y = hand->palm.position.y - hand->arm.next_joint.y;
	wrist_palm_y = check_range(wrist_palm_y, LM_PITCH_UP, LM_PITCH_DOWN);
	float wrist1 = map(wrist_palm_y, LM_PITCH_UP, LM_PITCH_DOWN, WRIST1_UP, WRIST1_DOWN);
	float moved_shoulder = ur3.shoulder - prev_shoulder;
	float moved_elbow = ur3.elbow - prev_elbow;
	printf("shoulder moved by : %f \n",moved_shoulder);
	printf("elbow moved by : %f \n", moved_elbow);

	return  wrist1;//wrist1 -47
}

void moveY(float y) {
	//increase
	//case 1: if shoulder+y >=-87.6 -> no action
	//case 2: else -> move up shoulder, and at the same time normalise elbow by moving it down with the same amount

	//decrease
	//case 1: if shoulder -y < = 
}

int getWrist2(LEAP_HAND* hand) {
	float wrist_palm_x = hand->palm.position.x - hand->arm.next_joint.x;//hand->palm.position.y - hand->arm.next_joint.y;
	wrist_palm_x = check_range(wrist_palm_x, LM_YAW_RIGHT, LM_YAW_LEFT);
	wrist_palm_x = map(wrist_palm_x, LM_YAW_LEFT, LM_YAW_RIGHT, WRIST2_LEFT, WRIST2_RIGHT);
	return wrist_palm_x;
}

int getWrist3(LEAP_HAND* hand) {
	//printf("rotation palm %f %f %f \n",hand->arm.rotation.x, hand->arm.rotation.y, hand->arm.rotation.z);
	float wrist_roll_z = hand->arm.rotation.z;
	wrist_roll_z = check_range(wrist_roll_z, LM_ROLL_LEFT, LM_ROLL_RIGHT);
	wrist_roll_z = map(wrist_roll_z, LM_ROLL_LEFT, LM_ROLL_RIGHT, WRIST3_LEFT, WRIST3_RIGHT);
	return wrist_roll_z;
}

void onConnection() {
	OpenConnection();
	while (!IsConnected)
		millisleep(100); //wait a bit to let the connection complete
	printf("Connected = %s\n", IsConnected ? "true" : "false");
}

/*Checking for the gesture for activating the robot*/
boolean check_gesture() {
	LEAP_TRACKING_EVENT *frame = GetFrame();  //get the current frame
	if (frame && (frame->tracking_frame_id > lastFrameID)) { //check if frame is valid
		lastFrameID = frame->tracking_frame_id; //update the old frame id
		//printf("Frame %lli with %i hands.\n", (long long int)frame->tracking_frame_id, frame->nHands);
		if (frame->nHands > 0) { // if frame is not empty
			LEAP_HAND* hand = &frame->pHands[0]; //get the first tracked hand
			//printf("\nIndex is extended %d\n", hand->index.is_extended);
			//if the gesture is a peace sign
			if (hand->index.is_extended == 1 && hand->middle.is_extended == 1 && hand->pinky.is_extended == 0 && hand->ring.is_extended == 0 && hand->thumb.is_extended == 0) {
				return true;
			}
		}
	}
	return false;
}





void trackHand() {
	LEAP_TRACKING_EVENT *frame = GetFrame();
	if (frame && (frame->tracking_frame_id > lastFrameID)) {
		lastFrameID = frame->tracking_frame_id; //update previous frame id to current frame id
		int list_size = frame->nHands;
		LEAP_HAND* hand = &frame->pHands[0];
		if (list_size == 0) {
			bactiveMove = false;
			handID = -1;
			init_pos();
			return;
		}
		if (bactiveMove) {
			//find the tracked hand 
			for (int i = 0; i < list_size; i++) {
				if (hand->id == handID) break;
				if (i == list_size - 1) {
					//no hand with prev tracked handID found in the current frame
					bactiveMove = false;
					handID == -1;
					init_pos();
					break;
				}
				*hand++;
			}
			if (bactiveMove && distanceMove(hand) > MOVE_THRESHOLD) {//move ur3 if move threshold is reached
				if (index_set == false) {
					*prev_index = hand->index.metacarpal.next_joint;
					*prev_palm = hand->palm.position;
				}
				setUR3Values(hand);//, &ur3. map the hand readings to robot pose
				writeFile(ur3.waist, ur3.shoulder, ur3.elbow, ur3.wrist1, ur3.wrist2, ur3.wrist3, ur3.grip);//write the readings to a file which will be read by python socket
				//update the previous palm position
				prevx = hand->palm.position.x;
				prevy = hand->palm.position.y;
				prevz = hand->palm.position.z;
				//update prev_index and palm
				*prev_index = hand->index.metacarpal.next_joint;
				*prev_palm = hand->palm.position;
				return;
			}
			else if (bactiveMove && ur3.grip == 0 && hand->thumb.is_extended == 0 && hand->index.is_extended == 1 && hand->middle.is_extended == 0 && hand->ring.is_extended == 0 && hand->pinky.is_extended == 1) {
				drop();
				bactiveMove = false;
				handID = -1;
				return;
			}
		}
		if (!bactiveMove) {//Detect signals for activation
			for (int i = 0; i < list_size; i++) {
				if (hand->palm.position.x > LM_MIN_X && hand->palm.position.x < LM_MAX_X && hand->palm.position.y < LM_MAX_Y && hand->palm.position.z < LM_MAX_Z && hand->palm.position.z > LM_MIN_Z) {
					//check for "one" sign -> 4G version
					if (hand->thumb.is_extended == 0 && hand->index.is_extended == 1 && hand->middle.is_extended == 0 && hand->ring.is_extended == 0 && hand->pinky.is_extended == 0) {
						handID = hand->id;
						bactiveMove = true;
						delay = DELAY_4G;
						break;
					}
					//check for peace sign -> 5G version
					if (hand->thumb.is_extended == 0 && hand->index.is_extended == 1 && hand->middle.is_extended == 1 && hand->ring.is_extended == 0 && hand->pinky.is_extended == 0) {
						handID = hand->id;
						bactiveMove = true;
						delay = DELAY_5G;
						break;
					}
				}
				*hand++;
			}
		}
	}
}


float dist(LEAP_VECTOR a, LEAP_VECTOR b) {//in millimeter
	float x = pow((a.x - b.x), 2);
	float y = pow((a.y - b.y), 2);
	float z = pow((a.z - b.z), 2);
	return sqrtf(x + y + z);
}

float setWrist3(LEAP_HAND* hand) {
	float y = hand->index.metacarpal.next_joint.y;
	for (int i = 2; i < 5; i++) {
		if (abs(int(y - hand->digits[i].metacarpal.next_joint.y)) > ERROR_RANGE) break; // not parallel vertically, break from the loop
		if (i == 4) return W3_ROTATION_2; // if all 4 are parallel vertically, set wrist3 to pos1
	}
	float x = hand->index.metacarpal.next_joint.x;
	for (int i = 2; i < 5; i++) {
		if (abs(int(x - hand->digits[i].metacarpal.next_joint.x)) > ERROR_RANGE) break; // not parallel horizontally, break from the loop
		if (i == 4) return W3_ROTATION_1; // if all 4 are parallel horizontally, set wrist3 to pos2
	}
	//calcuate the angle turned
	y = y - hand->pinky.metacarpal.next_joint.y; //diff in y betw index & pinky
	x = x - hand->pinky.metacarpal.next_joint.x; //diff in x betw index & pinky
	if ((int)x == 0) {
		return W3_ROTATION_2;
	}
	float angle = atan(abs((int) y)/abs((int) x));
	if (prev_index->x > hand->index.metacarpal.next_joint.x) angle*= -1;

	printf("Angle turned: %f", 181.60 + angle * 180 / M_PI);
	return 181.60+angle*180/M_PI;//getWrist3(hand)
	//return ur3.wrist3; // simply return the previous rotation since it doesnt change much.
}


void trackHands() { 
	LEAP_TRACKING_EVENT *frame = GetFrame(); 
	// if frame is not empty and the id of the frame is larger than the previous frame id
	if (frame && (frame->tracking_frame_id > lastFrameID)) {
		lastFrameID = frame->tracking_frame_id; //update previous frame id to current frame id
		//printf("Frame %lli with %i hands.\n", (long long int)frame->tracking_frame_id, frame->nHands);
		if (frame->nHands > 0) {//check if number of hands detected is more than 0. nHands stores the number of hands in the frame
			handCheck = 0;
			LEAP_HAND* hand = &frame->pHands[0]; //LEAP_HAND knownhand = frame.hand(handID) use hand.id() to get the id of the tracked hand the pose of the first hand

			float distance_moved = distanceMove(hand); //This is to calculate the distance moved by the user's palm.
			//printf("%f\n", distance_moved);
			if (distance_moved > MOVE_THRESHOLD) {
				setUR3Values(hand);//commented out &ur3, map the hand readings to robot pose
				writeFile(ur3.waist, ur3.shoulder, ur3.elbow, ur3.wrist1, ur3.wrist2, ur3.wrist3, ur3.grip);//write the readings to a file which will be read by python socket
				//update the previous palm position
				prevx = hand->palm.position.x;
				prevy = hand->palm.position.y;
				prevz = hand->palm.position.z;
			}

			printf("%s hand with ur3 setup (%d, %d, %d, %d,%d).\n",
				(hand->type == eLeapHandType_Left ? "left" : "right"),
				ur3.waist,
				ur3.shoulder,
				ur3.elbow,
				ur3.wrist1,
				ur3.grip);
		}
		else {
			handCheck++;
			if (handCheck >= HAND_RESET) {
				init_pos();
				bactiveMove = false;
			}
		}
			
	}
}


/*void socketCommunication() {
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	char *hello = "Hello from server";

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST,
		&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 30000
	if (bind(server_fd, (struct sockaddr *)&address,
		sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
		(socklen_t*)&addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	valread = read(new_socket, buffer, 1024);
	printf("%s\n", buffer);
	send(new_socket, hello, strlen(hello), 0);
	printf("Hello message sent\n");
	return 0;
}*/

int main(int argc, char** argv) {
	onConnection();
	LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
	if (deviceProps)
		printf("Using device %s.\n", deviceProps->serial);
	init_pos();
	for (;;) {
		trackHand();
		/*if (bactivemove == false) {
			bactivemove = check_gesture();
			if(bactivemove)
				printf("\nrobot activated %d", bactivemove);
		}
		else
			trackhand();*/
			//trackHand(controller); //c++ replacement
		//socketCommunication();
	} //ctrl-c to exit
	
	return 0;
}


void moveZ(float dist, float* elbow, float* shoulder) { //dist passed in is in LeapMotion world
	printf("Distance in z direction in LM: %f\n",dist);
	dist = map(dist, LM_MIN_Z, LM_MAX_Z, UR3_MIN_Z, UR3_MAX_Z);
	dist = abs((int)dist);
	dist = check_range(dist, UR3_MIN_Z, UR3_MAX_Z);
	printf("Distance = %f",dist);
	//distance is in cm
	float theta = acos((pow(ARM1, 2) + pow(ARM2, 2) - pow(dist, 2)) / (2 * ARM1*ARM2));
	*elbow = theta*180/M_PI - 180;
	printf("elbow angle = %f",*elbow);
	float beta = acos((pow(ARM1, 2) + pow(dist, 2) - pow(ARM2, 2)) / (2 * ARM1*dist));
	*shoulder = beta * 180 / M_PI - 180;
	printf("shoulder angle = %f", *shoulder);
}
//End-of-Sample