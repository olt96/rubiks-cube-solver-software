﻿
//~15 cm 


/**

*The Kociemba implementation code is from Jay Flatland 
*https://www.youtube.com/watch?v=ixTddQQ2Hs4&t=10s
* 
* The names of the facelet positions of the cube
*             |************|
*             |*U1**U2**U3*|
*             |************|
*             |*U4**U5**U6*|
*             |************|
*             |*U7**U8**U9*|
*             |************|
* ************|************|************|************|
* *L1**L2**L3*|*F1**F2**F3*|*R1**R2**R3*|*B1**B2**B3*|
* ************|************|************|************|
* *L4**L5**L6*|*F4**F5**F6*|*R4**R5**R6*|*B4**B5**B6*|
* ************|************|************|************|
* *L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*|
* ************|************|************|************|
*             |************|
*             |*D1**D2**D3*|
*             |************|
*             |*D4**D5**D6*|
*             |************|
*             |*D7**D8**D9*|
*             |************|
* </pre>
*
*A cube definition string "UBL..." means for example: In position U1 we have the U-color, in position U2 we have the
* B-color, in position U3 we have the L color etc. according to the order U1, U2, U3, U4, U5, U6, U7, U8, U9, R1, R2,
* R3, R4, R5, R6, R7, R8, R9, F1, F2, F3, F4, F5, F6, F7, F8, F9, D1, D2, D3, D4, D5, D6, D7, D8, D9, L1, L2, L3, L4,
* L5, L6, L7, L8, L9, B1, B2, B3, B4, B5, B6, B7, B8, B9 of the enum constants.
*/
//order up right front down left back 

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/video/tracking.hpp"
#include "search.h"
#include "tools.h"
#include <string>
#include "SerialPort.h"
using namespace cv;
using namespace std;

std::string convertToSolverNotation(int *stickers, int *centers);
int mainColor(Mat &mat);
void ShowManyImages(string title, int nArgs, ...);
std::string toSolverNotation(bool showProcess , SerialPort arduino);
Scalar getColor(int clr);
void drawCube(Mat &img, int stickers[54]);
std::vector <unsigned char>  convertToShortMoves(string moves);
string convertToArduinoMovesSimple (string inMoves);

int countMoves(std::string moves);
//minimum and maximum hsv values for each color  


int H_MIN[6] = { 165,
39,
79,
26,
0,
77
};
int H_MAX[6] = { 189,
88,
131,
46,
26,
124
};

int S_MIN[6] = { 177,
126,
128,
168,
216,
0
};
int S_MAX[6] = { 235,
212,
193,
256,
255,
56
};

int V_MIN[6] = { 100,
186,
189,
223,
210,
184
};
int V_MAX[6] = { 207,
251,
255,
255,
255,
255
};

/*int const H_MIN_D[6] = { 165 , 35 , 75 , 28 , 7 , 116 };
int const H_MAX_D[6] = { 184 , 63 , 100,67 , 19 , 147 };

int const S_MIN_D[6] = { 161 , 230 , 139 , 47 , 209 , 23 };
int const S_MAX_D[6] = { 202 , 255 , 161,121 ,256 , 119 };

int const V_MIN_D[6] = { 140 , 207, 123, 140,238,196 };
int const V_MAX_D[6] = { 214 ,255 , 255,255,255 ,255};
*/




enum col {

	red = 0 , green = 1 , blue = 2 , yellow = 3 , orange = 4 , white = 5

};

//
/*up , front,  right  ,back ,left  ,down , 
stickers is an array of 54 integers from 0 - 5 , 0 is red , white is 5 etc . 
sideColor is an array of the center piece colors of the 6 sides .
in the stickers array 
the first 9 spots are occupied by the stickers on the top face (U) 
the next 9 by the Front face 
the next by the Right face
then the back , left , and down face .
this is not the same as the kociemba implementation , which uses the order UP , RIGHT , FRONT , DOWN , LEFT , BACK 
this is why in the convertToSolverNotation function , we have to use the order[] array , and the 3 for cycles to get the correct order
I could change the order of the stickers array , but I won't bother 
*/

const int CUBE_FACES = 6;
const int NUM_OF_STICKERS = 54 ;

int stickers[NUM_OF_STICKERS];
int sideColor[CUBE_FACES];




