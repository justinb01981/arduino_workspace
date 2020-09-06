/*
 * stepper-demo
 */

//int pinSequence[] = {5,2,3,4};
int pinSequence1[] = {5,2,3,4};
//int pinSequence1[] = {2,3,4,5};
int pinSequence2[] = {8,9,10,11};
int pinCur1 = 0;
int pinCur2 = 0;
int pinSequenceInc1 = 1;
int pinSequenceInc2 = 1;
int pinDelayMs = 1;
int halfStep = 1;
int pinSequenceLen = sizeof(pinSequence1)/sizeof(int);

// the setup routine runs once when you press reset:
void setup()  { 
  Serial.begin(9600);
  Serial.print("setup\n");
  pinCur1 = 0;
  pinCur2 = 0;
  // declare pins as outputs
  for(int i = 0; i < pinSequenceLen; i++) {
    pinMode(pinSequence1[i], OUTPUT);
    pinMode(pinSequence2[i], OUTPUT);
    digitalWrite(pinSequence1[i], 0);
    digitalWrite(pinSequence2[i], 0);
  }
}

void stepAdvance(int *pos, int* pinArr, int pinArrLen, int dir, int half)
{
  // advance
  int posLast = *pos;
  
  *pos = *pos + dir;
  if(*pos >= pinArrLen) *pos = 0;
  if(*pos < 0) *pos = pinArrLen-1;
  
  digitalWrite(pinArr[*pos], HIGH);
  delay(pinDelayMs);
  
  if(half)
  {
    digitalWrite(pinArr[posLast], LOW);
    delay(pinDelayMs);
  }
  else
  {
    digitalWrite(pinArr[*pos], LOW);
    delay(pinDelayMs);
  }
}

// the loop routine runs over and over again forever:
void loop()  {
  stepAdvance(&pinCur1, pinSequence1, sizeof(pinSequence1)/sizeof(int), pinSequenceInc1, halfStep);
  stepAdvance(&pinCur2, pinSequence2, sizeof(pinSequence2)/sizeof(int), pinSequenceInc2, halfStep);
}



