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
	convertToNotes(handID, s);

	getNewSection(handID);
	Serial.println(destinationNoteOrder[handID]);

	getNextNotes(handID);
	Serial.println(pressNotes[handID][0]);
	Serial.println(handPlacement[handID]);
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

	Serial.print("P:");
	Serial.println(getNoteName(hand[RIGHT]->GetCurrentPos()));
	
	checkBeatForPress(LEFT);
	checkBeatForPress(RIGHT);
}

void Sheet::getNewSection(uint8_t handID)
{
	uint8_t value =  0;
	currentNoteOrder[handID] = destinationNoteOrder[handID];

	while (destinationNoteOrder[handID] < notesSize[handID])
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

		if (currentNoteOrder[handID] - 1 < notesSize[handID])
		{
			currentNoteOrder[handID]++;
		}
	}
	
	uint8_t highestNote = pressNotes[handID][0];
	uint8_t lowestNote = pressNotes[handID][0];
	uint8_t lastNote = pressNotes[handID][0];
	uint8_t noteNumber = 0;

	bool isGoOn = false;

	int16_t handPlaceOrder = currentNoteOrder[handID];
	while (handPlaceOrder < notesSize[handID])
	{
		if (notes[handID][handPlaceOrder] == RE)
		{
			handPlaceOrder += 2;
		}
		else
		{
			break;
		}
	}

	if (notesSize[handID] - handPlaceOrder > 3 )
	{
		if (notes[handID][handPlaceOrder] < notes[handID][handPlaceOrder + 2])
			isGoOn = true;

		for (uint16_t i = handPlaceOrder; i <= notesSize[handID]; i+=2)
		{
			if (handPlaceOrder > C8)
			{
				continue;
			}

			noteNumber++;

			if (isGoOn == true)
			{
				if (handPlaceOrder > highestNote)
				{
					highestNote = handPlaceOrder;
				}

				if (handPlaceOrder < lastNote)
				{
					break;
				}
			}
			else
			{
				if (handPlaceOrder < lowestNote)
				{
					lowestNote = handPlaceOrder;
				}

				if (handPlaceOrder > lastNote)
				{
					break;
				}
			}
		}

		if (noteNumber > 5)
			noteNumber = 5;
	}

	pressNotes[handID][0] = notes[handID][currentNoteOrder[handID]];

	Serial.print("po:");
	Serial.println(handPlaceOrder);

	handPlacement[handID] = notes[handID][handPlaceOrder] - (5 - noteNumber);

	noteValue[handID] = notes[handID][currentNoteOrder[handID] + 1];
}

// "G4-1 C5 D5 D5-2 E5 E5-8 C5-1 D5 E5 D5-2 G5 G5-9 RE-1 C5-1 D5 E5 D5-2 G5 G5-8 RE-1 G5-1 A5 B5 B5-2 C6 C6-6 D6-2 E6 D6 C6 B5-8 RE-1";


void Sheet::convertToNotes(uint8_t handID, String s)
{
	int idOrder = 0;
	int spaceOrder1 = 0;
	int spaceOrder2 = 1;
	uint16_t noteOrder = 0;
	int8_t noteValue = 1;

	while (spaceOrder2 != s.length())
	{
		spaceOrder2 = s.indexOf(' ', spaceOrder1 + 1);

		if (spaceOrder2 < 0)
			spaceOrder2 = s.length();

		String keyS = s.substring(spaceOrder1, spaceOrder2);

		Serial.print(keyS);
		Serial.print(" ");

		spaceOrder1 = spaceOrder2 + 1;

		int8_t barOrder = keyS.indexOf('-');

		if (barOrder < 0)
		{
			barOrder = keyS.length();
		}
		else
		{
			noteValue = getValueID(keyS.substring(barOrder + 1));
		}

		String noteS = keyS.substring(0, barOrder);
		
		uint8_t noteID = getNoteID(noteS);

		notes[handID][noteOrder] = noteID;
		++noteOrder;
		notes[handID][noteOrder] = noteValue;
		++noteOrder;
	}

	notesSize[handID] = noteOrder;

	printNotes(handID);
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
		return tone * 7 + order - 5 + 1;
}

uint8_t Sheet::getValueID(String value)
{
	return value.toInt() + 70;
}

String Sheet::getNoteName(uint8_t id)
{
	uint8_t order;
	uint8_t tone;
	String name = "";
	
	tone = (id + 4) / 7;
	order = (id + 4) % 7;

	if (tone == 0)
		name = "C";
	if (tone == 1)
		name = "D";
	if (tone == 2)
		name = "E";
	if (tone == 3)
		name = "F";
	if (tone == 4)
		name = "G";
	if (tone == 5)
		name = "A";
	if (tone == 6)
		name = "B";

	if (tone == RE)
		name = "RE";
	else
		name += String(order);

	return name;
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
		currentNoteOrder[handID]+=2;

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
		Serial.print("k:");
		Serial.println(pressNotes[handID][0]);
		hand[handID]->PressKeys(pressNotes[handID], noteValue[handID] - 70);
		hand[handID]->PressTime = interval * hand[handID]->SkipBeat - UP_FINGER_TIME;
		releaseThread[handID].Counter = millis();
	}
}

void Sheet::printNotes(uint8_t handID)
{
	for (uint16_t i = 0; i < notesSize[handID]; i++)
	{
		Serial.print(notes[handID][i]);
		Serial.print("-");
		Serial.print(notes[handID][++i]);
		Serial.print(" ");
	}
}

int16_t Sheet::log2(int16_t n)
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