string convertToArduinoMoves(string inMoves);
int cn = 0 ;
int main(int argc, const char** argv)
{
	char port_name[] = "\\\\.\\COM5";
	//cout << port_name; 
	char k; 
	Kociemba::TwoPhase::Search search;
	int a;
	char c;
	while (true) {
		cout << "1:Settings \n2:cont \n";
		cin >> a;
		if (a == 2) break;
		cout << "enter comPort \n";
		cin >> port_name[7];
		cout << "enter camera number (default 0)";
		cin>> cn;

	}

	SerialPort arduino(port_name);
	
	while (true) {
		for (auto i = 0; i < NUM_OF_STICKERS ; i++) {
			stickers[i] = 0;
		}
	std::string boi = toSolverNotation(true , arduino);
	//				UP, RIGHT, FRONT, DOWN, LEFT, BACK
	// ctor will calc pruning tables, may take a full second
	
	
	std::string  sol;

	try {
		sol = search.solution(boi, 21, 20000, false);
	}
	catch (std::runtime_error err) {
		cout <<err.what();
		continue;
	}

	cout << sol << endl; 
	
	string out = convertToArduinoMovesSimple(sol);
	cout << out << " " << countMoves(out) << endl;
	out = convertToArduinoMoves(sol);
	out.append("\n");
	cout << out << " " << countMoves(out) << endl;
	const char *c_string = out.c_str();


	//Writing string to arduino
	arduino.writeSerialPort(c_string, out.length());
	Sleep(10000);
	

	
	}

	


}




//******************************************************************************************
//These functions convert the solution string into a motor move string  

