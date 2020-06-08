#include "GOGO.h"
#include "math.h"
// Scara Parameters
const double L1 = 26.0, L2 = 32.66;
const double offset1 = 55.0, offset2 = 80.0;
double th1 = 0.0, th2 = 0.0;
const double X_MAX = 29.0, X_MIN = 0.0, Y_MAX = 55.0, Y_MIN = 10.0;
double X = 20.58, Y = -10.87, dx = 0.0, dy = 0.0;

// Basket Parameters
const int bktdeg = 30;

int lspd = 0, rspd = 0, zspd = 0;
const int mvspd = 255;

// Servo Parameters
Servo sv[12]; // Servo pin 4-12
int svdeg[12]           = {-1, -1,    0,    0,    0,    180,    0,      0,    0,    90,   5,    0};
const int svdeg_MIN[12] = {0,   0,    0,    0,    0,    0,      0,      0,    0,    0,    5,    0};
const int svdeg_MAX[12] = {180, 180,  180,  180,  180,  bktdeg, bktdeg, 95,   180,  180,  180,  180};
int dd10[12]            = {0  , 0,    0,    0,    0,    0,      0,      0,    0,    0,    0,    0};

void setup() {
  // warn Light on
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);

  Serial.begin(9600);
  PS2_set();  // After PS2_set(), use wait() instead delay()
  // warn Light off
  digitalWrite(LED_BUILTIN, LOW);
//  delay(500);
  // DC motor init
  for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 3; ++j)
      pinMode(dcm[i][j], OUTPUT);
  // Servo motor init
  for(int i = PBSKT1; i <= PTESTS; ++i) {
    sv[i].attach(i);
    sv[i].write(svdeg[i]);
  }
  wait(2000);
//  sv[PBSKT1].detach();
//  sv[PBSKT2].detach();
  wait(500);
}

void loop() {
  ps2x.read_gamepad(false, vibrate);
  // move robot
  lspd = map(ps2x.Analog(PSS_LY),0,255,mvspd,-mvspd);
  lspd = abs(lspd) < 20? 0 : lspd;
  rspd = map(ps2x.Analog(PSS_RY),0,255,-mvspd,mvspd);
  rspd = abs(rspd) < 20? 0 : rspd;
  // move Z
  if(ps2x.BP(PSB_L1)) zspd = mvspd;
  if(ps2x.BR(PSB_L1)) zspd = 0;
  if(ps2x.BP(PSB_L2)) zspd = -mvspd;
  if(ps2x.BR(PSB_L2)) zspd = 0;

  // Basket
  if(ps2x.BP(PSB_R1)) {
    Serial.print("BP_PSB_R1... ");
    basket_mv(PBSKT1);
    Serial.println("PBSKT1 detached!");
  }
  if(ps2x.BP(PSB_R2)) {
    Serial.println("BP_PSB_R2... ");
    basket_mv(PBSKT2);
    Serial.println("PBSKT2 detached!");
  }
  // GRIPPER
  if(ps2x.BP(PSB_RED))        dd10[PGRP] = 1;
  if(ps2x.BR(PSB_RED))        dd10[PGRP] = 0;
  if(ps2x.BP(PSB_BLUE))       dd10[PGRP] = -1;
  if(ps2x.BR(PSB_BLUE))       dd10[PGRP] = 0;

  // SCARA XY conrol
  if(ps2x.BP(PSB_PAD_LEFT))   dx = -1;
  if(ps2x.BR(PSB_PAD_LEFT))   dx = 0;
  if(ps2x.BP(PSB_PAD_RIGHT))  dx = 1;
  if(ps2x.BR(PSB_PAD_RIGHT))  dx = 0;
  if(ps2x.BP(PSB_PAD_UP))     dy = 1;
  if(ps2x.BR(PSB_PAD_UP))     dy = 0;
  if(ps2x.BP(PSB_PAD_DOWN))   dy = -1;
  if(ps2x.BR(PSB_PAD_DOWN))   dy = 0;

  // ARM3
  if(ps2x.BP(PSB_START))      dd10[PARM3] = 1;
  if(ps2x.BR(PSB_START))      dd10[PARM3] = 0;
  if(ps2x.BP(PSB_SELECT))     dd10[PARM3] = -1;
  if(ps2x.BR(PSB_SELECT))     dd10[PARM3] = 0;

  // Test ARM1 ARM2
  if(ps2x.BP(PSB_PAD_UP))     dd10[PARM1] = 1;
  if(ps2x.BR(PSB_PAD_UP))     dd10[PARM1] = 0;
  if(ps2x.BP(PSB_PAD_DOWN))   dd10[PARM1] = -1;
  if(ps2x.BR(PSB_PAD_DOWN))   dd10[PARM1] = 0;
  if(ps2x.BP(PSB_PAD_LEFT))   dd10[PARM2] = -1;
  if(ps2x.BR(PSB_PAD_LEFT))   dd10[PARM2] = 0;
  if(ps2x.BP(PSB_PAD_RIGHT))  dd10[PARM2] = 1;
  if(ps2x.BR(PSB_PAD_RIGHT))  dd10[PARM2] = 0;

  // Test Pin
  if(ps2x.BP(PSB_PINK))  dd10[PTESTS] = 1;
  if(ps2x.BR(PSB_PINK))  dd10[PTESTS] = 0;
  if(ps2x.BP(PSB_GREEN)) dd10[PTESTS] = -1;
  if(ps2x.BR(PSB_GREEN)) dd10[PTESTS] = 0;
  keep();
  delay(10);
}

