/*Example sketch to control a stepper motor with A4988 stepper motor driver and Arduino without a library. More info: https://www.makerguides.com */

// Define stepper motor connections and steps per revolution:
#define dirPin 22
#define stepPin 23
#define stepsPerRevolution 200
int ddh = 100, ddl = 4900;
void setup() {
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  // Set the spinning direction clockwise:
  digitalWrite(dirPin, HIGH);

  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < stepsPerRevolution; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    digitalWrite(13, HIGH);
    delayMicroseconds(ddh);
    digitalWrite(stepPin, LOW);
    digitalWrite(13, LOW);
    delayMicroseconds(ddl);
  }

//  delay(1000);
//
//  // Set the spinning direction counterclockwise:
//  digitalWrite(dirPin, LOW);
//
//  // Spin the stepper motor 1 revolution quickly:
//  for (int i = 0; i < stepsPerRevolution; i++) {
//    // These four lines result in 1 step:
//    digitalWrite(stepPin, HIGH);
//    digitalWrite(13, HIGH);
//    delayMicroseconds(ddm);
//    digitalWrite(stepPin, LOW);
//    digitalWrite(13, LOW);
//    delayMicroseconds(ddm);
////    delay(del);
//  }
//  delay(1000);
//
//  // Set the spinning direction clockwise:
//  digitalWrite(dirPin, HIGH);

//  // Spin the stepper motor 5 revolutions fast:
//  for (int i = 0; i < 5 * stepsPerRevolution; i++) {
//    // These four lines result in 1 step:
//    digitalWrite(stepPin, HIGH);
//    delayMicroseconds(500000);
//    digitalWrite(stepPin, LOW);
//    delayMicroseconds(500000);
//  }
//
//  delay(1000);
//
//  // Set the spinning direction counterclockwise:
//  digitalWrite(dirPin, LOW);
//
//  //Spin the stepper motor 5 revolutions fast:
//  for (int i = 0; i < 5 * stepsPerRevolution; i++) {
//    // These four lines result in 1 step:
//    digitalWrite(stepPin, HIGH);
//    delayMicroseconds(500000);
//    digitalWrite(stepPin, LOW);
//    delayMicroseconds(500000);
//  }
//
//  delay(1000);
}
