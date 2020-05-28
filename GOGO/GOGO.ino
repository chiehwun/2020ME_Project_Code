#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>
PS2X ps2x; // create PS2 Controller Class
/* PS2 Configuration
 * PSB_L1               armZ+
 * PSB_L2               armZ-
 * PSB_R1               arm2bsktA
 * PSB_R2               arm2bsktB
 * PSB_PAD_UP           armY+
 * PSB_PAD_DOWN         armY-
 * PSB_PAD_LEFT         armX+
 * PSB_PAD_RIGHT        armX-
 * PSB_RED              grp+  // 夾緊
 * PSB_BLUE             grp-  // 放鬆
 * Analog(PSS_LY)       LDCM_move
 * Analog(PSS_RY)       RDCM_move
 */
// PS2 pin layout
#define PCLK  51
#define PSEL  50  // pin CS
#define PCMD  53
#define PDAT  52

#define BP    ButtonPressed
#define BR    ButtonReleased
byte vibrate = 0;
int lspd = 0, rspd = 0, mvspd = 255;
Servo sv[12]; // Servo pin 4-12
int svdeg[12] = {0,0,0,0,180,0,0,0,0,0,0,0};
int dd10 = 0;
byte dcm[][3] = {{2, 22, 23},{3, 24, 25}}; // {EN IN1 IN2}
unsigned nxt = 0;

inline void PS2_set() {
  // GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  byte error = ps2x.config_gamepad(PCLK,PCMD,PSEL,PDAT,true,true);
  if(error == 0)
    Serial.println("Found Controller, configured successful");
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  byte type = ps2x.readType();  // 0: Unknown, 1: DualShock, 2: GuitarHero
  int count = 0;
  Serial.println("Press START and RED for 1 sec");
  // Protection: Press both START and RED for 1 seconds to start
  while(1) {
    ps2x.read_gamepad(false, false);
    if(ps2x.Button(PSB_START) && ps2x.Button(PSB_RED)) {  // press both START and RED
      if(++count > 30)
        break;
    }
    else count = 0;
    nxt = millis() + 30;
    while(millis() < nxt);
  }
  Serial.println("Robot Launched Sucessfully!!");
}

void keep() {
  DCM_set(0, lspd);
  DCM_set(1, rspd);
  svdeg[4] += dd10;
  svdeg[4] = constrain(svdeg[4], 0, 180);
  for(int i = 4; i <= 4; i++)
    sv[i].write(svdeg[i]);
}

void setup(){
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(500);

  Serial.begin(9600);
  PS2_set();
  digitalWrite(13, LOW);
  delay(500);
  // DC motor init
  for(int i = 0; i < 2; i++)
    for(int j = 0; j < 3; j++)
      pinMode(dcm[i][j], OUTPUT);

  for(int i = 4; i <= 4; i++) {
    sv[i].attach(i);
    svdeg[4] = constrain(svdeg[4], 0, 180);
//    sv[i].write(svdeg[i]);
  }
}

void loop() {
  ps2x.read_gamepad(false, vibrate);
  lspd = map(ps2x.Analog(PSS_LY),0,255,mvspd,-mvspd);
  rspd = map(ps2x.Analog(PSS_RY),0,255,-mvspd,mvspd);
  Serial.print("L = ");
  Serial.print(lspd);
  Serial.print(", R = ");
  Serial.println(rspd);
  if(ps2x.BP(PSB_L1)){
    Serial.println("PSB_L1");
  }
  if(ps2x.BR(PSB_L1)){
    Serial.print("PSB_L1 = BR");
  }
  if(ps2x.BP(PSB_L2)) {
    Serial.println("PSB_L2");
  }
  if(ps2x.BR(PSB_L2)){
    Serial.print("PSB_L2 = BR");
  }

  if(ps2x.BP(PSB_R1)){
    Serial.print("PSB_R1 = BP");
    dd10 = 5;
  }
  if(ps2x.BR(PSB_R1)) dd10 = 0;
  if(ps2x.BP(PSB_R2)) dd10 = -5;
  if(ps2x.BR(PSB_R2)) dd10 = 0;
  //if(ps2x.BP(PSB_R1)) sv[4].write(0);
  keep();
  delay(10);
}


void 2(byte motor, int val) {
  if(val > 255 || val < -255) return;
  digitalWrite(dcm[motor][1], val > 0 );  // Direction set
  digitalWrite(dcm[motor][2], val < 0 );  // Direction set
  analogWrite (dcm[motor][0], abs(val));  // Speed set
}


void warn() {
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  delay(50);
}
