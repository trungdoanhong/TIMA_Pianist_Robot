#include <Wire.h>
#include "PCA9685.h"
#include "Sheet.h"
#include "PianoConstant.h"
#include "PianoHand.h"
#include "VirtualTimer.h"
#include "fastio.h"

#define X_STEP			2
#define Y_STEP			3
#define Z_STEP			4

#define X_DIR			5
#define Y_DIR			6
#define Z_DIR			7

#define EN				8

#define X_ENDSTOP		9
#define Y_ENDSTOP		10
#define Z_ENDSTOP		11

#define LEFT_PRESS_ANGLE -50
#define LEFT_RELEASE_ANGLE 10

#define RIGHT_PRESS_ANGLE 30
#define RIGHT_RELEASE_ANGLE -30

//String SheetSector = "G4-1 C5 D5 D5-2 E5 E5-8 C5-1 D5 E5 D5-2 G5 G5-9 RE-1 C5-1 D5 E5 D5-2 G5 G5-8 RE-1 G5-1 A5 B5 B5-2 C6 C6-6 D6-2 E6 D6 C6 B5-8 RE-1";
String RightSheet = "RE-4 RE RE RE-1 G4-1 C5 D5 D5-2 E5 E5-8 RE-1 C5-1 D5 E5";
String LeftSheet = "RE-16 C3-2 G3 C4 D4 E4 RE-2 RE-4";

PianoHand* LeftHand;
PianoHand* RightHand;

Sheet PianoSheet;

uint8_t RightPins[5] = { 4, 3, 2, 1, 0 };
uint8_t LeftPins[5] = { 9, 8, 7, 6, 5 };

void setup()
{
	Serial.begin(9600);

	Serial.println("Serial begin");

	VirtualTimer.Init();

	VirtualTimer.Add(ToggleStepPinX, 40);
	VirtualTimer.Add(ToggleStepPinZ, 40);
	VirtualTimer.Run();

	LeftHand = new PianoHand();	
	RightHand = new PianoHand();
	
	LeftHand->Init(LeftPins, C3, D3, E3, F3, G3);
	RightHand->Init(RightPins, C5, D5, E5, F5, G5);
	
	LeftHand->SetStepperMotor(X_STEP, X_DIR, EN, X_ENDSTOP);
	RightHand->SetStepperMotor(Z_STEP, Z_DIR, EN, Z_ENDSTOP);
	
	LeftHand->SetActiveAngle(LEFT_RELEASE_ANGLE, LEFT_PRESS_ANGLE);
	RightHand->SetActiveAngle(RIGHT_RELEASE_ANGLE, RIGHT_PRESS_ANGLE);

	PianoSheet.SetTempo(65);
	PianoSheet.SetSheet(RightSheet, RIGHT);
	PianoSheet.SetSheet(LeftSheet, LEFT);

	PianoSheet.SetHandToneFa(LeftHand);
	PianoSheet.SetHandToneSol(RightHand);
	RightHand->Release();
	LeftHand->Release();	
	delay(500);
	Home();

	//RightHand->TestServo();
	
	//LaunchFingers();
}

void loop()
{	
	PianoSheet.Execute();
}

void LaunchFingers()
{
	RightHand->Move(G4);
	LeftHand->Move(C3);
	WaitR();
	WaitL();
	RightHand->PressKey(G4, 1);
	delay(300);
	RightHand->Release();
	delay(100);
	RightHand->PressKey(C5, 1);
	delay(300);
	RightHand->Release();
	delay(100);
	RightHand->PressKey(D5, 1);
	delay(300);
	RightHand->Release();
	delay(100);
	RightHand->PressKey(D5, 1);
	LeftHand->PressKey(C3, 1);
	delay(300);
	RightHand->Release();
	delay(100);
	RightHand->PressKey(E5, 1);
	delay(300);
	RightHand->Release();
	delay(100);
	RightHand->PressKey(E5, 1);
	delay(300);
	RightHand->Release();
	delay(100);





	/*for (uint8_t i = 0; i < 5; i++)
	{
		delay(1000);
		LeftHand->PressKey(A0 + i, 1);
		RightHand->PressKey(G4 + i, 1);
		delay(1000);
		LeftHand->Release();
		RightHand->Release();
	}*/
}

void WaitR()
{
	while (RightHand->IsMoving())
	{
		delay(1);
	};
}

void WaitL()
{
	while (LeftHand->IsMoving())
	{
		delay(1);
	};
}

void Home()
{
	LeftHand->UpdateHandPlacement(C8);

	LeftHand->Move(A0);

	while (!LeftHand->IsEndstopActive())
	{

	}

	LeftHand->Stop();

	LeftHand->UpdateHandPlacement(A0);

	//----------------------------------------

	RightHand->UpdateHandPlacement(A0);
	RightHand->Move(F7);

	/*while (1)
	{
		Serial.println(RightHand->IsEndstopActive());
		Serial.println(LeftHand->IsEndstopActive());
		delay(100);
	}*/

	while (!RightHand->IsEndstopActive())
	{
		
	}

	RightHand->Stop();

	RightHand->UpdateHandPlacement(F7);

}

void ToggleStepPinX()
{
	if (LeftHand->CurrentStep == LeftHand->DesiredStep)
	{
		return;
	}

	TOGGLE(X_STEP);

	LeftHand->CurrentStep += LeftHand->DirectionValue;
}

void ToggleStepPinZ()
{
	if (RightHand->CurrentStep == RightHand->DesiredStep)
	{
		return;
	}

	TOGGLE(Z_STEP);

	RightHand->CurrentStep += RightHand->DirectionValue;
}