int countMoves(std::string moves) {
	bool fp = true;
	int ctr = 0; 
	for (std::string::iterator it = moves.begin(); it != moves.end(); ++it) {
		if (*it == '.') ctr++;
		
	}
	return ctr/2;
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

enum motorStates {
	neutral =1, min =0 , max = 2

};
motorStates& operator++(motorStates& orig)
{

	orig = static_cast<motorStates>(orig + 1); 					
	return orig;
}

motorStates& operator--(motorStates& orig)
{
	orig = static_cast<motorStates>(orig -1);
	return orig;
}

enum motors {

	front , right , back , left 

};

motors onLeft (motors currMotor) {

	if (currMotor == motors::front) return motors::left; 
	return (static_cast <motors>(currMotor - 1));

}
motors onRight(motors currMotor) {

	if (currMotor == motors::left) return motors::front;
	return (static_cast <motors>(currMotor + 1));

}
motors operator! (motors currMotor) {
	
	if (currMotor == motors::left) return motors::right; 
	if (currMotor == motors::back) return motors::front;
	return (static_cast <motors>(currMotor +2));
}
void regularMove(motors movingMotor, motorStates *motorState, bool *blockedMotors, char *sideState, bool prime, string &effectString) {

 	static string sideMoves[] = { ".S." , ".K." , ".S." , ".K." };

	std::string moves[4][2]{ {"F" ,"f"} ,{"R","r"} , {"B","b"} ,{"L","l"} };

	if (blockedMotors[movingMotor] == true) {

		effectString.append(sideMoves[movingMotor]);
		
		if (motorState[onRight(movingMotor)] == motorStates::min) { 
		
			effectString.append(moves[onRight(movingMotor)][0]);  
			++motorState[onRight(movingMotor)]; 
		
		}else if (motorState[onRight(movingMotor)] == motorStates::max) {
			
			effectString.append(moves[onRight(movingMotor)][1]); 
			--motorState[onRight(movingMotor)];
		
		}

		if (motorState[onLeft(movingMotor)] == motorStates::min) {
		
			effectString.append(moves[onLeft(movingMotor)][0]);
			++motorState[onLeft(movingMotor)];
		
		}else if (motorState[onLeft(movingMotor)] == motorStates::max) {
		
		effectString.append(moves[onLeft(movingMotor)][1]); 
		--motorState[onLeft(movingMotor)];
		}

		effectString.append(sideMoves[movingMotor]);

		blockedMotors[movingMotor] = false;
		blockedMotors[!movingMotor] = false;
	}
	
	if (prime == false && (motorState[movingMotor] == motorStates::max)) {
		if (motorState[movingMotor] == motorStates::max) {
			effectString.append(sideMoves[onRight(movingMotor)] + moves[movingMotor][1] + sideMoves[onRight(movingMotor)]);
			--motorState[movingMotor];

		}

	}
	else {
		if (motorState[movingMotor] == motorStates::min) {
			effectString.append(sideMoves[onRight(movingMotor)] + moves[movingMotor][0] + sideMoves[onRight(movingMotor)]);
			++motorState[movingMotor];

		}

	}
	if (prime) 
	{ effectString.append(moves[movingMotor][1]); --motorState[movingMotor]; }
	else 
	{ effectString.append(moves[movingMotor][0]); ++motorState[movingMotor];  }
	


	if (motorState[movingMotor] != motorStates::neutral) {
		blockedMotors[onRight(movingMotor)] = true;
		blockedMotors[onLeft(movingMotor)] = true;
	}



}


void rotateCube(char movingSide, motorStates *motorState, bool *blockedMotors, char *sideState, string &effectString) {
	
	bool moveBack= false ; 
	if (movingSide == 'F' || movingSide == 'B') {
		moveBack = true; 
	}
	
	bool rightReset = false;
	bool leftReset = false;
	if (!moveBack) {
		if (motorState[motors::right] == motorStates::min) rightReset = true;
		if (motorState[motors::left]  == motorStates::max) leftReset  = true;

		if (rightReset || leftReset) {
			effectString.append(".S.");
			if (leftReset)  { effectString.append("l"); --motorState[motors::left]; }
			if (rightReset) { effectString.append("R"); ++motorState[motors::right];}
			effectString.append(".S");
		}

		effectString.append(".K.Lr");
		//**********************
		if (motorState[motors::front] == motorStates::min) {

			effectString.append("F");
			++motorState[motors::front];

		}
		else if (motorState[motors::front] == motorStates::max) {

			effectString.append("f");
			--motorState[motors::front];

		}

		if (motorState[motors::back] == motorStates::min) {

			effectString.append("B");
			++motorState[motors::back];

		}
		else if (motorState[motors::back] == motorStates::max) {

			effectString.append("b");
			--motorState[motors::back];
		}
		effectString.append(".K.");
		//**********************



		sideState[motors::front] = 'U';
		sideState[motors::back]  = 'D';
		++motorState[motors::left];
		--motorState[motors::right];
		if (motorState[motors::left] != motorStates::neutral || motorState[motors::right] != motorStates::neutral) {
			blockedMotors[motors::front] = true;
			blockedMotors[motors::back] = true;

		}
	}

	else {
		if (motorState[motors::right] == motorStates::max) rightReset = true;
		if (motorState[motors::left]  == motorStates::min) leftReset  = true;

		if (rightReset || leftReset) {
			effectString.append(".S.");
			if (leftReset)  { effectString.append("L"); ++motorState[motors::left]; }
			if (rightReset) { effectString.append("r"); --motorState[motors::right];}
			effectString.append(".S");
		}

		effectString.append(".K.lR");
		if (motorState[motors::front] == motorStates::min) {

			effectString.append("F");
			++motorState[motors::front];

		}
		else if (motorState[motors::front] == motorStates::max) {

			effectString.append("f");
			--motorState[motors::front];

		}

		if (motorState[motors::back] == motorStates::min) {

			effectString.append("B");
			++motorState[motors::back];

		}
		else if (motorState[motors::back] == motorStates::max) {

			effectString.append("b");
			--motorState[motors::back];
		}
		effectString.append(".K.");
		//**********************

		sideState[motors::front] = 'F';
		sideState[motors::back]  = 'B';
		--motorState[motors::left];
		++motorState[motors::right];
		if (motorState[motors::left] != motorStates::neutral || motorState[motors::right] != motorStates::neutral) {
			blockedMotors[motors::front] = true;
			blockedMotors[motors::back]  = true;

		}
	
	}


}


string convertToArduinoMoves(string inMoves) {
	//									F	      R        B         L	
	motorStates motorState[4] = { neutral , neutral, neutral , neutral };
	bool blockedMotors[4] = { false   ,  false ,   false ,   false };
	char sideState[4] = { 'F' ,'R' ,'B'  ,'L' };

	string resetMoves[] = { "" };

	std::vector <std::string> splitMoves = split(inMoves, ' ');
	string effectString = "";


	bool backOne = false; 
	for (vector <string >::iterator it = splitMoves.begin(); it != splitMoves.end(); ++it) {
		cout << *it << " "; 
		bool prime = false, dM = false;
		if (backOne)  --it;

		if (it->length() == 2) {
			if (it->at(1) == '2') {
				dM = true;
			}
			else {
				prime = true;
			}
		}
		switch (it->at(0)) {
		

		case 'U':
			if (sideState[motors::front] == 'U') {

				regularMove(motors::front, motorState, blockedMotors, sideState, prime, effectString);
				if (dM) regularMove(motors::front, motorState, blockedMotors, sideState, prime, effectString);
				backOne = false;
			}
			else {
				rotateCube(it->at(0), motorState, blockedMotors, sideState, effectString);
				if (it + 1 != splitMoves.end())
					backOne = true;
				else --it; 
				continue;

			}

			break;
		case 'F':
			if (sideState[motors::front] == 'F') {

				regularMove(motors::front, motorState, blockedMotors, sideState, prime, effectString);
				if (dM) regularMove(motors::front, motorState, blockedMotors, sideState, prime, effectString);
				backOne = false;
			}
			else {
				rotateCube(it->at(0), motorState, blockedMotors, sideState, effectString);
				if (it + 1 != splitMoves.end())
					backOne = true;
				else --it;
				continue;

			}
			break;
		case 'R':



			regularMove(motors::right, motorState, blockedMotors, sideState, prime, effectString);
			if (dM) regularMove(motors::right, motorState, blockedMotors, sideState, prime, effectString);
			backOne = false;
			break;
		case 'L':

			regularMove(motors::left, motorState, blockedMotors, sideState, prime, effectString);
			if (dM) regularMove(motors::left, motorState, blockedMotors, sideState, prime, effectString);
			backOne = false;
			break;
		case 'D':
			//----------------

			if (sideState[motors::back] == 'D') {

				regularMove(motors::back, motorState, blockedMotors, sideState, prime, effectString);
				if (dM) regularMove(motors::back, motorState, blockedMotors, sideState, prime, effectString);
				backOne = false;
			}
			else {
				rotateCube(it->at(0), motorState, blockedMotors, sideState, effectString);
				if (it + 1 != splitMoves.end())
					backOne = true;
				else --it;
				continue;

			}

			break;
		case 'B':
			if (sideState[motors::back] == 'B') {

				regularMove(motors::back, motorState, blockedMotors, sideState, prime, effectString);
				if (dM) regularMove(motors::back, motorState, blockedMotors, sideState, prime, effectString);
				backOne = false;
			}
			else {
				rotateCube(it->at(0), motorState, blockedMotors, sideState, effectString);
				if (it + 1 != splitMoves.end())
					backOne = true;
				else --it;
				continue;

			}
		}








	}
	//scrub the return strings of extra pauses 
	for (string::iterator it = effectString.begin(); (it+1) != effectString.end(); ++it) {
		if ((*it == '.') && (*(it + 1) == '.'))
			effectString.erase(it);
	
	}
	return effectString; 
}




string convertToArduinoMovesSimple(string inMoves) {
								//   F	      R        B         L
	motorStates motorState[] = { neutral , neutral, neutral , neutral }; 
						//F    R 	B	  L
	char sideState[] = { 'F' ,'R' ,'B'  ,'L' };
								//F			/F'				F2
	const string rFront[] = { "F.K.f.k." , "f.K.F.k." , "K.f.k.FF.K.f.k."};
	const string rRight[] = { "R.S.r.s." , "r.S.R.s." ,"S.r.s.RR.S.r.s." };
	const string rLeft[] = { "L.S.l.s." , "l.S.L.s." , "S.l.s.LL.S.l.s." };
	const string rBack[] = { "B.K.b.k." , "b.K.B.k." ,"K.b.k.BB.K.b.k." };
									//turn U						turn U'						U2
	const string upMove[] = {"K.Lr.k.S.lR.s.F.K.lRf.k.S.Lr.s." ,"K.Lr.k.S.lR.s.f.K.lRF.k.S.Lr.s."  , "K.LrF.k.S.lR.s.ff.K.lRF.k.S.Lr.s." };
	const string downMove[] = {"K.Lr.k.S.lR.s.B.K.lRb.k.S.Lr.s." ,"K.Lr.k.S.lR.s.b.K.lRB.k.S.Lr.s."  ,"K.LrB.k.S.lR.s.bb.K.lRF.k.S.Lr.s." };
	
	std::vector <std::string> splitMoves = split(inMoves, ' ');
	string effectString = "";
	for (vector <string >::iterator it = splitMoves.begin(); it != splitMoves.end(); ++it) {
		switch (it->length()) {
			case  1:
				switch (it->at(0)) {
				case 'U':
					effectString.append(upMove[0]);
					break;
				case 'F':
					effectString.append(rFront[0]);
					break;
				case 'R':

					effectString.append(rRight[0]);
					break;
				case 'L':

					effectString.append(rLeft[0]);
					break;
				case 'D':
					effectString.append(downMove[0]);
					break;
				case 'B':
					effectString.append(rBack[0]);
					break;
				}
					break;
		
		case 2: 
			bool twoOrInv = true; 
			if (it->at(1) != '2')twoOrInv = false;
			switch (it->at(0)) {
			case 'U':

				if(twoOrInv)effectString.append(upMove[2]);
				else effectString.append(upMove[1]);
				break;
			case 'F':

				if (twoOrInv)effectString.append(rFront[2]);
				else effectString.append(rFront[1]);
				break;
			case 'R':

				if (twoOrInv)effectString.append(rRight[2]);
				else effectString.append(rRight[1]);
				break;
			case 'L':

				if (twoOrInv)effectString.append(rLeft[2]);
				else effectString.append(rLeft[1]);
				break;
			case 'D':
				if (twoOrInv)effectString.append(downMove[2]);
				else effectString.append(downMove[1]);
				break;
			case 'B':
				if (twoOrInv)effectString.append(rBack[2]);
				else effectString.append(rBack[1]);
				break;
			}


			break;
		
		}
	
	
	}

	//

	return effectString;

}


//******************************************************************************************************************************
// code should be reorganized in classes 
//THESE FUNCTIONS DEAL WITH THE RECOGNISING AND INITIAL SOLVING OF THE CUBE 
//******************************************************************************************************************************
//this function draws the 6 sided "opened up" rubik's cube . It's ugly but it does it's job , it can be written as 1 for cycle 


Scalar getColor(int clr) {

	switch ((col)clr) {
	case red:
		return Scalar(0, 0, 255);
		break;
	case green:
		return Scalar(0, 255, 0);
		break;
	case blue:
		return Scalar(255, 0, 0);
		break;
	case orange:
		return Scalar(0, 128, 255);
		break;
	case yellow:
		return Scalar(0, 255, 255);
		break;
	case white:
		return Scalar(255, 255, 255);
		break;
	default:
		return Scalar(0, 0, 0);
		break;
	}

	return Scalar(0, 0, 0);
}


//****************************************************************************
//Bad code , needs chaning in next iteration 
std::string toSolverNotation(bool showProcess , SerialPort arduino) {
	std::string faces[] = { "top" ,"front" , "right" , "back" , "left" , "down" };
	std::string collors[] = { "red" , "green" , "blue" , "yellow" , "orange" , "white" };
	VideoCapture stream1(cn);   //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened()) { //check if video device has been initialised

	}

	//9 rectangles according to the 9 ROI , could be done with some sort of a loop 
	Rect Rec2(220, 100, 130, 130);
	Rect Rec5(220, 265, 130, 130);
	Rect Rec8(220, 410, 60, 60);
	Rect Rec1(365, 100, 130, 130);
	Rect Rec4(365, 270, 130, 130);
	Rect Rec7(365, 410, 60, 60);
	Rect Rec(510, 100, 120, 120);
	Rect Rec3(510, 280, 120, 120);
	Rect Rec6(510, 420, 60, 60);

	//unconditional loop
	//string text = "Funny text inside the box";
	int fontFace = FONT_HERSHEY_COMPLEX;
	double fontScale = 1;
	int thickness = 1;

	Mat cube(360, 500, CV_8UC3, Scalar(0, 0, 0));

	std::string rotations[7] = {"K.Rl.K.S.rL.S\n",
								"S.fB.S.K.Fb.K\n",
								"S.fB.S.K.Fb.K\n",
								"S.fB.S.K.Fb.K\n",
								"S.fB.S.K.FbRl.K.S.rL.S\n",
								"S.rL.S.K.RRll.K.S.rL.S\n",
								}; 

	for (int i = 0; i < CUBE_FACES ; i++) {
		
		while (char((waitKey(100)) != 'a')) {

			cv::putText(cube, faces[i], Point(250, 90), fontFace, 2.8, Scalar::all(255), 8, 8, false);

			cube = (Scalar(255,255, 255));
			drawCube(cube, stickers);
			cv::putText(cube, faces[i], Point(250, 90), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			Mat cameraFrame;
			Mat HSV;
			stream1.read(cameraFrame);
			//bgr to hsv 
			cvtColor(cameraFrame, HSV, COLOR_BGR2HSV);
			
			//тука се поставуваат боите на страните , цело време. Ова би можело да биде поефикасно

			Mat RoiTL = HSV(Rec);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiTL)], Point(140, 90), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[0 + (i * 9)] = (int)mainColor(RoiTL);

			Mat RoiTC = HSV(Rec3);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiTC)], Point(300, 90), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[1 + (i * 9)] = (int)mainColor(RoiTC);

			Mat RoiTR = HSV(Rec6);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiTR)], Point(450, 90), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[2 + (i * 9)] = (int)mainColor(RoiTR);

			Mat RoiML = HSV(Rec1);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiML)], Point(140, 250), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[3 + (i * 9)] = (int)mainColor(RoiML);

			Mat RoiMC = HSV(Rec4);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiMC)], Point(300, 250), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[4 + (i * 9)] = (int)mainColor(RoiMC);
			sideColor[i] = (int)mainColor(RoiMC);

			Mat RoiMR = HSV(Rec7);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiMR)], Point(450, 250), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[5 + (i * 9)] = (int)mainColor(RoiMR);

			Mat RoiDL = HSV(Rec2);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiDL)], Point(140, 400), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[6 + (i * 9)] = (int)mainColor(RoiDL);

			Mat RoiDC = HSV(Rec5);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiDC)], Point(300, 400), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[7 + (i * 9)] = (int)mainColor(RoiDC);

			Mat RoiDR = HSV(Rec8);
			//cv::putText(cameraFrame, collors[(int)mainColor(RoiDR)], Point(450, 400), fontFace, fontScale, Scalar::all(0), thickness, 8, false);
			stickers[8 + (i * 9)] = (int)mainColor(RoiDR);


			
			if (showProcess) {
			

		
				//***
				rectangle(cameraFrame, Rec, Scalar(255), 1, 8, 0);
				rectangle(cameraFrame, Rec1, Scalar(255), 1, 8, 0);
				rectangle(cameraFrame, Rec2, Scalar(255), 1, 8, 0);
				rectangle(cameraFrame, Rec3, Scalar(255), 1, 8, 0);
				rectangle(cameraFrame, Rec4, Scalar(255), 1, 8, 0);
				rectangle(cameraFrame, Rec5, Scalar(255), 1, 8, 0);
				rectangle(cameraFrame, Rec6, Scalar(255), 1, 8, 0);
				rectangle(cameraFrame, Rec7, Scalar(255), 1, 8, 0);
				rectangle(cameraFrame, Rec8, Scalar(255), 1, 8, 0);


				ShowManyImages("picture" , 2 ,cube , cameraFrame );
			
			}


		}

		const char *c_string = rotations[i].c_str();
		
		arduino.writeSerialPort(c_string, rotations[i].length());


	}


	std::string boi = convertToSolverNotation(stickers, sideColor);
	

	return boi; 
}

