#include <PS2X_lib.h>  // Thư viện điều khiển PS2
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

PS2X ps2x; // Tạo đối tượng PS2X để xử lý tín hiệu từ PS2

// Khởi tạo PCA9685 với địa chỉ I2C mặc định (0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Định nghĩa các chân kết nối với tay cầm PS2
#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK

#define pressures false
#define rumble false

int A = 0, B = 0, M = 0, S = 0, left_speed, right_speed, X, Y, speed;

//Định nghĩa các chân kết nối với động cơ
#define motor1_int8  8
#define motor1_int9  9
#define motor2_int10  10
#define motor2_int11  11
#define motor3_int12  12
#define motor3_int13  13
#define motor4_int14  14
#define motor4_int15  15

#define takedrop_int0  0
#define takedrop_int1  1
#define throw_int2  2
#define throw_int3  3

#define servo_int5 5

// Hàm setup() khởi tạo kết nối và thiết lập các chế độ
void setup() {
  // Khởi động Serial
  Serial.begin(115200);

  // Khởi động PCA9685
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(60);

  Wire.setClock(400000);

  delay(300);

  // Khởi động PS2 Controller
  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
}

// Hàm loop() để chạy mã liên tục
void loop() {
  ps2x.read_gamepad(false, 0); // Đọc tín hiệu từ PS2

  Run_ps2();

  if (M == 0) stopMoving();
  if (M == 1) moveForward();
  if (M == 2) moveBackward();
  if (M == 3) rotateLeft();
  if (M == 4) rotateRight();
  if (M == 5 || M == 7) Front();
  if (M == 6 || M == 8) Back();

  if (A == 0) stopBall();
  if (A == 1) takeBall();
  if (A == 2) dropBall();

  if (B == 1) throwBall();
  if (B == 0) stopThrow();

  if (S == 1) servo();
}

void Run_ps2() {
  // joystick - Di chuyển
  X = ps2x.Analog(PSS_LX);
  Y = ps2x.Analog(PSS_LY);

  if (X == 128 && Y < 127) {
    M = 1;
    speed = topulse(Y, 0, 126, 4095, 1);
  }
  else if (X == 128 && Y > 127) {
    M = 2;
    speed = topulse(Y, 128, 255, 1, 4095);
  }
  else if (Y == 127 && X < 128) {
    M = 3;
    speed = topulse(X, 0, 127, 4095, 1);
  }
  else if (Y == 127 && X > 128) {
    M = 4;
    speed = topulse(X, 129, 255, 1, 4095);
  }
  else if (X < 128 && Y < 127) {
    M = 5;
    right_speed = topulse(X, 0, 127, 4095, 1);
    left_speed = topulse(Y, 0, 126, right_speed, 1);
  }
  else if (X < 128 && Y > 127) {
    M = 6;
    right_speed = topulse(X, 0, 127, 4095, 1);
    left_speed = topulse(Y, 128, 255, 1, right_speed);
  }
  else if (X > 128 && Y < 127) {
    M = 7;
    left_speed = topulse(X, 129, 255, 1, 4095);
    right_speed = topulse(Y, 0, 126, left_speed, 1);
  }
  else if (X > 128 && Y > 127) {
    M = 8;
    left_speed = topulse(X, 129, 255, 1, 4095);
    right_speed = topulse(Y, 128, 255, 1, left_speed);
  }
  else {
    M = 0;
  }

  if (ps2x.Button(PSB_CIRCLE)) {  // Nút Tròn - Lấy bóng
    A = 1;
  }
  else if (ps2x.Button(PSB_SQUARE)) {  // Nút Vuông - Nhả bóng
    A = 2;
  }
  else if (ps2x.Button(PSB_TRIANGLE)) { // Nút Tam Giác - Dừng quay
    A = 0;
  }

  if (ps2x.Button(PSB_CROSS) && B == 0) {  // Nút X - Ném bóng
    B = 1;
    delay(1000);
  }
  else if (ps2x.Button(PSB_CROSS) && B == 1) {
    B = 0;
    delay(1000);
  }

  if (ps2x.Button(PSB_R1) && S == 0) { // Nút R1 - Xoay servo
    S = 1;
    delay(1000);
  }
  else if (ps2x.Button(PSB_R1) && S == 1) {
    S = 0;
    delay(1000);
  }

  delay(50);
}

// Hàm điều khiển di chuyển tiến lên
void moveForward() {
  pwm.setPWM(motor1_int8, 0, speed);
  pwm.setPWM(motor1_int9, 0, 0);
  pwm.setPWM(motor2_int10, 0, speed);
  pwm.setPWM(motor2_int11, 0, 0);
  pwm.setPWM(motor3_int12, 0, speed);
  pwm.setPWM(motor3_int13, 0, 0);
  pwm.setPWM(motor4_int14, 0, speed);
  pwm.setPWM(motor4_int15, 0, 0);
}

