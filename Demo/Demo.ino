#include <PS2X_lib.h>
#include <Servo.h>

#define PDAT 1
#define PCMD 2
#define PSEL 3
#define PCLK 4

#define BP          ButtonPressed
#define BR          ButtonReleased

Servo sv[13];
PS2X ps;
unsigned long nxt = 0;
int ss[13] = {};
int dd10 = 0, dd12 = 0, mvsp = 255, x = 3, y = 3;

bool is_L3 = 0, is_R3 = 0, is_sel = 0, is_L1 = 0, is_L2 = 0, is_up = 0, is_dw = 0;

int mp[][3] = {{4, 30, 32},{3, 26, 28},{5, 34, 36},{2, 22, 24}};
int sini[5] = {76, 42, 5, 90, 150};
int xy2deg8[][6] = {
  -1, 33, 47, 54, -1, -1,
  25, 40, 52, 63, -1, -1,
  36, 48, 64, 69, 80, 92,
  51, 60, 68, 76, 86, 97,
  68, 73, 78, 85, 93, 104,
  -1, -1, 89, 94, 101, 111,
  -1, -1, 100, 104, 110, 120};
int xy2deg9[][6] = {
  -1, 66, 66, 82, -1, -1,
  57, 55, 56, 60, -1, -1,
  42, 42, 40, 51, 58, 67,
  26, 30, 36, 42, 51, 61,
  20, 19, 27, 35, 44, 55,
  -1, -1, 19, 29, 38, 50,
  -1, -1, 13, 24, 34, 40};


int bd_deg(int val, int loo, int upp) {
  if(val > upp) return upp;
  else if(val < loo) return loo;
  else return val;
}

void spset(byte motor, int val) {
  if(val > 255 || val < -255) return;
  digitalWrite(mp[motor][1], val > 0);
  digitalWrite(mp[motor][2], val < 0);
  if(val < 0) val = -val;
  analogWrite(mp[motor][0], val);
}

void keep() {
  ss[10] += dd10;
  ss[12] += dd12;
  ss[10] = bd_deg(ss[10], 0, 180);
  ss[12] = bd_deg(ss[12], 90, 170);
  for(int i = 10; i < 13; i++) sv[i].write(ss[i]);
  sv[8].write(xy2deg8[y][x]);
  sv[9].write(xy2deg9[y][x]);
  spset(0, map(ps.Analog(PSS_RY), 0, 255, -mvsp, mvsp));
  spset(2, map(ps.Analog(PSS_RY), 0, 255, -mvsp, mvsp));
  spset(1, map(ps.Analog(PSS_LY), 0, 255, -mvsp, mvsp));
  spset(3, map(ps.Analog(PSS_LY), 0, 255, -mvsp, mvsp));

  for(int i = 8; i < 13; i++) {
    Serial.print(sv[i].read());
    Serial.print(" ");
  }
  Serial.print(dd10);
  Serial.print(" ");
  Serial.print(dd12);
  Serial.print('\n');
}



void setup(){
  nxt = millis() + 30;
  while(millis() < nxt);
  dd10 = dd12 = 0;
  mvsp = 255;
  Serial.begin(9600);
  for(int i = 8; i < 13; i++) {
    sv[i].attach(i);
    sv[i].write(sini[i-8]);
    ss[i] = sini[i-8];
  }
  int error =
    ps.config_gamepad(PCLK, PCMD, PSEL, PDAT, false, false);
  int type = ps.readType();
  for(int i = 0; i < 4; i++)
    for(int j = 1; j < 3; j++) pinMode(mp[i][j], OUTPUT);

  while(1) {
    ps.read_gamepad(false, false);
    if(ps.BP(PSB_START)) break;
    nxt = millis() + 30;
    while(millis() < nxt);
  }

}

void loop() {
  ps.read_gamepad(false, false);

  if(ps.BP(PSB_CIRCLE)) dd12 = -1;
  if(ps.BR(PSB_CIRCLE)) dd12 = 0;

  if(ps.BP(PSB_CROSS)) ss[12] = 170;

  if(ps.BP(PSB_SQUARE)) {
    if(x == 1 && y == 2) x = 1, y = 0;
    else x = 1, y = 2;
  }
  if(ps.BP(PSB_TRIANGLE)) {
    if(x == 1 && y == 2) x = 3, y = 0;
    else x = 1, y = 2;
  }

  if(ps.BR(PSB_R3)) is_R3 = 0;
  else if(ps.BP(PSB_R3) && is_R3 == 0) {
    is_R3 = 1;
    mvsp = (mvsp == 255)? 80: 255;
  }

  if(ps.BR(PSB_L3)) is_L3 = 0;
  else if(ps.BP(PSB_L3) && is_L3 == 0) {
    is_L3 = 1;
    mvsp = (mvsp == 255)? 80: 255;
  }

  if(ps.BP(PSB_SELECT)) is_sel = 1;
  else if(is_sel == 1) {
    if(ps.BP(PSB_R1)) is_sel = 0, ss[11] = (ss[11] == 10)? 90: 10;
    else if(ps.BP(PSB_R2)) {
      is_sel = 0;
      for(int i = 0; i < 4; i++) spset(i, -255);
      nxt = millis() + 500;
      while(millis() < nxt);
      for(int i = 0; i < 4; i++) spset(i, 255);
      nxt = millis() + 500;
      while(millis() < nxt);
      ss[11] = 90;
      sv[11].write(ss[11]);
      nxt = millis() + 100;
      while(millis() < nxt);
    }
  }

  if(ps.BP(PSB_L1)) if(is_L1 == 0 && y > 0 && xy2deg8[y-1][x] > 0) y--, is_L1 = 1;
  if(ps.BR(PSB_L1)) is_L1 = 0;
  if(ps.BP(PSB_L2)) if(is_L2 == 0 && y < 6 && xy2deg8[y+1][x] > 0) y++, is_L2 = 1;
  if(ps.BR(PSB_L2)) is_L2 = 0;

  if(ps.BP(PSB_PAD_UP)) if(is_up == 0 && x < 5 && xy2deg8[y][x+1] > 0) x++, is_up = 1;
  if(ps.BR(PSB_PAD_UP)) is_up = 0;
  if(ps.BP(PSB_PAD_DOWN)) if(is_dw == 0 && x > 0 && xy2deg9[y][x-1] > 0) x--, is_dw = 1;
  if(ps.BR(PSB_PAD_DOWN)) is_dw = 0;

  if(ps.BP(PSB_PAD_RIGHT)) dd10 = 1;
  if(ps.BR(PSB_PAD_RIGHT)) dd10 = 0;
  if(ps.BP(PSB_PAD_LEFT)) dd10 = -1;
  if(ps.BR(PSB_PAD_LEFT)) dd10 = 0;

  keep();
  nxt = millis() + 10;
  while(millis() < nxt);
}
