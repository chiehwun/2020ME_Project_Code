#include <Servo.h>
#define MVSP 255    // DCM moving speed
Servo myservo;      // SVM
int pos = 0;
int dcm[][3] = {{2, 22, 23},{3, 24, 25}};
void spset(byte motor, int val) { 
  if(val > 255 || val < -255) return;
  digitalWrite(dcm[motor][1], val > 0);
  digitalWrite(dcm[motor][2], val < 0);
  if(val < 0) val = -val;
  analogWrite(dcm[motor][0], val);
}
void setup() {
//  myservo.attach(2);
  for(int i=0; i < 2; i++) {
    for(int j=0; j < 3; j++) {
      pinMode(dcm[i][j], OUTPUT);
    }
  }
}

void loop() {
  spset(0,255);
  spset(1,255);
  delay(5000);
  
//  for(int i=0; i < 256; i++) {
//    spset(0,i);
//    delay(50);
//  }
//  delay(2000);
//    for(int i=255; i >= 0; i--) {
//    spset(0,i);
//    delay(50);
//  }
  
//  for(pos = 0; pos < 180; pos += 1) {
//    myservo.write(pos);
//    delay(5);
//  }
//  for(pos = 180; pos>=1; pos-=1) {
//    myservo.write(pos);
//    delay(5);
//  }
}
