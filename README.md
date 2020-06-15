# 2020ME_Project ver2.0.0
Main Program: GOGO.ino

ADD:
1. 當夾爪在籃子上方時，釋放夾爪會讓兩個籃子馬達繃緊，確保東西不會掉落
2. 修正 BSK() 跳出問題
3. 刪除角度弧度轉換 function ，以內建常數替代

TODO:


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
