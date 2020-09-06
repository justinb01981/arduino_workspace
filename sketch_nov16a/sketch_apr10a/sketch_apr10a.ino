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
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop()  {
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  delay(3000);
  
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  delay(2000);
  
  digitalWrite(3, LOW);
  delay(2000);
}
