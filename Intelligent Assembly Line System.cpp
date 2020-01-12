
#include "mindsensors-motormux.h"

//array that simulates board
char tiles [6][8];

//array which contains the x coordinates for all the points for all the pieces
int blockXCoord [4][16] =

{

	//L
	{0, 1, 0, 0, //R0
	0, 1, 0 ,0, // R1
	0, -1, 0, 0, //R2
	0, -1, 0, 0}, //R3

	//I
	{0, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,
	0, 1, 0, 0},

	//dot
	{0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0},

	//square
	{0, 1, 0, 1,
	0, 1, 0, 1,
	0, 1, 0, 1,
	0, 1, 0, 1 }

	};

//array that contains the y coordinates for all the points of all the pieces
int blockYCoord [4][16] =
{

	//L
	{0, 0, -1, 0,
	0, 0, 1, 0,
	0, 0, 1, 1,
	0, 0, -1, 0,},

	//I
	{0, 1, 0, 0,
	0, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 0 },

	//dot
	{0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0},

	//square
	{0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1}
};

//function which moves to conveyor belt and reads in the type of piece
//this function works sometimes, but was not consistent enough to be used in our main
void moveConveyor(tSensors S1, tSensors S2, int & currPiece, int & currRot, float & xOffset, float & yOffset)
{

	//setmotorpower
	MSMMotor(mmotor_S3_2, 25);

	//timer for left/right side of piece
	float leftColorTimer = 0;
	float rightColorTimer = 0;

	//normalized timers as ratios
	int normalizedLeftColorTimer = 0;
	int normalizedRightColorTimer = 0;

	//time it takes for one piece to pass conveyor belt
	const float ONE_PIECE = .4;

	//both equal 1 if the piece is read in at the same time for both left and right side
	int leftTimeStart = 0;
	int rightTimeStart = 0;

	//if the sensor has passed by the piece on its side
	bool leftTimeFinished = false;
	bool rightTimeFinished = false;

	//if the piece started with a hole, and then comes, the boolean turns true
	bool leftDelayedStart = false;
	bool rightDelayedStart = false;


	//for testing, if color sensor is reading conveyor belt
	while ((SensorValue[S1] != (int)colorRed && SensorValue[S2] != (int)colorRed)) {

		displayBigTextLine(5, "GREEN");

	}

	//if the color sensor reads in a piece on the left side start a timer, and if right side as well, also start timer
	if (SensorValue[S1] == (int)colorRed) {
		time1[T1] = 0;
		leftTimeStart = 1;
		wait1Msec(400);
		if (SensorValue[S2] == (int)colorRed) {
			time1[T2] = 0;
			rightTimeStart = 1;
		}
	}

	//if the color sensor reads in a piece on the right side start a timer, and if left side as well, also start timer
	if (SensorValue[S2] == (int)colorRed) {
		time1[T2] = 0;
		rightTimeStart = 1;

		wait1Msec(400);

		if (SensorValue[S1] == (int)colorRed) {
			time1[T1] = 0;
			leftTimeStart= 1;
		}
	}


	//white either of them are on the block
	while (SensorValue[S1] == (int)colorRed || SensorValue[S2] == (int)colorRed) {
		displayBigTextLine(8, "PIECe");

		//if the left one hasn't already started, and the piece comes, turn on the timer
		if (leftTimeStart == 0) {
			if (SensorValue[S1] == (int)colorRed && leftDelayedStart == false) {
				time1[T1] = 0;
				leftDelayedStart = true;
				displayBigTextLine(12, "left delayed");

			}
		}


		//if the right one hasn't already started, and the piece comes, turn on the timer
		if (rightTimeStart == 0) {
			if (SensorValue[S2] == (int)colorRed && rightDelayedStart == false) {
				time1[T2] = 0;
				rightDelayedStart = true;
				displayBigTextLine(12, "right delayed");

			}
		}

		//if the left side of the piece has passed the piece, store its time data
		if ((SensorValue[S1] != (int) colorRed && leftTimeFinished == false) &&
			((leftTimeStart == 0 && leftDelayedStart == true) || leftTimeStart == 1)) {

				leftColorTimer = time1[T1];
				leftTimeFinished = true;

		}

		//if the right side of the piece has passed the piece, store its time data
		if ((SensorValue[S2] != (int) colorRed && rightTimeFinished == false) &&
			((rightTimeStart == 0 && rightDelayedStart == true) || rightTimeStart == 1)) {
			rightColorTimer = time1[T2];
			rightTimeFinished = true;
		}

	}

	//if the piece hasn't come yet on that side, reset timer
	if (leftTimeStart == 0 && leftDelayedStart == false) {
		leftColorTimer = 0;
	}
	if (rightTimeStart == 0 && rightDelayedStart == false) {
		rightColorTimer = 0;
	}


	//normalize pieces into ratios
	if (leftColorTimer > rightColorTimer) {
		if (rightColorTimer != 0){
			normalizedRightColorTimer = 1;
			normalizedLeftColorTimer = round(leftColorTimer / rightColorTimer);
		} else {
			normalizedRightColorTimer = 0;
			normalizedLeftColorTimer = 2;
		}
	}

	if (leftColorTimer < rightColorTimer) {
		if (leftColorTimer !=0){
			normalizedLeftColorTimer = 1;
			normalizedRightColorTimer = round(rightColorTimer / leftColorTimer);
		} else {
			normalizedLeftColorTimer = 0;
			normalizedRightColorTimer = 2;
		}
	}


	int rightLength = round((rightColorTimer/1000)/ONE_PIECE);

	//if both sides are equal in length
	if ((normalizedLeftColorTimer == normalizedRightColorTimer) && normalizedLeftColorTimer != 0 && normalizedRightColorTimer != 0) {

	//if piece is vertical on the right side
		if (rightLength == 1) {
			displayBigTextLine(14, "line vert");//right
			currPiece = 1;
			currRot = 0;
			xOffset = 0;
			yOffset = 0;

			//if the piece is square
		} else if (rightLength == 2) {
			displayBigTextLine(14, "square");
			currPiece = 3;
			currRot = 0
			xOffset = 0;
			yOffset = 0;
			}
	}

	//if the right side is longer than the left side
	else if (normalizedRightColorTimer > normalizedLeftColorTimer) {
			displayBigTextLine(14, "%d", rightLength);


		if (leftColorTimer == 0){

		//if a dot on the right side
			if (rightLength == 1) {
				displayBigTextLine(14, "dot");
				currPiece = 2;
				currRot = 0;
				xOffset = 2.3;
				yOffset = -1.15;

				//if a horizontal line
			} else if (rightLength == 2) {
				displayBigTextLine(14, "line horiz");
				currPiece = 1;
				currRot = 1;
				xOffset = 3.45;
				yOffset = -1.15;


			}
		}

		//if both sides start at the same time, and it is L shaped in rotation 3
		else if (leftTimeStart == 1 && rightTimeStart == 1) {

			displayBigTextLine(14, "L 3"); //3
			currPiece = 0;
			currRot = 3;
			xOffset = 0;
			yOffset = 0;

			//if the piece is L shaped in rotation 0
		} else {
			displayBigTextLine(14, "L 0");
			currPiece = 0;
			currRot = 0;
			xOffset = 0;
			yOffset = 0;
		}
	}

	else {

		int leftLength = round((leftColorTimer/1000)/ONE_PIECE);

		if(rightColorTimer == 0) {

		//if the piece is a dot
			if (leftLength == 1){
				currPiece = 2;
				currRot = 0;
				xOffset = 2.3;
				yOffset = 1.15;

				//if the peice is a horizontal line
			} else if (leftLength == 2 ) {
				displayBigTextLine(14, "line horiz");
				currPiece = 1;
				currRot = 1;
				xOffset = 3.45;
				yOffset = -1.15;

			}
		}

		//if the piece is an L in rotation 2
		else if (leftTimeStart == 1 && rightTimeStart ==1) {
			displayBigTextLine(14, "L 2");//0
			currPiece = 0;
			currRot = 2;
			xOffset = 0;
			yOffset = 0;

		//if the piece is an L in rotation 1
		}
		else {
			currPiece = 0;
			currRot = 1;
			xOffset = 0;
			yOffset = 0;
		}

	}

	//run the conveyor belt until it is at the pickup location
	MSMMotorEncoderReset(mmotor_S3_2);
	while (MSMMotorEncoder(mmotor_S3_2) < 150) {
	}

	//stop motor
	MSMotorStop(mmotor_S3_2);
	wait1Msec(3000);

}