void keep() {
  if(dd10[PARM1] != 0 || dd10[PARM2] != 0)
    scara();
  // DCM ACT
  DCM_set(0, lspd);
  DCM_set(1, rspd);
  DCM_set(2, zspd);
  // SVM ACT
  for(int i = PARM1; i <= PTESTS; i++) {
    svdeg[i] = constrain(svdeg[i] + dd10[i], svdeg_MIN[i], svdeg_MAX[i]);
    sv[i].write(svdeg[i]);
    // Serial.print("svdeg[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(svdeg[i]);
  }
//  Serial.print("ARM1: ");
//  Serial.print(svdeg[PARM1]);
//  Serial.print(", ARM2: ");
//  Serial.print(svdeg[PARM1]);
//  Serial.print(", TEST:");
//  Serial.println(svdeg[PTESTS]);
}

void basket_mv(int port) {
  sv[port].attach(port);
  warn();
  sv[port].write(port == PBSKT1? 180 - bktdeg : bktdeg);
  wait(2000);
  sv[port].write(port == PBSKT1? 180 : 0);
  wait(1000);
  sv[port].detach();
}

void scara() {
  double E,Q,S,e,q,s;
  X = constrain(X+dx, X_MIN, X_MAX);
  Y = constrain(Y+dy, Y_MIN, Y_MAX);
  e = acos((pow(X, 2) + pow(Y, 2) - pow(L1, 2) - pow(L2, 2))/2/L1/L2);
  q = acos((pow(X, 2) + pow(Y, 2) + pow(L1, 2) - pow(L2, 2))/2/L1/sqrt(pow(X,2) + pow(Y,2)));
  s = atan(Y/X) - Q;
  if(isnan(e) || isnan(q) || isnan(s)){
    Serial.println("NAN");
    return;
  }
  else {
    E = e;
    Q = q;
    S = s;
  }
  th1 = S + 2 * Q;
  th2 = th1 - E;
  Serial.print("X: ");     Serial.print(X);
  Serial.print(", Y:");    Serial.print(Y);
  Serial.print(", m1: ");  Serial.print(rad2deg(th1) - offset1);
  Serial.print(", m2: ");  Serial.print(rad2deg(th2)+offset2-rad2deg(th1)+offset1);
  Serial.print(", E: ");   Serial.print(rad2deg(E));
  Serial.print(", S: ");   Serial.print(rad2deg(S));
  Serial.print(", Q: ");   Serial.println(rad2deg(Q));
}
