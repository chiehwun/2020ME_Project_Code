#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>
PS2X ps2x; // create PS2 Controller Class
/* PS2 Configuration
 * PSB_L1               armZ+
 * PSB_L2               armZ-
 * PSB_R1               arm2bskt1
 * PSB_R2               arm2bskt2
 * PSB_PAD_UP           armY+
 * PSB_PAD_DOWN         armY-
 * PSB_PAD_LEFT         armX+
 * PSB_PAD_RIGHT        armX-
 * PSB_RED              grp+  // 夾緊
 * PSB_BLUE             grp-  // 放鬆
 * PSB_PINK             bskt_push
 * PSB_GREEN            bskt_pull
 * Analog(PSS_LY)       LDCM_move
 * Analog(PSS_RY)       RDCM_move
 */
 /* Pin Layout
  * LDCM  2
  * RDCM  3
  * ZDCM  4
  * ARM1  5
  * ARM2  6
  * ARM3  7
  * GRP   8
  * BSKT1 9
  * BSKT2 10
  * TESTS 11
  */
// DC pin
#define PLDCM   2
#define PRDCM   3
#define PZDCM   4
// Servo pin
#define PBSKT1  5
#define PBSKT2  6
#define PARM1   7
#define PARM2   8
#define PARM3   9
#define PGRP    10
#define PTESTS  11
// PS2 pin layout
#define PCLK    23
#define PSEL    22  // pin CS
#define PCMD    25
#define PDAT    24

#define BP    ButtonPressed
#define BR    ButtonReleased
unsigned nxt = 0;
byte dcm[][3] = {{PLDCM, 50, 51},{PRDCM, 52, 53},{PZDCM, 49, 48}}; // {EN IN1 IN2}

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

void warn() {
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  delay(50);
}

void DCM_set(byte motor, int val) {
  if(val > 255 || val < -255) return;
  digitalWrite(dcm[motor][1], val >   0 );  // Direction set
  digitalWrite(dcm[motor][2], val <=  0 );  // Direction set
  analogWrite (dcm[motor][0], abs(val));  // Speed set
}