//function to find most efficient location
void findEfficientLocation(int & placeX, int & placeY, int currPiece, int currRot)
{


	//constants for board
	const char FILLEDSPACE = 'O';
	const char TESTSPACE = 'X';
	const char EMPTYSPACE = '*';

	//points added for each adjascent side
	const int ADJASCENTMULTIPLIER = 10;

	//highest score for move
	int highestTotal = 0;

	//array for points for each possible move
	int testPoints [6][8];

	//constant arrays to cycle through piece without redundancy
	const int BLOCKSINPIECE [4] = {3, 2, 1, 4};

	//element in 4th dimension in board array for the X and Y point
	const int XPOINT = 0;
	const int YPOINT = 1;

	for (int fillX = 0; fillX < 6; fillX++) {

		for (int fillY = 0; fillY < 8; fillY++) {

			//reset points array
			testPoints[fillX][fillY] = 0;

		}
	}

	placeX = 1;
	placeY = 1;

	//cycle through all X possiblities
	for (int cycleX = 1; cycleX < 5; cycleX++) {

		//cycle through all the Y possibilities
		for (int cycleY = 1; cycleY < 7; cycleY++) {

			//cycle through all components of the piece
			for (int cyclePoints = 0; cyclePoints < BLOCKSINPIECE[currPiece]; cyclePoints++) {

				//check adjascent above
				if (cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints] - 1 >= 0){
					if ((tiles[cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints]] [cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints] - 1] == FILLEDSPACE) || (cycleY + blockYCoord[currPiece][currRot * 4 + cyclePoints] - 1 == 0 ))  {
						testPoints[cycleX][cycleY] += ADJASCENTMULTIPLIER;
					}
				}

				//check adjascent below
				if (cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints] + 1 < 8) {
					if ((tiles[cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints]] [cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints] + 1] == FILLEDSPACE) || (cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints] + 1 == 7 ))  {
						testPoints[cycleX][cycleY] += ADJASCENTMULTIPLIER;
					}
				}

				//check adjascent left
				if (cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints] - 1 >= 0){
					if ((tiles[cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints] - 1] [cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints]] == FILLEDSPACE) || (cycleX + blockYCoord[currPiece][4 * currRot + cyclePoints] - 1 == 0 ))  {
						testPoints[cycleX][cycleY] += ADJASCENTMULTIPLIER;
					}
				}

					//check adjascent right
				if (cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints] + 1 < 6){
					if ((tiles[cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints] + 1] [cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints]] == FILLEDSPACE) || (cycleX + blockYCoord[currPiece][4 * currRot + cyclePoints] + 1 == 5 ))  {
						testPoints[cycleX][cycleY] += ADJASCENTMULTIPLIER;
					}
				}

				//check on top
				if (tiles[cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints]] [cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints]] == FILLEDSPACE)  {
					testPoints[cycleX][cycleY] -= 1000;
				}

				//check if out of bounds
				if ((cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints] == 5 ) || (cycleX + blockXCoord[currPiece][4 * currRot + cyclePoints] == 0 ) || (cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints] == 7 ) || (cycleY + blockYCoord[currPiece][4 * currRot + cyclePoints] == 0 ))  {
						testPoints[cycleX][cycleY] -= 1000;
				}
			}
		}
	}

	//cycle through all X possibilities
	for (int cycleX = 0; cycleX < 5; cycleX++) {

		//cycle through all Y possibilities
		for (int cycleY = 0; cycleY < 7; cycleY++) {

			//if it is the highest point total
			if (testPoints[cycleX][cycleY] > highestTotal) {

				//save the best move
				placeX = cycleX;
				placeY = cycleY;
				highestTotal = testPoints[cycleX][cycleY];

			}
		}
	}

	int leftestPoint = placeX;
	int lowestPoint = placeY;

	//find the lowest point and the most left point of any piece
	for (int cyclePoints = 0; cyclePoints < BLOCKSINPIECE[currPiece]; cyclePoints++) {

		tiles[placeX + blockXCoord[currPiece][4 * currRot + cyclePoints]] [placeY + blockYCoord[currPiece][4 * currRot + cyclePoints]] = FILLEDSPACE;

		if (placeX + blockXCoord[currPiece][4 * currRot + cyclePoints] < leftestPoint) {

			leftestPoint = placeX + blockXCoord[currPiece][4 * currRot + cyclePoints];
		}

		if (placeY + blockYCoord[currPiece][4 * currRot + cyclePoints] > lowestPoint) {

			lowestPoint = placeY + blockYCoord[currPiece][4 * currRot + cyclePoints];

		}

	}

	//convert X and Y to the x and y of the plotter
	placeX = leftestPoint - 1;
	placeY = 6 - lowestPoint;

}

