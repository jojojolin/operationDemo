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
	writeFile(ur3->waist, ur3->shoulder, ur3->elbow, ur3->wrist1 + 10, ur3->wrist2, ur3->wrist3, 0);
	//set a longer delay to ensure smooth movement
	//move to basket with a delay of 2
	millisleep(1000);
	delay = 3;
	//writeFile(74.22, -86.20, -77.05, -120.85, 98.62, 263.74, 0);
	ur3->grip = 0;
	//millisleep(3000);

	// drop/release the grip
	delay = 1;
	ur3->waist = 74.22;
	ur3->shoulder = -86.20;
	ur3->elbow = -78.0;
	ur3->wrist1 = -120.85;
	ur3->wrist2 = 98.62;// 93.25;
	ur3->wrist3 = 263.74;
	ur3->grip = 0;
	//intermediate waypoint
	writeFile(-38.96f, -84.25f, -89.89f, 5.39f, 77.77f, 222.5f, 0);//wrist was -1.39f
	millisleep(1000);//added

	delay = 1.5;
	writeFile(-86.93f, -87.15f, -68.10f, -110.43f, 89.93f, 180.93f, 0);//-88.05f, -86.20f, -97.0f, -75.37f, 89.93f, 263.74f, 0
	millisleep(1500);//1000
	delay = 1.5;//stay for 1.5 (was 1) second after gripper has openned
	//open the gripper
	writeFile(-86.93f, -87.15f, -68.10f, -110.43f, 89.93f, 180.93f, 1);//-88.05f, -86.20f, -97.0f, -75.37f, 89.93f, 263.74f, 1
	millisleep(1500);// was 1000

	ur3->grip = 1;
	//move back to initial position with a delay of 1.5
	delay = 1.5;//stable number
	writeFile(2.26, -87.6, -91.5, -90.38, 93.25, 181.86, 1); //2.26, -87.6, -91.5, -90.38, 93.25, 181.86, 1
	millisleep(1500);//stable number
	delay = 0;
}

/*Creating the Start Position*/
void init_pos() {
	printf("\nSetting Base Position");
	//writeFile(0, -182, 121, -120, 90, 270, 1);
	writeFile(2.26, -87.6, -91.5, -90.38, 93.25, 278.67, 1);
	ur3->waist = 2.26f;
	ur3->shoulder = -87.6f;
	ur3->elbow = -91.5f;
	ur3->wrist1 = -90.38f;
	ur3->wrist2 = 93.25f;
	ur3->wrist3 = 278.67f;
	ur3->grip = 1;
  //old {3.80, -95, -82, -90, 85, 270, 1}
	prev_shoulder = -87.6;
	prev_elbow = -91.5;
	prevx = 0;
	prevy = 0;
	prevz = 0;
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
	ur3->grip = chkGrip(hand);
	ur3->waist = getWaist(hand->palm.position.x);

	//threshold was >10
	if (ur3->shoulder - prev_shoulder > 5)  prev_shoulder = ur3->shoulder;
	ur3->shoulder = getShoulder(hand->palm.position.z);
	if (ur3->elbow - prev_elbow > 5) prev_elbow = ur3->elbow;
	ur3->elbow = getElbow(hand);
	
	ur3->wrist1 = getWrist1(ur3->elbow, ur3->shoulder, hand);
	ur3->wrist2 = getWrist2(hand);
	//if in the rib area(see joints manually by freedrive), then twist to make gripper horizontal, else vertical.
	ur3->wrist3 = 278.67;//setWrist3(hand);
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
	float max = ELBOW_MAX;
	float min = ELBOW_MIN;
	if (ur3->shoulder > 120 && ur3->shoulder < -90) max = -82; // middle area (heart + rib) was -86
	else if (ur3->shoulder >= -90) { max = -91.5; min = -122; } //move backwards to pick up right arm bone
	elbow = map(elbow, LM_MAX_Y, LM_MIN_Y, max, min);
	float moved = ur3->shoulder - prev_shoulder; //added to compensate for shoulder movement
	return check_range(elbow - moved, max, min);
}