//**************************************************************************






//order up, right  , front   , down  , left  , back , this is the order the engine uses 
//up , front,  right  ,back ,left  ,down , this is the order my program uses 
//need to convert from one to the other .
//this is a sub function of toSolverNotation
std::string convertToSolverNotation(int *stickers , int *centers) {

	std::string faces[] = { "U" ,"F" , "R" , "B" , "L" , "D" };
	//quick and ugly solution , does it's job 
	int order[CUBE_FACES] = { 0 , 18 , 9 , 45 , 36 , 27 };
	std::string out;
	std::string temp;
	int ctr = 0;
	for (int i = 0; i < CUBE_FACES; i++) {
		for (int k = order[i]; k < (order[i]+9); k++) {
			
			for (int j = 0; j < CUBE_FACES; j++) {
				if (stickers[k] == centers[j]) {
					out.append(faces[j]);
					break;
				}

			}
			

		}
		
	}
	return out;
}

int maxElem(int *mat) {
	int max = mat[0];
	int mm = 0;
	for (int i = 0; i < CUBE_FACES; i++) {
		if (max < mat[i]) {
			max = mat[i];
			mm = i;
		}

	}
	return mm;
}



int mainColor(Mat &mat) {
	Mat output; 


	//red green blue yellow orange white 
	int colors[CUBE_FACES] = {0 , 0 , 0 , 0, 0, 0 } ;
	//blue
	inRange(mat, Scalar(H_MIN[col::blue], S_MIN[col::blue], V_MIN[col::blue]), Scalar(H_MAX[col::blue], S_MAX[col::blue], V_MAX[col::blue]), output);
	colors[2] = countNonZero(output);

	//red
	inRange(mat, Scalar(H_MIN[col::red], S_MIN[col::red], V_MIN[col::red]), Scalar(H_MAX[col::red], S_MAX[col::red], V_MAX[col::red]), output);
	colors[0] = countNonZero(output);
	
	//green
	inRange(mat, Scalar(H_MIN[col::green], S_MIN[col::green], V_MIN[col::green]), Scalar(H_MAX[col::green], S_MAX[col::green], V_MAX[col::green]), output);
	colors[1] = countNonZero(output);

	//yellow
	inRange(mat, Scalar(H_MIN[col::yellow], S_MIN[col::yellow], V_MIN[col::yellow]), Scalar(H_MAX[col::yellow], S_MAX[col::yellow], V_MAX[col::yellow]), output);
	colors[3] = countNonZero(output);
	
	//orange
	inRange(mat, Scalar(H_MIN[col::orange], S_MIN[col::orange], V_MIN[col::orange]), Scalar(H_MAX[col::orange], S_MAX[col::orange], V_MAX[col::orange]), output);
	colors[4] = countNonZero(output);

	//white
	inRange(mat, Scalar(H_MIN[col::white], S_MIN[col::white], V_MIN[col::white]), Scalar(H_MAX[col::white], S_MAX[col::white], V_MAX[col::white]), output);
	colors[5] = countNonZero(output);
	//std::cout << countNonZero(output) << std::endl;
	//std::cout << maxElem(colors); 
	return maxElem(colors);
}




