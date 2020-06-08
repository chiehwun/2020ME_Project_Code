#include <PS2X_lib.h>  //for v1.6
PS2X ps2x; // create PS2 Controller Class
#define PDAT 24
#define PCMD 25
#define PSEL 22
#define PCLK 23
#define BP     ButtonPressed
#define BR     ButtonReleased
byte vibrate = 0;

unsigned nxt = 0;
void setup(){
  Serial.begin(57600);
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
  while(1) {
    ps2x.read_gamepad(false, false);
    if(ps2x.BP(PSB_START) && ps2x.BP(PSB_RED)) break; // press both START and RED
    nxt = millis() + 30;
    while(millis() < nxt);
  }
}

void loop() {
  ps2x.read_gamepad(false, vibrate);          //read controller and set large motor to spin at 'vibrate' speed

  if(ps2x.Button(PSB_START))                   //will be TRUE as long as button is pressed
  Serial.println("Start is being held");
  if(ps2x.Button(PSB_SELECT))
  Serial.println("Select is being held");

  if(ps2x.Button(PSB_PAD_UP)) {         //will be TRUE as long as button is pressed
    Serial.print("Up held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
  }
  if(ps2x.Button(PSB_PAD_RIGHT)){
    Serial.print("Right held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
  }
  if(ps2x.Button(PSB_PAD_LEFT)){
    Serial.print("LEFT held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
  }
  if(ps2x.Button(PSB_PAD_DOWN)){
    Serial.print("DOWN held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
  }

  vibrate = ps2x.Analog(PSAB_BLUE);        //this will set the large motor vibrate speed based on

  if (ps2x.NewButtonState())               //will be TRUE if any button changes state (on to off, or off to on)
  {
    if(ps2x.Button(PSB_L3))
    Serial.println("L3 pressed");
    if(ps2x.Button(PSB_R3))
    Serial.println("R3 pressed");
    if(ps2x.Button(PSB_L2))
    Serial.println("L2 pressed");
    if(ps2x.Button(PSB_R2))
    Serial.println("R2 pressed");
    if(ps2x.Button(PSB_GREEN))
    Serial.println("Triangle pressed");
  }

  if(ps2x.ButtonPressed(PSB_RED))             //will be TRUE if button was JUST pressed
  Serial.println("Circle just pressed");

  if(ps2x.ButtonReleased(PSB_PINK))             //will be TRUE if button was JUST released
  Serial.println("Square just released");

  if(ps2x.NewButtonState(PSB_BLUE)){            //will be TRUE if button was JUST pressed OR released
    Serial.println("X just changed");
    Serial.print("vibrate=");
    Serial.println(vibrate);
  }

  if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) // print stick values if either is TRUE
  {
    Serial.print("Stick Values:");
    Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_LX), DEC);
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RY), DEC);
    Serial.print(",");
    Serial.println(ps2x.Analog(PSS_RX), DEC);
  }
  delay(50);

}
