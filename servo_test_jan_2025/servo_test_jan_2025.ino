
const int apin = A0;
int apin_val = 128;
long sleep_int = 20;
long pwmlen = sleep_int / 2;
//

void setup() {
  // put your setup code here, to run once:

  pinMode(apin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(apin, HIGH);
  delay(sleep_int-pwmlen);
  digitalWrite(apin, 0);
  delay(pwmlen);
  
  pwmlen++;
  if(pwmlen >= sleep_int) pwmlen = 1;
}