//function to open claw
void openClaw () {

	//reset motor encoder
	nMotorEncoder[motorD] = 0;

	//close claw to ensure it is in its fully closed position, so when it is opened, it is opened predictably
	motor[motorD] = -100;
	while (nMotorEncoder[motorD] > -600) {
	}
	motor[motorD] = 0;

	//open claw until it is wide enough to accept any piece
	motor[motorC]= 0;
	nMotorEncoder[motorD] = 0;
	motor[motorD] = 100;
	while (nMotorEncoder[motorD] < 800) {
			displayBigTextLine(5, "%d", getMotorEncoder(motorD));

	}
	motor[motorD] = 0;

}



bool returnStartingPosition (int xSpeed, int ySpeed) {

	//open the claw
	openClaw();

	//move to the original y location
	motor[motorC] = -ySpeed;

	//until the touch sensor is pressed
	while (SensorValue[S4] == 0) {
	}

	motor[motorC] = 0;
	nMotorEncoder[motorC] = 0;


	//return to start X
	motor[motorA] = motor[motorB] = -xSpeed;

	//until the motor encoder is 0, which is at the start, while overshooting to ensure it is at the edge
	while (nMotorEncoder[motorB] > -100) {

	}
	motor[motorA] = motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;

	//return true to make sure it has returned to start
	return true;

}

