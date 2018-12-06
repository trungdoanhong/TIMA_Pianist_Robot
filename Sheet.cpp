// 
// 
// 

#include "Sheet.h"

Sheet::Sheet()
{
	executeThread = MultiThread();
}

void Sheet::ReadNote()
{

}

void Sheet::SetHandToneFa(PianoHand * left)
{
	hand[LEFT] = left;
}

void Sheet::SetHandToneSol(PianoHand * right)
{
	hand[RIGHT] = right;
}

void Sheet::SetTempo(int tem)
{
	tempo = tem;
	interval = (60000) / (tempo * 4);
}

void Sheet::SetSheet(String s, uint8_t handID)
{
	sheet[handID] = s;
	convertToNotes(notes[handID], sheet[handID]);
	getNewSection(handID);
	getNextNotes(handID);
}

void Sheet::Execute()
{
	if (hand[LEFT]->IsMoving() || hand[RIGHT]->IsMoving())
	{
		return;
	}

	checkHandPosition(LEFT);
	checkHandPosition(RIGHT);

	if (isHandInPosition == false)
	{
		return;
	}

	checkTimeForReleaseFinger(LEFT);
	checkTimeForReleaseFinger(RIGHT);

	if (isJustRelease[LEFT] == true || isJustRelease[RIGHT] == true)
	{
		delay(UP_FINGER_TIME);
		checkStateForMoveNextPosition(LEFT);
		checkStateForMoveNextPosition(RIGHT);

		isJustRelease[LEFT] = false;
		isJustRelease[RIGHT] = false;

		return;
	}	

	RUN_EVERY(executeThread, interval);
	
	checkBeatForPress(LEFT);
	checkBeatForPress(RIGHT);
}

void Sheet::getNewSection(uint8_t handID)
{
	uint8_t value;
	currentNoteOrder[handID] = destinationNoteOrder[handID];

	while (1)
	{
		destinationNoteOrder[handID]++;		

		if (notes[handID][destinationNoteOrder[handID]] >= N1)
		{
			if (destinationNoteOrder[handID] - currentNoteOrder[handID] >= 2 && (notes[handID][destinationNoteOrder[handID]] - value) >= 2)
			{
				return;
			}

			if (notes[handID][destinationNoteOrder[handID] - 1] == RE)
			{
				return;
			}

			value = notes[handID][destinationNoteOrder[handID]];
		}
	}
}

void Sheet::getNextNotes(uint8_t handID)
{
	if (notes[handID][currentNoteOrder[handID]] >= N1)
	{
		noteValue[handID] = notes[handID][currentNoteOrder[handID]];
		currentNoteOrder[handID]++;
	}
	
	uint8_t highestNote = pressNotes[handID][0];
	uint8_t lowestNote = pressNotes[handID][0];
	uint8_t lastNote = pressNotes[handID][0];
	uint8_t noteNumber = 0;

	bool isGoOn = false;

	if (notes[handID][currentNoteOrder[handID]] < notes[handID][currentNoteOrder[handID] + 1])
		isGoOn = true;

	for (uint16_t i = currentNoteOrder[handID] + 1; i <= destinationNoteOrder[handID]; i++)
	{
		if (currentNoteOrder[handID] > C8)
		{
			continue;
		}

		noteNumber++;

		if (isGoOn == true)
		{
			if (currentNoteOrder[handID] > highestNote)
			{
				highestNote = currentNoteOrder[handID];
			}

			if (currentNoteOrder[handID] < lastNote)
			{
				break;
			}
		}
		else
		{
			if (currentNoteOrder[handID] < lowestNote)
			{
				lowestNote = currentNoteOrder[handID];
			}

			if (currentNoteOrder[handID] > lastNote)
			{
				break;
			}
		}
	}

	if (noteNumber > 5)
		noteNumber = 5;

	pressNotes[handID][0] = notes[handID][currentNoteOrder[handID]];

	handPlacement[handID] = pressNotes[handID][0] - (5 - noteNumber);
}

