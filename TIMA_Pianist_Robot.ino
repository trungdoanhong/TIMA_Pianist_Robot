#include <Wire.h>
#include "Sheet.h"
#include "PianoConstant.h"
#include "PianoHand.h"
#include "VirtualTimer.h"
#include "fastio.h"
#include "PCA9685.h"

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
#define LEFT_RELEASE_ANGLE 30

#define RIGHT_PRESS_ANGLE -50
#define RIGHT_RELEASE_ANGLE 30

String SheetSector = "G4-1 C5 D5 D5-2 E5 E5-8 C5-1 D5 E5 D5-2 G5 G5-9 RE-1 C5-1 D5 E5 D5-2 G5 G5-8 RE-1 G5-1 A5 B5 B5-2 C6 C6-6 D6-2 E6 D6 C6 B5-8 RE-1";

PianoHand* LeftHand;
PianoHand* RightHand;

Sheet PianoSheet;

uint8_t LeftPins[5] = { 0, 1, 2, 3, 4 };
uint8_t RightPins[5] = { 5, 6, 7, 8, 9 };

void setup()
{
	Serial.begin(9600);

	Serial.println("Serial begin");

	LeftHand = new PianoHand();
	RightHand = new PianoHand();

	LeftHand->Init(LeftPins, C3, D3, E3, F3, G3);
	RightHand->Init(RightPins, C5, D5, E5, F5, G5);

	LeftHand->SetStepperMotor(X_STEP, X_DIR, EN, X_ENDSTOP);
	RightHand->SetStepperMotor(Z_STEP, Z_DIR, EN, Z_ENDSTOP);

	LeftHand->SetActiveAngle(LEFT_RELEASE_ANGLE, LEFT_PRESS_ANGLE);
	RightHand->SetActiveAngle(RIGHT_RELEASE_ANGLE, RIGHT_PRESS_ANGLE);

	/*PianoSheet.SetTempo(65);
	PianoSheet.SetSheet(SheetSector, LEFT);

	PianoSheet.SetHandToneFa(LeftHand);
	PianoSheet.SetHandToneSol(RightHand);*/

	VirtualTimer.Init();

	VirtualTimer.Add(ToggleStepPinX, 40);
	VirtualTimer.Add(ToggleStepPinZ, 40);
	VirtualTimer.Run();

	Home();
}

void loop()
{
	//PianoSheet.Execute();
}

void Home()
{
	//LeftHand->UpdateHandPlacement(C8);

	//LeftHand->Move(A0);

	//while (!LeftHand->IsEndstopActive())
	//{
	//}

	//LeftHand->Stop();

	//LeftHand->UpdateHandPlacement(A0);

	//----------------------------------------

	Serial.println("move home");

	RightHand->UpdateHandPlacement(A0);
	RightHand->Move(F7);

	while (!RightHand->IsEndstopActive())
	{

	}

	RightHand->Stop();

	RightHand->UpdateHandPlacement(F7);

	Serial.println("finish move");
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