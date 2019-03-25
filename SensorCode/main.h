#ifndef UR3LMSetup
#define UR3LMSetup

//#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <WS2tcpip.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "LeapC.h"
#include "ExampleConnection.h"
#include <math.h>

/*UR3 Variables*/
#define PORT 30000
#define WAIST_MAX 20.41//30
#define WAIST_MIN -20
#define SHOULDER_MAX -60.5//-60.5 //-80
#define SHOULDER_MIN -143//-130//-135
#define ELBOW_MAX -30 //-40
#define ELBOW_MIN -104//-97//-105
#define WRIST2_BASE_POS 85
#define WRIST1_UP -65
#define WRIST1_DOWN -103//-115
#define WRIST2_LEFT 120
#define WRIST2_RIGHT 60
#define WRIST3_RIGHT 310
#define WRIST3_LEFT 230


/*Leap Motion Max Min System*/
#define LM_MAX_X 140
#define LM_MIN_X -140
#define LM_MAX_Y 400
#define LM_MIN_Y 250
#define LM_MAX_Z 150//170
#define LM_MIN_Z 0//10
#define LM_PITCH_UP 50 //was 50
#define LM_PITCH_DOWN -80 // was -50
#define LM_YAW_LEFT -48
#define LM_YAW_RIGHT 32
#define LM_ROLL_RIGHT -0.5
#define LM_ROLL_LEFT 0.5


#define M_PI 3.14159265358979323846
#define MOVE_THRESHOLD 5
#define ERROR_RANGE 10.0
#define W3_ROTATION_1 278.67
#define W3_ROTATION_2 181.86
#define W1_MAX -3.0
#define W1_MIN -126.0
#define W1_MAX -8.0
#define W1_MIN -96
#define SCALE_Z 2.0
#define SCALE_Y 1.0
#define ARM1 30.0 //in cm, the arm closer to the base
#define ARM2 25.0 //in cm, the arm closer to the end effector
#define UR3_MAX_Z 60.0
#define UR3_MIN_Z 10.0
#define DELAY_4G 1.0
#define DELAY_5G 0.7
#define MOVE_THRESHOLD_HB 400
#define SIGN_4G 1
#define SIGN_5G 2

int64_t lastFrameID = 0; //The last frame received
boolean is_extended = true;
static float delay = 0;
int prev_sign = 0;
int sign_check = 0;

/*This is to interpret the coordinate movement of the UR3*/
typedef struct{
	float waist; //waist is for the sideways movement (X axis of the Leap motion)
	float shoulder; //shoulder, elbow and wrist1 coordinate together
	float elbow;    // to provide motion in Y and Z direction (in Leap Motion Up and Forward)
	float wrist1; // Wrist1 also provides the pitch movement or the human wrist going up and down
	float wrist2; // Wrist2 This is for yaw movement of wrist i.e. wrist moving sideways
	float wrist3; // Wrist3 is for rolling of the wrist.
	bool grip; //Grip True implies to close the gripper, False is to open
}UR3data;


//void init(UR3data*u);
static boolean bactiveMove = false;
static int handCheck = 0; //This is to check nuber of frames without a hand
#define HAND_RESET 100// if without hand for more than 5 frames than reset
static float prevx= 0;
static float prevy = 0;
static float prevz = 0;
static UR3data* ur3 = (UR3data*)malloc(sizeof(UR3data)); //prevent garbage values
float prev_shoulder = -87.6;
float prev_elbow = -91.5;
LEAP_VECTOR* prev_index = (LEAP_VECTOR*)malloc(sizeof(LEAP_VECTOR));
LEAP_VECTOR* prev_palm = (LEAP_VECTOR*)malloc(sizeof(LEAP_VECTOR));
boolean index_set = false;



int32_t handID = -1;

boolean chkGrip(LEAP_HAND* hand);
void setUR3Values(LEAP_HAND* hand);
int getWaist(float HandX);
int getShoulder(float HandY);
int getElbow(LEAP_HAND* hand);
int getWrist1(int elbow, int shoulder, LEAP_HAND* hand);
int getWrist2(LEAP_HAND* hand);
int getWrist3(LEAP_HAND* hand);
float degToRad(int val);
float distanceMove(LEAP_HAND* hand);
float dist(LEAP_VECTOR a, LEAP_VECTOR b);
float setWrist3(LEAP_HAND* hand);
void drop();

#endif