// Hàm điều khiển di chuyển lùi
void moveBackward() {
  pwm.setPWM(motor1_int8, 0, 0);
  pwm.setPWM(motor1_int9, 0, speed);
  pwm.setPWM(motor2_int10, 0, 0);
  pwm.setPWM(motor2_int11, 0, speed);
  pwm.setPWM(motor3_int12, 0, 0);
  pwm.setPWM(motor3_int13, 0, speed);
  pwm.setPWM(motor4_int14, 0, 0);
  pwm.setPWM(motor4_int15, 0, speed);
}

// Hàm điều khiển xoay trái
void rotateLeft() {
  pwm.setPWM(motor1_int8, 0, 0);
  pwm.setPWM(motor1_int9, 0, speed);
  pwm.setPWM(motor2_int10, 0, 0);
  pwm.setPWM(motor2_int11, 0, speed);
  pwm.setPWM(motor3_int12, 0, speed);
  pwm.setPWM(motor3_int13, 0, 0);
  pwm.setPWM(motor4_int14, 0, speed);
  pwm.setPWM(motor4_int15, 0, 0);
}

// Hàm điều khiển xoay phải
void rotateRight() {
  pwm.setPWM(motor1_int8, 0, speed);
  pwm.setPWM(motor1_int9, 0, 0);
  pwm.setPWM(motor2_int10, 0, speed);
  pwm.setPWM(motor2_int11, 0, 0);
  pwm.setPWM(motor3_int12, 0, 0);
  pwm.setPWM(motor3_int13, 0, speed);
  pwm.setPWM(motor4_int14, 0, 0);
  pwm.setPWM(motor4_int15, 0, speed);
}

// Hàm lên trái/phải
void Front() {
  pwm.setPWM(motor1_int8, 0, left_speed);
  pwm.setPWM(motor1_int9, 0, 0);
  pwm.setPWM(motor2_int10, 0, left_speed);
  pwm.setPWM(motor2_int11, 0, 0);
  pwm.setPWM(motor3_int12, 0, right_speed);
  pwm.setPWM(motor3_int13, 0, 0);
  pwm.setPWM(motor4_int14, 0, right_speed);
  pwm.setPWM(motor4_int15, 0, 0);
}

// Hàm xuống trái/phải
void Back() {
  pwm.setPWM(motor1_int8, 0, 0);
  pwm.setPWM(motor1_int9, 0, left_speed);
  pwm.setPWM(motor2_int10, 0, 0);
  pwm.setPWM(motor2_int11, 0, left_speed);
  pwm.setPWM(motor3_int12, 0, 0);
  pwm.setPWM(motor3_int13, 0, right_speed);
  pwm.setPWM(motor4_int14, 0, 0);
  pwm.setPWM(motor4_int15, 0, right_speed);
}

// Hàm dừng di chuyển
void stopMoving() {
  pwm.setPWM(motor1_int8, 0, 0);
  pwm.setPWM(motor1_int9, 0, 0);
  pwm.setPWM(motor2_int10, 0, 0);
  pwm.setPWM(motor2_int11, 0, 0);
  pwm.setPWM(motor3_int12, 0, 0);
  pwm.setPWM(motor3_int13, 0, 0);
  pwm.setPWM(motor4_int14, 0, 0);
  pwm.setPWM(motor4_int15, 0, 0);
}

// Hàm điều khiển lấy bóng
void takeBall() {
  pwm.setPWM(takedrop_int0, 0, 4095);
  pwm.setPWM(takedrop_int1, 0, 0);
}

// Hàm điều khiển nhả bóng
void dropBall() {
  pwm.setPWM(takedrop_int0, 0, 0);
  pwm.setPWM(takedrop_int1, 0, 4095);
}

// Hàm dừng quay bóng
void stopBall() {
  pwm.setPWM(takedrop_int0, 0, 0);
  pwm.setPWM(takedrop_int1, 0, 0);
}

// Hàm điều khiển ném bóng
void throwBall() {
  pwm.setPWM(throw_int2, 0, 4095);
  pwm.setPWM(throw_int3, 0, 0);
}

// Hàm dừng bắn bóng
void stopThrow() {
  pwm.setPWM(throw_int2, 0, 0);
  pwm.setPWM(throw_int3, 0, 0);
}

// Hàm điều khiển servo
void servo() {
  for (int goc = 0; goc <= 90; goc += 10) {
    delay(100);
    pwm.setPWM(servo_int5, 0, topulse(goc, 0, 90, 90, 600));
  }

  for (int goc = 90; goc >= 0; goc -= 10) {
    delay(100);
    pwm.setPWM(servo_int5, 0, topulse(goc, 0, 90, 90, 600));
  }
}

// Hàm biến đổi xung
int topulse(int val, int inp_min, int inp_max, int out_min, int out_max) {
  int xung = map(val, inp_min, inp_max, out_min, out_max);
  return xung;
}
