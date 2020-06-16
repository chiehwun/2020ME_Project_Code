#include "GOGO_XY.h"
#include "math.h"
#define DEBUG
// Scara Parameters
const double L1 = 26.0, L2 = 32.66;
const double offset1 = 63.0, offset2 = 150.0;
double TH1 = 0.0, TH2 = 0.0;
const double X_MAX = 50.0, X_MIN = -50.0, Y_MAX = 55.0, Y_MIN = -20.0;
double X = 17.5, Y = 17.3, dx = 0.0, dy = 0.0;
double E = acos((X*X + Y*Y - L1*L1 - L2*L2)/2/L1/L2),
       Q = acos((X*X + Y*Y + L1*L1 - L2*L2)/2/L1/sqrt(X*X + Y*Y)),
       S = atan2(Y,X) - Q;
double scara_speed = 1.2;
// Basket Parameters
// B1: ARM1 = 114, ARM2 = 18, ARM3 = 50
// B2: ARM1 = 91,  ARM2 = 13, ARM3 = 14
const int bktdeg = 45, bkt1 = 180, bkt2 = 40;
const int BSK_ARM[2][3] = {{114,18,50},{91,13,14}};
bool bsk_state[2] = {false, false};   // 歸零狀態

// Moving Speed Parameters
const int HIGHSPD = 255, LOWSPD = 225;
int lspd = 0, rspd = 0, zspd = 0, mvspd = HIGHSPD;

// Servo Parameters
Servo sv[13]; // Servo pin 4-12
double svdeg[13]        = {-1.0,  -1.0,   0.0,    0.0,    0.0,    bkt1,     bkt2,     62.0,   18.0,   0.0,    5.0,    88.0,   0.0};
const int svdeg_MIN[13] = {0.0,   0.0,    0.0,    0.0,    0.0,    0.0,      0.0,      0.0,    0.0,    0.0,    5.0,    0.0,    0.0};
const int svdeg_MAX[13] = {180.0, 180.0,  180.0,  180.0,  180.0,  bktdeg,   bktdeg,   117.0,   150.0,  180.0,  80.0,  180.0,  180.0};
double dd10[13]         = {0.0};
const int pulse_min[13] = {544,   544,    544,    544,    544,    544,      544,      570,    470,    544,    544,    544,    544};
const int pulse_max[13] = {2400,  2400,   2400,   2400,   2400,   2400,     2400,     1950,   2500,   2400,   2400,   2400,   2400};
const double SCARA_MAX = 1.0, SCARA_MIN = 0.5;
double scara_spd = SCARA_MAX;
const int SCARA_SAVEDEG = 5;
bool exit_scara = false;

bool atBkt(){
  return (InBound(svdeg[PARM1], BSK_ARM[0][0], 2) && InBound(svdeg[PARM2], BSK_ARM[0][1], 2)) || 
         (InBound(svdeg[PARM1], BSK_ARM[1][0], 2) && InBound(svdeg[PARM2], BSK_ARM[1][1], 2));
}

void Theta2XY(){
    TH1 = DEG_TO_RAD * (svdeg[PARM1] + offset1);
    TH2 = TH1 + DEG_TO_RAD * (svdeg[PARM2] - offset2);
    X = L1 * cos(TH1) + L2 * cos(TH2);
    Y = L1 * sin(TH1) + L2 * sin(TH2);
}

