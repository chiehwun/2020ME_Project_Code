# 2020ME_Project ver2.0.0
Main Program: GOGO.ino

ADD:
1. 利用attach() 校正 M1, M2 角度
2. Scara直接移動到籃子(可中斷)
3. 修復保護措施的bug

TODO:
1. 物件導向化
  (1) MyServo
  (2) Scara
2. Scara.moveXY(double x, double y)
3. 整理程式

How to Calibrate Scara ARM:
1. Using attch() to initialize min and max pulse width <https://www.arduino.cc/en/Reference/ServoAttach>
2. ARM1 reset: offset1 = M1 set to 0 then measure the angle between X+ & ARM1
3. ARM2 reset: offset2 = make ARM1 & ARM2 parallel then record M2

Including:
1. PS2          control
2. DC     motor control
3. Servo  motor control
4. XY Scara Arm control

Discard:
1. Stepper motor control
