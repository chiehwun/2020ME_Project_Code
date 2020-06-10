#include "GOGO_XY.h"
#include "math.h"
#define DEBUG
// Scara Parameters
const double L1 = 26.0, L2 = 32.66;
const double offset1 = 78.0, offset2 = 155.0;
double th1 = 0.0, th2 = 0.0;
const double X_MAX = 50.0, X_MIN = -50.0, Y_MAX = 55.0, Y_MIN = -20.0;
double X = 15.77, Y = 10.12, dx = 0.0, dy = 0.0;
double E = 0.0, Q = 0.0, S = 0.0;

// Basket Parameters
const int bktdeg = 30, bkt1 = 150, bkt2 = 50;

// Moving Speed Parameters
const int HIGHSPD = 255, LOWSPD = 225;
int lspd = 0, rspd = 0, zspd = 0, mvspd = HIGHSPD;

// Servo Parameters
Servo sv[12]; // Servo pin 4-12
double svdeg[12]        = {-1.0,  -1.0,   0.0,    0.0,    0.0,    bkt1,    bkt2,      47.0,   10.0,   88.0,   5.0,    0.0};
const int svdeg_MIN[12] = {0.0,   0.0,    0.0,    0.0,    0.0,    0.0,      0.0,      0.0,    0.0,    0.0,    5.0,    0.0};
const int svdeg_MAX[12] = {180.0, 180.0,  180.0,  180.0,  180.0,  bktdeg,   bktdeg,   95.0,   180.0,  180.0,  180.0,  180.0};
double dd10[12]         = {0.0};

void setup() {
  // warn Light on
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);

  Serial.begin(9600);
  PS2_set();  // After PS2_set(), use wait() instead delay()
  // warn Light off
  digitalWrite(LED_BUILTIN, LOW);

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
  sv[PBSKT1].detach();
  sv[PBSKT2].detach();
  wait(500);
}

void loop() {
  ps2x.read_gamepad(false, vibrate);
  // move robot
  if(ps2x.BP(PSB_L3))
    if(mvspd == LOWSPD) {
      Serial.println("HIGHSPEED");
      mvspd = HIGHSPD;
    }
    else {
      Serial.println("LOWSPEED");
      mvspd = LOWSPD;
    }

  lspd = map(ps2x.Analog(PSS_LY),0,255,mvspd,-mvspd);
  lspd = abs(lspd) < 100? 0 : lspd;
  rspd = map(ps2x.Analog(PSS_RY),0,255,-mvspd,mvspd);
  rspd = abs(rspd) < 100? 0 : rspd;
  // move Z
  if(ps2x.BP(PSB_L1)) zspd = HIGHSPD;
  if(ps2x.BR(PSB_L1)) zspd = 0;
  if(ps2x.BP(PSB_L2)) zspd = -HIGHSPD;
  if(ps2x.BR(PSB_L2)) zspd = 0;

  // Basket
  if(ps2x.BP(PSB_R1)) {
    Serial.print("BP_PSB_R1... ");
    basket_mv(PBSKT1);
    Serial.println("PBSKT1 detached!");
  }
  if(ps2x.BP(PSB_R2)) {
    Serial.print("BP_PSB_R2... ");
    basket_mv(PBSKT2);
    Serial.println("PBSKT2 detached!");
  }
  // GRIPPER
  if(ps2x.BP(PSB_RED))        dd10[PGRP] = 1.0;
  if(ps2x.BR(PSB_RED))        dd10[PGRP] = 0.0;
  if(ps2x.BP(PSB_BLUE))       dd10[PGRP] = -1.0;
  if(ps2x.BR(PSB_BLUE))       dd10[PGRP] = 0.0;

  // SCARA XY conrol
  if(ps2x.BP(PSB_PAD_LEFT))   dx = -0.3;
  if(ps2x.BR(PSB_PAD_LEFT))   dx = 0.0;
  if(ps2x.BP(PSB_PAD_RIGHT))  dx = 0.3;
  if(ps2x.BR(PSB_PAD_RIGHT))  dx = 0.0;
  if(ps2x.BP(PSB_PAD_UP))     dy = 0.3;
  if(ps2x.BR(PSB_PAD_UP))     dy = 0.0;
  if(ps2x.BP(PSB_PAD_DOWN))   dy = -0.3;
  if(ps2x.BR(PSB_PAD_DOWN))   dy = 0.0;

  // Test ARM1 ARM2
//  if(ps2x.BP(PSB_PAD_UP))     dd10[PARM1] = 0.1;
//  if(ps2x.BR(PSB_PAD_UP))     dd10[PARM1] = 0.0;
//  if(ps2x.BP(PSB_PAD_DOWN))   dd10[PARM1] = -0.1;
//  if(ps2x.BR(PSB_PAD_DOWN))   dd10[PARM1] = 0.0;
//  if(ps2x.BP(PSB_PAD_LEFT))   dd10[PARM2] = -0.4;
//  if(ps2x.BR(PSB_PAD_LEFT))   dd10[PARM2] = 0.0;
//  if(ps2x.BP(PSB_PAD_RIGHT))  dd10[PARM2] = 0.4;
//  if(ps2x.BR(PSB_PAD_RIGHT))  dd10[PARM2] = 0.0;

  // ARM3
  if(ps2x.BP(PSB_START))      dd10[PARM3] = 1.0;
  if(ps2x.BR(PSB_START))      dd10[PARM3] = 0.0;
  if(ps2x.BP(PSB_SELECT))     dd10[PARM3] = -1.0;
  if(ps2x.BR(PSB_SELECT))     dd10[PARM3] = 0.0;

  // Test Pin
  if(ps2x.BP(PSB_PINK))       dd10[PTESTS] = 1.0;
  if(ps2x.BR(PSB_PINK))       dd10[PTESTS] = 0.0;
  if(ps2x.BP(PSB_GREEN))      dd10[PTESTS] = -1.0;
  if(ps2x.BR(PSB_GREEN))      dd10[PTESTS] = 0.0;
  keep();
  delay(10);
}