void BSK(int n){
  all_stop();
  while(1){
    ps2x.read_gamepad(false, vibrate);
    if(ps2x.BP(PSB_GREEN) || ps2x.BP(PSB_PINK)) {
      exit_scara = true;
      break;
    }
    double di = -sign(svdeg[PARM1] - BSK_ARM[n][0])*scara_speed;
    double dj = -sign(svdeg[PARM2] - BSK_ARM[n][1])*scara_speed;
    double dk = -sign(svdeg[PARM3] - BSK_ARM[n][2])*scara_speed;
    bool ok = true;
    if(abs((int)svdeg[PARM1] - BSK_ARM[n][0]) <= 5) {
      svdeg[PARM1] = BSK_ARM[n][0];
    }
    else {
      svdeg[PARM1] = constrain(svdeg[PARM1] + di, svdeg_MIN[PARM1], svdeg_MAX[PARM1]);
      sv[PARM1].write(svdeg[PARM1]);
      Serial.print("F1/");
      ok = false;
    }
    if(abs((int)svdeg[PARM2] - BSK_ARM[n][1]) <= 5) {
      svdeg[PARM2] = BSK_ARM[n][1];
    }
    else {
      svdeg[PARM2] = constrain(svdeg[PARM2] + dj, svdeg_MIN[PARM2], svdeg_MAX[PARM2]);
      sv[PARM2].write(svdeg[PARM2]);
      ok = false;
      Serial.print("F2/");
    }
    if(abs((int)svdeg[PARM3] - BSK_ARM[n][2]) <= 5) {
        svdeg[PARM3] = BSK_ARM[n][2];
    }
    else {
      svdeg[PARM3] = constrain(svdeg[PARM3] + dk, svdeg_MIN[PARM3], svdeg_MAX[PARM3]);
      sv[PARM3].write(svdeg[PARM3]);
      ok = false;
      Serial.print("F3/ ");
    }
    if(ok){
      break;
    }
    Serial.print("m1: ");       Serial.print(svdeg[PARM1]);
    Serial.print(", m2: ");     Serial.print(svdeg[PARM2]);
    Serial.print(", m3: ");     Serial.println(svdeg[PARM3]);
  }
  Theta2XY();
}

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
    sv[i].attach(i, pulse_min[i], pulse_max[i]);
    sv[i].write(svdeg[i]);
  }
  wait(2000);
  sv[PBSKT1].detach();
  sv[PBSKT2].detach();
  wait(500);
  Theta2XY();
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

  if(ps2x.BP(PSB_R3))
    if(scara_spd == SCARA_MIN) {
      Serial.println("SCARA_HIGHSPEED");
      scara_spd = SCARA_MAX;
    }
    else {
      Serial.println("SCARA_LOWSPEED");
      scara_spd = SCARA_MIN;
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
    Serial.println("BP_PSB_R1... ");
    basket_state(0);
//    basket_mv(PBSKT1);
//    Serial.println("PBSKT1 detached!");
  }
  if(ps2x.BP(PSB_R2)) {
    Serial.println("BP_PSB_R2... ");
    basket_state(1);
//    basket_mv(PBSKT2);
//    Serial.println("PBSKT2 detached!");
  }
  // GRIPPER
  if(ps2x.BP(PSB_RED))      dd10[PGRP] = 2.0;
  if(ps2x.BR(PSB_RED))      dd10[PGRP] = 0.0;
  if(ps2x.BP(PSB_BLUE)) {
    dd10[PGRP] = -1.5;
    if(atBkt()) {
      Serial.print("PPP");
      sv[PBSKT1].attach(PBSKT1);
      sv[PBSKT1].write(bkt1);
      sv[PBSKT2].attach(PBSKT2);
      sv[PBSKT2].write(bkt2);
    }
  }
  if(ps2x.BR(PSB_BLUE)) {
    dd10[PGRP] = 0.0;
    if(atBkt()) {
      Serial.print("RRR");
      sv[PBSKT1].detach();
      sv[PBSKT2].detach();
    }
  }

  // SCARA XY conrol
  if(ps2x.BP(PSB_PAD_LEFT))   dx = -scara_spd;
  if(ps2x.BR(PSB_PAD_LEFT))   dx = 0.0;
  if(ps2x.BP(PSB_PAD_RIGHT))  dx = scara_spd;
  if(ps2x.BR(PSB_PAD_RIGHT))  dx = 0.0;
  if(ps2x.BP(PSB_PAD_UP))     dy = scara_spd;
  if(ps2x.BR(PSB_PAD_UP))     dy = 0.0;
  if(ps2x.BP(PSB_PAD_DOWN))   dy = -scara_spd;
  if(ps2x.BR(PSB_PAD_DOWN))   dy = 0.0;
  exit_scara = false;
  if(ps2x.BP(PSB_GREEN))      BSK(0);
  if(exit_scara)  return;
  if(ps2x.BP(PSB_PINK))       BSK(1);
  if(exit_scara)  return;

  // ARM3
  if(ps2x.BP(PSB_START))      dd10[PARM3] = 1.0;
  if(ps2x.BR(PSB_START))      dd10[PARM3] = 0.0;
  if(ps2x.BP(PSB_SELECT))     dd10[PARM3] = -1.0;
  if(ps2x.BR(PSB_SELECT))     dd10[PARM3] = 0.0;

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
//      svdeg[i] = constr1ain(svdeg[i] + dd10[i], svdeg_MIN[i], svdeg_MAX[i]);
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

