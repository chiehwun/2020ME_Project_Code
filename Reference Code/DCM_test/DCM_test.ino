// OTCarTest
// A simple test program for OTCar, will make the OTCar move forward, backward, 
// turn right and turn left for specific seconds.
// http://www.osslab.com.tw/Hardware/Open_Embedded_System/MCU/AVR/Arduino/Remote_Car

/* Input for motorA:
   IN1      IN2      Action
   LOW      LOW      Motor Stop
   HIGH     LOW      Motor moves forward  
   LOW      HIGH     Motor moves backward
   HIGH     HIGH     Motor Stop
*/   
const int motorIn1 = 3;
const int motorIn2 = 4;
const int motorIn3 = 9;      
const int motorIn4 = 10;       

const int DELAY = 1000;

void setup()  
{
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorIn3, OUTPUT);
  pinMode(motorIn4, OUTPUT);    
}

void loop()
{
  selfTest();
}

void selfTest()
{
  forward();
  delay(DELAY);
  motorstop(); delay(500);
  
  backward();
  delay(DELAY);
  motorstop(); delay(500);
  
  right();
  delay(DELAY);
  motorstop(); delay(500);
  
  left();
  delay(DELAY);
  motorstop(); delay(500);  
}

void motorstop()
{
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorIn3, LOW);
  digitalWrite(motorIn4, LOW);
}

void forward()
{
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorIn3, HIGH);
  digitalWrite(motorIn4, LOW);
}

void backward()
{
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
  digitalWrite(motorIn3, LOW);
  digitalWrite(motorIn4, HIGH);
}

// Let right motor keep running, but stop left motor
void right()
{
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorIn3, LOW);
  digitalWrite(motorIn4, LOW);
}

// Let left motor keep running, but stop right motor
void left()
{
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorIn3, HIGH);
  digitalWrite(motorIn4, LOW);
}