void keep() {
  if(dx != 0 || dy != 0)
    scara();
  // DCM ACT
  DCM_set(0, lspd);
  DCM_set(1, rspd);
  DCM_set(2, zspd);
  if(lspd != 0 || rspd != 0 || zspd != 0) {
    Serial.print("L: ");
    Serial.print(lspd);
    Serial.print(", R: ");
    Serial.print(rspd);
    Serial.print(", Z: ");
    Serial.println(zspd);
  }
  // SVM ACT
  for(int i = PARM1; i <= PTESTS; i++) {
    if(i == PARM1 || i == PARM2)
      svdeg[i] = constrain(svdeg[i], svdeg_MIN[i], svdeg_MAX[i]);
    else
      svdeg[i] = constrain(svdeg[i] + dd10[i], svdeg_MIN[i], svdeg_MAX[i]);
    sv[i].write(int(svdeg[i]));
    if(dd10[i] != 0) {
      Serial.print("serv[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(svdeg[i]);
    }
  }
//  Serial.print("ARM1: ");
//  Serial.print(svdeg[PARM1]);
//  Serial.print(", ARM2: ");
//  Serial.print(svdeg[PARM1]);
//  Serial.print(", TEST:");
//  Serial.println(svdeg[PTESTS]);
}

void basket_mv(int port) {
  all_stop();
  sv[port].attach(port);
  warn();
  sv[port].write(port == PBSKT1? bkt1 - bktdeg : bkt2 + bktdeg);
  wait(2000);
  sv[port].write(port == PBSKT1? bkt1 : bkt2);
  wait(500);
  sv[port].detach();
}

void all_stop() {
  DCM_set(0, 0);
  DCM_set(1, 0);
  DCM_set(2, 0);
}

void scara() {
  double e,q,s;
  X = constrain(X+dx, X_MIN, X_MAX);
  Y = constrain(Y+dy, Y_MIN, Y_MAX);
  e = acos((pow(X, 2) + pow(Y, 2) - pow(L1, 2) - pow(L2, 2))/2/L1/L2);
  q = acos((pow(X, 2) + pow(Y, 2) + pow(L1, 2) - pow(L2, 2))/2/L1/sqrt(pow(X,2) + pow(Y,2)));
  s = atan2(Y,X) - Q;
  if(isnan(e) || isnan(q) || isnan(s)){
    Serial.println("NAN");
    return;
  }
  if(abs(th1 - (s + 2*q)) > 5) {
    Serial.println("delta_th1 > 5!!");
    return;
  }
  if(abs(th2 - (s + 2*q - e)) > 5) {
    Serial.println("delta_th2 > 5!!");
    return;
  }
  if(e < 0) {
    Serial.println("th1 > th2 !!");
    return;
  }
//  if((s + 2*q) > 183 || (s + 2*q) < offset1) {
//    Serial.println("th1 OutOfRange!!");
//    return;
//  }
  
  E = e;
  Q = q;
  S = s;
  th1 = S + 2*Q;
  th2 = th1 - E;
  svdeg[PARM1] = rad2deg(th1) - offset1;
  svdeg[PARM2] = rad2deg(th2)+offset2-rad2deg(th1);
  Serial.print("X: ");     Serial.print(X);
  Serial.print(", Y:");    Serial.print(Y);
  Serial.print(", m1: ");  Serial.print(svdeg[PARM1]);
  Serial.print(", m2: ");  Serial.print(svdeg[PARM2]);
  Serial.print(", E: ");   Serial.print(rad2deg(E));
  Serial.print(", S: ");   Serial.print(rad2deg(S));
  Serial.print(", Q: ");   Serial.println(rad2deg(Q));
}
