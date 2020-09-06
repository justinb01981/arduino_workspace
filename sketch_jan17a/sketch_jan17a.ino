
/*
 * microphone-demo
 */


int microphoneAnalogPin = 0;
int ledDigitalPin = 9;
int unlockDigitalPin = 8;
int rangeTrigger = 0;
int rangeTriggerCounter = 500; // first N passes, learn max-range-trigger
int delayResolution = 1;
int highLast = 0;
int highLastLength = 0;
int lowLastLength = 0;
int lowLengthMax = 100;
int jitterIgnore = 4;


//int desiredSequenceMin[] = {2, 100, 2, 100, 2};
//int desiredSequenceMax[] = {30, 200, 30, 200, 30};

// SOS
//int desiredSequenceMin[] = {2, 2, 2, 10, 10, 10, 2, 2, 2};
//int desiredSequenceMax[] = {30, 30, 30, 120, 120, 120, 30, 30, 30};

// basic 5 sec hold
int desiredSequenceMin[] = {30, 30};
int desiredSequenceMax[] = {3000,3000};

unsigned int sequenceLen = sizeof(desiredSequenceMin) / sizeof(int);
int storedSequence[16];
unsigned int storedSequenceI = 0;

// the setup routine runs once when you press reset:
void setup()  { 
  Serial.begin(9600);
  Serial.print("setup\n");
  
  pinMode(ledDigitalPin, OUTPUT);
  pinMode(unlockDigitalPin, OUTPUT);
  
  rangeTrigger = 0;
}

// the loop routine runs over and over again forever:
void loop()  {
      char str[255];
      
      int range = 0;
      int readCount = 10; // millisecond
      int highRead = 0;
      int lowRead = 1024;
      while(readCount > 0)
      {
        int areadVal = analogRead(microphoneAnalogPin);
        
        if(areadVal < lowRead) lowRead = areadVal;
        if(areadVal > highRead) highRead = areadVal;
        
        //if(high - low > range) range = high - low;
        range = highRead - lowRead;
        readCount--;
        
        if(delayResolution > 0)
        { 
          delay(delayResolution);
        }
      }
      
      if(rangeTriggerCounter > 0)
      {
        if(range > rangeTrigger) rangeTrigger = range;
        rangeTriggerCounter--;
        if(rangeTriggerCounter == 0)
        {
          int signalCount = 5;
          // signal completion
          while(signalCount > 0)
          {
            digitalWrite(ledDigitalPin, 1);
            delay(1000);
            signalCount--;
            digitalWrite(ledDigitalPin, 0);
            delay(1000);
          }
        }
      }
      
      //sprintf(str, "analogRead range:%d\n", range);
      //Serial.print(str);
      
      digitalWrite(ledDigitalPin, range >= rangeTrigger? 1: 0);
      
      int high = (range >= rangeTrigger);
      if (high != highLast)
      {
        //sprintf(str, "highLength:%d", highLastLength);
        //Serial.println(str);
        
        if (highLastLength >= jitterIgnore && !high)
        {
          // log length into sequence
          storedSequence[storedSequenceI % sequenceLen] = highLastLength;
          storedSequenceI++;
        }
        
        // compare sequences
        Serial.print("sequence:");
        unsigned int i = 0;
        while(i < sequenceLen)
        {
          sprintf(str, "%d,", storedSequence[(storedSequenceI+i) % sequenceLen]);
          Serial.print(str);
          i++;
        }
        
        i = 0;
        while(i < sequenceLen)
        {
          unsigned int storedV = storedSequence[(storedSequenceI+i) % sequenceLen];
          if (storedV < desiredSequenceMin[i] || storedV > desiredSequenceMax[i]) break;
          i++;
        }
        //Serial.print("\n");
        
        if(i == sequenceLen)
        {
          int count = 10;
          while(count > 0)
          {
            Serial.println("SEQUENCE MATCH");
            digitalWrite(unlockDigitalPin, 1);
            delay(100);
            count--;
          }
          digitalWrite(unlockDigitalPin, 0);
          // clear sequence
          storedSequence[(storedSequenceI-1) % sequenceLen] = 0;
        }
        highLastLength = 0;
        lowLastLength = 0;
      }
      else
      {
        if(high)
        {
          highLastLength++;
        }
        else
        {
          lowLastLength++;
          if(lowLastLength > lowLengthMax)
          {
            storedSequence[storedSequenceI % sequenceLen] = 0;
            storedSequenceI++;
            lowLastLength = 0;
          }
        }
      }
      highLast = high;
}



