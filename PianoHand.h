// PianoHand.h

#ifndef _PIANOHAND_h
#define _PIANOHAND_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PianoConstant.h"
#include "PCA9685.h"

#define DOWNING		0
#define PRESSING	1
#define UPPING		2
#define RELEASING	3

#define UP_FINGER_TIME	100

#define STEPS_PER_MM		160
#define WIDTH_OF_KEY		23.5f
#define PLACEMENT_OFFSET	0

class PianoHand
{
public:
	PianoHand();
	void Init(uint8_t* pins, uint8_t f1, uint8_t f2, uint8_t f3, uint8_t f4, uint8_t f5);
	void SetStepperMotor(uint8_t step, uint8_t dir, uint8_t en, uint8_t ends);
	void SetActiveAngle(int releaseAngle, int pressAngle);
	int* GetFingerState();
	uint8_t GetCurrentPos();
	void PressKey(uint8_t key, uint8_t value);
	void PressKeys(uint8_t* keys, uint8_t value);
	void Release();
	void Move(uint8_t note);
	void Stop();
	void UpdateHandPlacement(uint8_t  note);
	bool IsEndstopActive();
	bool IsMoving();
	bool IsPressing();

	long CurrentStep = 0;
	long DesiredStep = 0;
	int8_t DirectionValue = 1;
	long JumpSteps = 0;

	bool IsInvertDirection = false;

	int8_t SkipBeat = 1;
	uint16_t PressTime = 0;

private:
	void setDirection(int8_t dir);
	void updateFingerPosition();

	int fingers[5] = { C5 ,D5, E5, F5, G5 };
	uint8_t currentPos = A0;
	uint8_t* fingerPins;
	uint8_t stepPin;
	uint8_t dirPin;
	uint8_t enPin;
	uint8_t endsPin;

	uint16_t toggleDuty;

	int releaseAngle;
	int pressAngle;

	uint16_t movingSpeed = 150;

	uint8_t fingerState;

	PCA9685_ServoEvaluator PWMMaker;
	PCA9685 ServoManager;

	float keyWidth = 23.5f;
};

#endif