int getWrist1(int elbow, int shoulder, LEAP_HAND* hand) {

	float wrist_palm_y = hand->palm.position.y - hand->arm.next_joint.y;
	wrist_palm_y = check_range(wrist_palm_y, LM_PITCH_UP, LM_PITCH_DOWN);
	float wrist1 = map(wrist_palm_y, LM_PITCH_UP, LM_PITCH_DOWN, WRIST1_UP, WRIST1_DOWN);
	float moved_shoulder = ur3->shoulder - prev_shoulder;
	float moved_elbow = ur3->elbow - prev_elbow;
	printf("shoulder moved by : %f \n",moved_shoulder);
	printf("elbow moved by : %f \n", moved_elbow);
	// To avoid singularity
	if (wrist1 - moved_elbow > 37480000 || wrist1 - moved_elbow < -37480000) return ur3->wrist1;
	return  check_range(wrist1-moved_elbow, WRIST1_UP, WRIST1_DOWN);//wrist1 -47
}

int getWrist2(LEAP_HAND* hand) {
	float wrist_palm_x = hand->palm.position.x - hand->arm.next_joint.x;//hand->palm.position.y - hand->arm.next_joint.y;
	wrist_palm_x = check_range(wrist_palm_x, LM_YAW_RIGHT, LM_YAW_LEFT);
	wrist_palm_x = map(wrist_palm_x, LM_YAW_LEFT, LM_YAW_RIGHT, WRIST2_LEFT, WRIST2_RIGHT);
	//to avoid singularity
	if (wrist_palm_x > 37480000 || wrist_palm_x < -37480000) return ur3->wrist2;
	return wrist_palm_x;
}

int getWrist3(LEAP_HAND* hand) {
	//printf("rotation palm %f %f %f \n",hand->arm.rotation.x, hand->arm.rotation.y, hand->arm.rotation.z);
	float wrist_roll_z = hand->arm.rotation.z;
	wrist_roll_z = check_range(wrist_roll_z, LM_ROLL_LEFT, LM_ROLL_RIGHT);
	wrist_roll_z = map(wrist_roll_z, LM_ROLL_LEFT, LM_ROLL_RIGHT, WRIST3_LEFT, WRIST3_RIGHT);
	//to avoid singularity
	if (wrist_roll_z > 37480000 || wrist_roll_z < -37480000) return ur3->wrist3;
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
				writeFile(ur3->waist, ur3->shoulder, ur3->elbow, ur3->wrist1, ur3->wrist2, ur3->wrist3, ur3->grip);//write the readings to a file which will be read by python socket
				//update the previous palm position
				prevx = hand->palm.position.x;
				prevy = hand->palm.position.y;
				prevz = hand->palm.position.z;
				//update prev_index and palm
				*prev_index = hand->index.metacarpal.next_joint;
				*prev_palm = hand->palm.position;
				return;
			}
			else if (bactiveMove && ur3->grip == 0 && hand->thumb.is_extended == 0 && hand->index.is_extended == 1 && hand->middle.is_extended == 0 && hand->ring.is_extended == 0 && hand->pinky.is_extended == 1) {
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
		if (i == 4) return W3_ROTATION_1; // if all 4 are parallel vertically, set wrist3 to pos1
	}
	float x = hand->index.metacarpal.next_joint.x;
	for (int i = 2; i < 5; i++) {
		if (abs(int(x - hand->digits[i].metacarpal.next_joint.x)) > ERROR_RANGE) break; // not parallel horizontally, break from the loop
		if (i == 4) return W3_ROTATION_2; // if all 4 are parallel horizontally, set wrist3 to pos2
	}
	//calcuate the angle turned
	y = y - hand->pinky.metacarpal.next_joint.y; //diff in y betw index & pinky
	x = x - hand->pinky.metacarpal.next_joint.x; //diff in x betw index & pinky
	if ((int)x == 0) {
		return W3_ROTATION_1;
	}
	float angle = atan(abs((int) y)/abs((int) x));
	if (prev_index->x > hand->index.metacarpal.next_joint.x) angle*= -1;

	printf("Angle turned: %f", 181.60 + angle * 180 / M_PI);
	return 181.60+angle*180/M_PI;//getWrist3(hand)
	//return ur3.wrist3; // simply return the previous rotation since it doesnt change much.
}




