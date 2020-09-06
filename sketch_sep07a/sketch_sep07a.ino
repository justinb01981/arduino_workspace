/*
 * stepper-demo
 */


int relayPins[] = {4,5,6,7};
int relayState[] = {0,0,0,0};
int relayIds[] = {4,3,2,1};

#define trigPin 10
#define echoPin 13

enum {
 MOTORSTATE_FORWARD,
 MOTORSTATE_STOP,
 MOTORSTATE_REVERSE
};

// the setup routine runs once when you press reset:
void setup()  { 
  Serial.begin(9600);
  Serial.print("setup\n");
  // declare pins as outputs
  for(int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], 0);
  }
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void relaySet(int idx, int lh)
{
  int ms = 100;
  relayState[idx] = lh? 1: 0;
  digitalWrite(relayPins[idx], relayState[idx]);
  delay(ms);
}

void relaySwap(int idx)
{
  relaySet(idx, !relayState[idx]);
}

void motorState(int idx, int state)
{
  int pins[2];
  int pinState[2];
  
  switch(idx) {
    case 0:
    pins[0] = 6;
    pins[1] = 7;
    break;
    
    case 1:
    pins[0] = 4;
    pins[1] = 5;
  }
  
  switch(state)
  {
    case MOTORSTATE_FORWARD:
    pinState[0] = 0;
    pinState[1] = 0;
    break;
    
    case MOTORSTATE_REVERSE:
    pinState[0] = 1;
    pinState[1] = 1;
    break;
    
    case MOTORSTATE_STOP:
    pinState[0] = 0;
    pinState[0] = 0;
    break;
  }
  
  // avoid a short by ordering correctly?
  digitalWrite(pins[0], pinState[0]);
  digitalWrite(pins[1], pinState[1]);
}

int motorPlan1[] = {MOTORSTATE_FORWARD, MOTORSTATE_FORWARD,MOTORSTATE_FORWARD,MOTORSTATE_FORWARD, MOTORSTATE_FORWARD, MOTORSTATE_FORWARD, MOTORSTATE_REVERSE,MOTORSTATE_REVERSE};
int motorPlan2[] = {MOTORSTATE_FORWARD, MOTORSTATE_FORWARD,MOTORSTATE_FORWARD,MOTORSTATE_FORWARD, MOTORSTATE_FORWARD, MOTORSTATE_FORWARD, MOTORSTATE_FORWARD,MOTORSTATE_FORWARD};

int mp1 = 0;
int mp2 = 0;

int motorIntervalCur = 0;
int motorInterval = 1000;
int sensorInterval = 500;
int sensorIntervalCur = 0;
int delayResolution = 10;
int distanceLast = 0;
int distanceTooClose = 20;
int useMotorPlan = 0;

// the loop routine runs over and over again forever:
void loop()  {

      if(motorIntervalCur == 0)
      {
        if(!useMotorPlan) {
          if(distanceLast <= distanceTooClose)
          {
            motorState(0, MOTORSTATE_FORWARD);
            motorState(1, MOTORSTATE_REVERSE);
          }
          else
          {
            motorState(0, MOTORSTATE_FORWARD);
            motorState(1, MOTORSTATE_FORWARD);
          }
        }
        else
        {
        motorState(0, motorPlan1[mp1]);
        mp1++;
        if(mp1 >= sizeof(motorPlan1) / sizeof(int)) mp1 = 0;
      
        motorState(1, motorPlan2[mp2]);
        mp2++;
        if(mp2 >= sizeof(motorPlan2) / sizeof(int)) mp2 = 0;
        motorIntervalCur = motorInterval;
        }
      } else 
      {
        motorInterval -= delayResolution;
      }
      
      if(sensorIntervalCur == 0)
      {
          sensorIntervalCur = sensorInterval;
          
          float duration, distance;
          // send a PING
          digitalWrite(trigPin, LOW); 
          delayMicroseconds(2);
 
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
  
          // wait for the pulse back
          duration = pulseIn(echoPin, HIGH);
          distance = (duration / 2) * 0.0344;
          
          Serial.print("Distance=");
          Serial.print(distance);
          Serial.print("\n");
          
          distanceLast = distance;
      }
      else
      {
        sensorIntervalCur -= delayResolution;
      }
      
      delay(delayResolution);
}