void basket_state(int code) {
  int port = (code == 0? PBSKT1:PBSKT2);
  all_stop();
  sv[port].attach(port);
  if(bsk_state[code]) {
    sv[port].write(port == PBSKT1? bkt1 : bkt2);  // restore
    bsk_state[code] = false;
  }
  else {
    sv[port].write(port == PBSKT1? bkt1 - bktdeg : bkt2 + bktdeg);  // pour
    bsk_state[code] = true;
  }
  wait(200);
  sv[port].detach();
}

void all_stop() {
  lspd = rspd = zspd = 0;
  DCM_set(0, 0);
  DCM_set(1, 0);
  DCM_set(2, 0);
}

void scara() {
  double e,q,s, x = X+dx, y = Y+dy;
  if(pow(x,2)+pow(y,2) > (L1+L2)*(L1+L2)) {
    Serial.println("X,Y Out of Range!!");
    return;
  }
  e = acos((x*x + y*y - L1*L1 - L2*L2)/2/L1/L2);
  q = acos((x*x + y*y + L1*L1 - L2*L2)/2/L1/sqrt(x*x + y*y));
  s = atan2(y,x) - q;
  double th1 = s + 2*q, th2 = th1 - e;
  if(isnan(e) || isnan(q) || isnan(s)) {
    Serial.println("NAN");
    return;
  }
  if(fabs(TH1 - th1) > DEG_TO_RAD * (SCARA_SAVEDEG)) {
    Serial.print("delta_TH1 Exceed!! = "); Serial.println(RAD_TO_DEG*(TH1 - th1));
    Serial.print("x: ");    Serial.print(x);    Serial.print(", y: ");    Serial.println(y);
    Serial.print("e: ");    Serial.print(RAD_TO_DEG * e);    Serial.print(", q: ");    Serial.print(q); Serial.print(", s: ");    Serial.println(s);
    Serial.print("TH1: ");  Serial.print(TH1);  Serial.print(", th1: "); Serial.println(th1);
    return;
  }
  if(fabs(TH2 - th2) > DEG_TO_RAD * (SCARA_SAVEDEG)) {
    Serial.println("delta_TH2 Exceed!! = "); Serial.println(RAD_TO_DEG*(TH2 - th2));
    return;
  }
  if(e < 0) {
    Serial.println("TH1 > TH2 !!");
    return;
  }

  if(outRange(RAD_TO_DEG * th1 - offset1, svdeg_MIN[PARM1], svdeg_MAX[PARM1])) {
    Serial.println("m1 Out of Range !!");
    return;
  }
  if(outRange(RAD_TO_DEG * th2 + offset2 - RAD_TO_DEG * th1, svdeg_MIN[PARM2], svdeg_MAX[PARM2])) {
    Serial.println("m2 Out of Range !!");
    return;
  }
  X = constrain(x, X_MIN, X_MAX);
  Y = constrain(y, Y_MIN, Y_MAX);
  E = e; Q = q; S = s; TH1 = th1; TH2 = th2;
  svdeg[PARM1] = RAD_TO_DEG * th1 - offset1;
  svdeg[PARM2] = RAD_TO_DEG * th2 + offset2 - RAD_TO_DEG * th1;
  Serial.print("X: ");     Serial.print(X);
  Serial.print(", Y:");    Serial.print(Y);
  Serial.print(", m1: ");  Serial.print(svdeg[PARM1]);
  Serial.print(", m2: ");  Serial.print(svdeg[PARM2]);
  Serial.print(", E: ");   Serial.print(RAD_TO_DEG * E);
  Serial.print(", S: ");   Serial.print(RAD_TO_DEG * S);
  Serial.print(", Q: ");   Serial.println(RAD_TO_DEG * Q);
}