// "G4-1 C5 D5 D5-2 E5 E5-8 C5-1 D5 E5 D5-2 G5 G5-9 RE-1 C5-1 D5 E5 D5-2 G5 G5-8 RE-1 G5-1 A5 B5 B5-2 C6 C6-6 D6-2 E6 D6 C6 B5-8 RE-1";


void Sheet::convertToNotes(uint8_t * ns, String s)
{
	uint16_t idOrder = 0;
	uint16_t spaceOrder1 = 0;
	uint16_t spaceOrder2 = 1;
	uint16_t noteOrder = 0;
	uint8_t noteValue = 1;

	while (spaceOrder2 != s.length())
	{
		spaceOrder2 = s.indexOf(' ', spaceOrder1 + 1);

		if (spaceOrder2 < 0)
			spaceOrder2 = s.length();

		String keyS = s.substring(spaceOrder1, spaceOrder2);

		spaceOrder1 = spaceOrder2 + 1;


		uint8_t barOrder = keyS.indexOf('-');

		if (barOrder < 0)
		{
			barOrder = keyS.length();
		}
		else
		{
			noteValue = getValueID(keyS.substring(barOrder));
		}

		String noteS = keyS.substring(0, barOrder);

		uint8_t noteID = getNoteID(noteS);

		ns[noteOrder] = noteID;
		noteOrder++;
		ns[noteOrder] = noteValue;
	}
}

bool Sheet::isNoteSign(char c)
{
	if (c <= 'A' && c <= 'G')
		true;
	else
		false;
}

bool Sheet::isNumber(char c)
{
	if (c >= '0' && c <= '9')
		true;
	else
		false;
}

uint8_t Sheet::getNoteID(String note)
{
	uint8_t order;
	uint8_t tone;

	if (note[0] == 'C')
		order = 0;
	if (note[0] == 'D')
		order = 1;
	if (note[0] == 'E')
		order = 2;
	if (note[0] == 'F')
		order = 3;
	if (note[0] == 'G')
		order = 4;
	if (note[0] == 'A')
		order = 5;
	if (note[0] == 'B')
		order = 6;

	tone = note[1] - '0';

	if (note == "RE")
		return RE;
	else
		return tone * 7 + order - 5;
}

uint8_t Sheet::getValueID(String value)
{
	return value.toInt() + 70;
}

void Sheet::checkHandPosition(uint8_t handID)
{
	if (hand[handID]->GetCurrentPos() != handPlacement[handID])
	{
		hand[handID]->Move(handPlacement[handID]);
		isHandInPosition = false;
	}
	else
	{
		isHandInPosition = true;
	}
}

void Sheet::checkTimeForReleaseFinger(uint8_t handID)
{
	if (releaseThread[handID].isSchedule(hand[handID]->PressTime) && hand[handID]->IsPressing())
	{
		hand[handID]->Release();
		isJustRelease[handID] = true;
	}
}

void Sheet::checkStateForMoveNextPosition(uint8_t handID)
{
	if (isJustRelease[handID] == true)
	{
		currentNoteOrder[handID]++;

		if (currentNoteOrder[handID] > destinationNoteOrder[handID])
		{
			getNewSection(handID);
		}

		getNextNotes(handID);

		hand[handID]->Move(handPlacement[handID]);
	}
}

void Sheet::checkBeatForPress(uint8_t handID)
{
	hand[handID]->SkipBeat--;

	if (!hand[handID]->IsMoving() && hand[handID]->SkipBeat <= 0)
	{
		hand[handID]->PressKeys(pressNotes[handID], noteValue[handID]);
		releaseThread[handID].Counter = millis();
	}
}

uint16_t Sheet::log2(uint16_t n)
{
	int i = 0;
	int r = 1;
	
	while (1)
	{
		if (r >= n)
			return i;
		i++;
		r *= 2;		
	}
}