/*function to show multiple images in 1 big window , found on the internet 
ShowManyImages("Image", 6, img1, img2, img3, img4, img5, img6);
*/
void ShowManyImages(string title, int nArgs, ...) {
	int size;
	int i;
	int m, n;
	int x, y;

	// w - Maximum number of images in a row
	// h - Maximum number of images in a column
	int w, h;

	// scale - How much we have to resize the image
	float scale;
	int max;

	// If the number of arguments is lesser than 0 or greater than 12
	// return without displaying
	if (nArgs <= 0) {
		printf("Number of arguments too small....\n");
		return;
	}
	else if (nArgs > 14) {
		printf("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
		return;
	}
	// Determine the size of the image,
	// and the number of rows/cols
	// from number of arguments
	else if (nArgs == 1) {
		w = h = 1;
		size = 500;
	}
	else if (nArgs == 2) {
		w = 2; h = 1;
		size = 500;
	}
	else if (nArgs == 3 || nArgs == 4) {
		w = 2; h = 2;
		size = 500;
	}
	else if (nArgs == 5 || nArgs == 6) {
		w = 3; h = 2;
		size = 400;
	}
	else if (nArgs == 7 || nArgs == 8) {
		w = 4; h = 2;
		size = 400;
	}
	else {
		w = 4; h = 3;
		size = 350;
	}

	// Create a new 3 channel image
	Mat DispImage = Mat::zeros(Size(100 + size * w, (size * h)-60), CV_8UC3);

	// Used to get the arguments passed
	va_list args;
	va_start(args, nArgs);

	// Loop for nArgs number of arguments
	for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {
		// Get the Pointer to the IplImage
		Mat img = va_arg(args, Mat);

		// Check whether it is NULL or not
		// If it is NULL, release the image, and return
		if (img.empty()) {
			printf("Invalid arguments");
			return;
		}

		// Find the width and height of the image
		x = img.cols;
		y = img.rows;

		// Find whether height or width is greater in order to resize the image
		max = (x > y) ? x : y;

		// Find the scaling factor to resize the image
		scale = (float)((float)max / size);

		// Used to Align the images
		if (i % w == 0 && m != 20) {
			m = 20;
			n += 20 + size;
		}

		// Set the image ROI to display the current image
		// Resize the input image and copy the it to the Single Big Image
		Rect ROI(m, n, (int)(x / scale), (int)(y / scale));
		Mat temp; resize(img, temp, Size(ROI.width, ROI.height));
		temp.copyTo(DispImage(ROI));
	}

	// Create a new window, and show the Single Big Image
	namedWindow(title, 1);
	imshow(title, DispImage);
	

	// End the number of arguments
	va_end(args);
}

void drawCube(Mat &img, int stickers[54]) {
	int ctr = 0;
	int shiftV = 0;
	int shiftH = 0;
	for (int i = 0; i < 9; i++) {
		rectangle(img, Rect(120 + (shiftH), shiftV, 40, 40), getColor(stickers[i]), CV_FILLED, 8, 0);
		rectangle(img, Rect(120 + (shiftH), shiftV, 40, 40), Scalar(0, 0, 0), 1, 8, 0);
		ctr++;
		shiftH += 40;
		if (ctr == 3) {
			shiftV += 40;
			ctr = 0;
			shiftH = 0;
		}

	}
	shiftV = 120;
	shiftH = 0;
	for (int i = 9; i < 18; i++) {
		rectangle(img, Rect(120 + (shiftH), shiftV, 40, 40), getColor(stickers[i]), CV_FILLED, 8, 0);
		rectangle(img, Rect(120 + (shiftH), shiftV, 40, 40), Scalar(0, 0, 0), 1, 8, 0);
		ctr++;
		shiftH += 40;
		if (ctr == 3) {
			shiftV += 40;
			ctr = 0;
			shiftH = 0;
		}

	}
	shiftV = 120;
	shiftH = 0;
	for (int i = 18; i < 27; i++) {
		rectangle(img, Rect(240 + (shiftH), shiftV, 40, 40), getColor(stickers[i]), CV_FILLED, 8, 0);
		rectangle(img, Rect(240 + (shiftH), shiftV, 40, 40), Scalar(0, 0, 0), 1, 8, 0);
		ctr++;
		shiftH += 40;
		if (ctr == 3) {
			shiftV += 40;
			ctr = 0;
			shiftH = 0;
		}

	}
	shiftV = 120;
	shiftH = 0;
	for (int i = 27; i < 36; i++) {
		rectangle(img, Rect(360 + (shiftH), shiftV, 40, 40), getColor(stickers[i]), CV_FILLED, 8, 0);
		rectangle(img, Rect(360 + (shiftH), shiftV, 40, 40), Scalar(0, 0, 0), 1, 8, 0);
		ctr++;
		shiftH += 40;
		if (ctr == 3) {
			shiftV += 40;
			ctr = 0;
			shiftH = 0;
		}

	}
	shiftV = 120;
	shiftH = 0;
	for (int i = 36; i < 45; i++) {
		rectangle(img, Rect(0 + (shiftH), shiftV, 40, 40), getColor(stickers[i]), CV_FILLED, 8, 0);
		rectangle(img, Rect(0 + (shiftH), shiftV, 40, 40), Scalar(0, 0, 0), 1, 8, 0);
		ctr++;
		shiftH += 40;
		if (ctr == 3) {
			shiftV += 40;
			ctr = 0;
			shiftH = 0;
		}

	}
	shiftV = 240;
	shiftH = 0;
	for (int i = 45; i < 54; i++) {
		rectangle(img, Rect(120 + (shiftH), shiftV, 40, 40), getColor(stickers[i]), CV_FILLED, 8, 0);
		rectangle(img, Rect(120 + (shiftH), shiftV, 40, 40), Scalar(0, 0, 0), 1, 8, 0);
		ctr++;
		shiftH += 40;
		if (ctr == 3) {
			shiftV += 40;
			ctr = 0;
			shiftH = 0;
		}

	}

}