void trackHandSimple() {
	LEAP_TRACKING_EVENT *frame = GetFrame();
	// if frame is not empty and the id of the frame is larger than the previous frame id
	if (frame && (frame->tracking_frame_id > lastFrameID)) {
		lastFrameID = frame->tracking_frame_id;
		if (frame->nHands > 0) {
			handCheck = 0;
			LEAP_HAND* hand = &frame->pHands[0];

			float distance_moved = distanceMove(hand); //This is to calculate the distance moved by the user's palm.
			printf("%f\n", distance_moved);
			if (ur3->grip == 0 && hand->thumb.is_extended == 0 && hand->index.is_extended == 1 && hand->middle.is_extended == 0 && hand->ring.is_extended == 0 && hand->pinky.is_extended == 1) {
				drop();
				bactiveMove = false;
				return;
			}
			else if (distance_moved > MOVE_THRESHOLD && distance_moved < MOVE_THRESHOLD_HB) {
				if (index_set == false) {
					*prev_index = hand->index.metacarpal.next_joint;
					*prev_palm = hand->palm.position;
					index_set = true;
				}
				setUR3Values(hand);//, &ur3. map the hand readings to robot pose
				writeFile(ur3->waist, ur3->shoulder, ur3->elbow, ur3->wrist1, ur3->wrist2, ur3->wrist3, ur3->grip);//write the readings to a file which will be read by python socket
				//update the previous palm position
				prevx = hand->palm.position.x;
				prevy = hand->palm.position.y;
				prevz = hand->palm.position.z;
				//update prev_index and palm
				*prev_index = hand->index.metacarpal.next_joint;
				*prev_palm = hand->palm.position;
				return;
			}

			printf("%s hand with ur3 setup (%f, %f, %f, %f,%f).\n",
				(hand->type == eLeapHandType_Left ? "left" : "right"),
				ur3->waist,
				ur3->shoulder,
				ur3->elbow,
				ur3->wrist1,
				ur3->grip);
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


boolean check_gesture_complicated() {
	LEAP_TRACKING_EVENT *frame = GetFrame();  //get the current frame
	if (frame && (frame->tracking_frame_id > lastFrameID)) { //check if frame is valid
		lastFrameID = frame->tracking_frame_id; //update the old frame id
												//printf("Frame %lli with %i hands.\n", (long long int)frame->tracking_frame_id, frame->nHands);
		if (frame->nHands > 0) { // if frame is not empty
			LEAP_HAND* hand = &frame->pHands[0]; //get the first tracked hand
												 //printf("\nIndex is extended %d\n", hand->index.is_extended);
												 //if the gesture is a peace sign
			if (hand->palm.position.x > LM_MIN_X && hand->palm.position.x < LM_MAX_X && hand->palm.position.y < LM_MAX_Y && hand->palm.position.z < LM_MAX_Z && hand->palm.position.z > LM_MIN_Z) {
				//check for "one" sign -> 4G version
				if (hand->thumb.is_extended == 0 && hand->index.is_extended == 1 && hand->middle.is_extended == 0 && hand->ring.is_extended == 0 && hand->pinky.is_extended == 0) {

					if (prev_sign == SIGN_4G)sign_check++;
					else sign_check = 1;//reset to 0;
					prev_sign = SIGN_4G; //update prev_sign
					if (sign_check > 2) {
						delay = DELAY_4G;
						sign_check = 0; //reset after execution
						return true;
					}
				}
				//check for peace sign -> 5G version
				else if (hand->thumb.is_extended == 0 && hand->index.is_extended == 1 && hand->middle.is_extended == 1 && hand->ring.is_extended == 0 && hand->pinky.is_extended == 0) {
					if (prev_sign == SIGN_5G)sign_check++;
					else sign_check = 1;
					prev_sign = SIGN_5G;
					if (sign_check > 2) {
						delay = DELAY_5G;
						sign_check = 0;
						return true;
					}
				}
			}
		}
	}
	return false;
}

int main(int argc, char** argv) {
	onConnection();
	LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
	if (deviceProps)
		printf("Using device %s.\n", deviceProps->serial);
	init_pos();
	for (;;) {
		//trackHand();
		if (bactiveMove == false) {
			bactiveMove = check_gesture_complicated();
			if (bactiveMove)
				printf("\nrobot activated %d", bactiveMove);
		}
		else
			trackHandSimple();
		//socketCommunication();
	} //ctrl-c to exit

	return 0;
}