//move to location
void moveToLocation (int currPiece, int currRot, int xSpeed, int ySpeed, int MOVEX1CM, int MOVEY1CM, int xCoord, int yCoord) {

	int xOffset = 0;
	int yOffset = 0;

	//offset how much the plotter needs to move depending on the piece
	if (currPiece == 2) {
		xOffset = 2.3;
		yOffset = -1.15;
	} else if (currPiece == 1 && (currRot == 1 || currRot == 3)) {
		xOffset = 3.45;
		yOffset = -1.15;
	}

	nMotorEncoder[motorB] = 0;

	//move the x plotter until it has reached the desired x location
	motor[motorA] = motor[motorB] = xSpeed;

	while (nMotorEncoder[motorB] < (xOffset + 5.4 + (2.3 * xCoord)) * MOVEX1CM) {
	}
	motor[motorA] = motor[motorB] = 0;


	//move the y plotter until it has reached the desired y location
	motor[motorC] = ySpeed;
	while (nMotorEncoder[motorC] < (yOffset + 2 + (yCoord * 2.3)) * MOVEY1CM) {
	}


	motor[motorC]= 0;

	//tighten the claw to ensure it has a good grip
	nMotorEncoder[motorD] = 0;
	motor[motorD] = 100;
	while (nMotorEncoder[motorD] < 400) {
			displayBigTextLine(5, "%d", getMotorEncoder(motorD));

	}
	motor[motorD] = 0;

	wait1Msec(3000);

}

//function to drop the piece
void dropPiece () {

	wait1Msec(5000);
	motor[motorC]= 0;
	nMotorEncoder[motorD] = 0;

	//let go of the piece until the claw is wide enough that the piece drops down
	motor[motorD] = -100;
	while (nMotorEncoder[motorD] > -1500) {
			displayBigTextLine(5, "%d", getMotorEncoder(motorD));
	}
	motor[motorD] = 0;

	wait1Msec(1000);

}


//main
task main()
{


	//configure sensors
	SensorType[S3]=sensorI2CCustom;
	MSMMUXinit();
	SensorType[S4] = sensorEV3_Touch;
	SensorType[S1] = sensorEV3_Color;
	SensorType[S2] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	SensorMode[S2] = modeEV3Color_Color;
	wait1Msec(50);

	//floats for the offset of the xy plotter depending on the piece
	float xOffset = 0;
	float yOffset = 0;

	//arrays which show the order of the pieces coming in
	int pieceShapeOrder [11] = {1, 0, 0, 1, 0, 1, 0, 1, 1, 2, 2};
	int pieceRotOrder [11] =   {0, 0, 1, 0, 2, 0, 3, 0, 0, 0, 0};

	//x and y of the best possible move
	int placeX = 3;
	int placeY = 3;

	//current shape and orientation of the piece
	int currPiece = 0;
	int currRot = 1;

	int xCoord = 2;
	int yCoord = 2;

	//speed for the plotter
	int xSpeed = 30;
	int ySpeed = 30;

	//conversion values for encoder to cm
	const float MOVEX1CM = 50;
	const float MOVEY1CM = 52;


		//cycle through file
	for (int fillX = 0; fillX < 6; fillX++) {

		for (int fillY = 0; fillY < 8; fillY++) {

			tiles[fillX][fillY] = '*';
		}
	}

	//cycle through the incoming pieces
	for (int amountPieces = 0; amountPieces < 10; amountPieces++){

		currPiece = pieceShapeOrder[amountPieces];
		currRot = pieceRotOrder[amountPieces];

		nMotorEncoder[motorB] = 0;


		//return to start
		if (returnStartingPosition(xSpeed,ySpeed)){

			//find most efficient location
			findEfficientLocation(xCoord, yCoord, currPiece, currRot);

			//drop piece
			dropPiece();

			//move to location
			moveToLocation (currPiece, currRot, xSpeed,ySpeed, MOVEX1CM, MOVEY1CM, xCoord, yCoord);
		}

		//return to start
		returnStartingPosition(xSpeed,ySpeed);

	}

	wait1Msec(5000);

}
