#include "GOGO.h"
#include "math.h"
// Scara Parameters
double L1 = 10.0, L2 = 10.0;
double th1 = 0.0, th2 = 0.0;
double X = 0.0, Y = 0.0;

byte vibrate = 0;
int lspd = 0, rspd = 0, zspd = 0, mvspd = 255;
Servo sv[12]; // Servo pin 4-12
int svdeg[12]     = {-1, -1,    0,    0,    0,    0,    0,    0,    0,    90,   5,    0};
int svdeg_MIN[12] = {0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    5,    0};
int svdeg_MAX[12] = {180, 180,  180,  180,  180,  180,  180,  180,  180,  180,  180,  180};
int dd10[12] =      {0  , 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0};

bool R1 = false, R2 = false;

void keep() {
  DCM_set(0, lspd);
  DCM_set(1, rspd);
  DCM_set(2, zspd);
  if(R1)  basket_mv(PBSKT1);
  if(R2)  basket_mv(PBSKT2);
  for(int i = PARM1; i <= PTESTS; i++) {
    Serial.print(i);
    svdeg[i] += dd10[i];
    svdeg[i] = constrain(svdeg[i], 0, 180);
    sv[i].write(svdeg[i]);
    // Serial.print("svdeg[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(svdeg[i]);
  }
}

void setup() {
  // warn Light
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(500);

  Serial.begin(9600);
  PS2_set();
  // warn Light
  digitalWrite(13, LOW);
  delay(500);
  // DC motor init
  for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 3; ++j)
      pinMode(dcm[i][j], OUTPUT);
  // Servo motor init
  for(int i = PARM1; i <= PTESTS; ++i) {
    sv[i].attach(i);
    svdeg[i] = constrain(svdeg[i], svdeg_MIN[i], svdeg_MAX[i]);
    sv[i].write(svdeg[i]);
  }
}

void loop() {
  ps2x.read_gamepad(false, vibrate);
  lspd = map(ps2x.Analog(PSS_LY),0,255,mvspd,-mvspd);
  rspd = map(ps2x.Analog(PSS_RY),0,255,-mvspd,mvspd);
  // move Z
  if(ps2x.BP(PSB_L1)) zspd = mvspd;
  if(ps2x.BR(PSB_L1)) zspd = 0;
  if(ps2x.BP(PSB_L2)) zspd = -mvspd;
  if(ps2x.BR(PSB_L2)) zspd = 0;
  // Basket
//  R1 = ps2x.Button(PSB_R1);
  if(ps2x.BP(PSB_R1) && !R1) {
    Serial.println("");
    Serial.println("R1");
    R1 = true;
    //basket_mv(PBSKT1);
  }
  if(ps2x.BR(PSB_R1)) R1 = false;
  if(ps2x.BP(PSB_R2) && !R2) {
    Serial.println("");
    Serial.println("R2");
    R2 = true;
    //basket_mv(PBSKT2);
  }
  if(ps2x.BR(PSB_R2)) R2 = false;
  
  if(ps2x.BP(PSB_RED))    dd10[PGRP] = 1;
  if(ps2x.BR(PSB_RED))    dd10[PGRP] = 0;
  if(ps2x.BP(PSB_BLUE))   dd10[PGRP] = -1;
  if(ps2x.BR(PSB_BLUE))   dd10[PGRP] = 0;
  // ARM3
  if(ps2x.BP(PSB_START))  dd10[PARM3] = 1;
  if(ps2x.BR(PSB_START))  dd10[PARM3] = 0;
  if(ps2x.BP(PSB_SELECT)) dd10[PARM3] = -1;
  if(ps2x.BR(PSB_SELECT)) dd10[PARM3] = 0;

  keep();
  delay(10);
}

void basket_mv(int port) {
  warn();
  sv[port].attach(port);
  warn();
  sv[port].write(150);
  delay(1000);
  sv[port].write(0);
  delay(1000);
  R1 = false;
  R2 = false;
//  sv[port].detach();
//  Serial.println(port);
}
