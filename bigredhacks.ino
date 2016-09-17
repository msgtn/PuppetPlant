// Grove temperature things
#include <math.h>
int a;
float in_temp;
int B=3975;                  //B value of the thermistor
float resistance;
#define temp_pin A0

// LCD display
//#include <SPI.h>
//#include <LiquidCrystal.h>
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#include <Wire.h>
#include "rgb_lcd.h"
rgb_lcd lcd;

// Servos
#include <Servo.h>
#define servo_1_pin 2
#define servo_2_pin 3
Servo servo_1;
Servo servo_2;
volatile int servo_1_pos;
volatile int servo_2_pos;
void sweep(Servo &s, int a, int b);
const int servo_inc = 1;
const int servo_del = 15;

// LEDs
#define r1_pin 13
#define g1_pin 12
#define b1_pin 11
//////
#define r2_pin 22
#define g2_pin 23
#define r3_pin 26
#define g3_pin 27
#define r4_pin 30
#define g4_pin 31
#define r5_pin 34
#define g5_pin 35
//////
int led_1[] = {r1_pin, g1_pin, b1_pin};
int led_2[] = {r2_pin, g2_pin};
int led_3[] = {r3_pin, g3_pin};
int led_4[] = {r4_pin, g4_pin};
int led_5[] = {r5_pin, g5_pin};
// time (for LEDs)
volatile int h = 0;
volatile int m = 0;

void setup() {
  // LCD init
  lcd.begin(16, 2);
  lcd.setRGB(0, 255, 0);
  lcd.setCursor(0, 0);
  lcd.print("NAME HERE");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // Serial init
  Serial.begin(9600);
  while (Serial.available() == 0) {
  }
  Serial.read();
  Serial.println("Start");
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  lcd.setCursor(0, 0);
  lcd.print("Out");
  lcd.setCursor(0,1);
  lcd.print("In");
  lcd.setCursor(9, 0);
  lcd.print("C");
  lcd.setCursor(9, 1);
  lcd.print("C");

  // Servo init
  servo_1.attach(servo_1_pin);
  servo_2.attach(servo_2_pin);
  servo_1_pos = 45;
  servo_2_pos = 0;
  servo_1.write(servo_1_pos);
  servo_2.write(servo_2_pos);

  // LED init
  for (int i = 0; i < 2; i++) {
    pinMode(led_1[i], OUTPUT);
    analogWrite(led_1[i], 0);
    pinMode(led_2[i], OUTPUT);
    analogWrite(led_2[i], i*255);
    pinMode(led_3[i], OUTPUT);
    analogWrite(led_3[i], i*255);
    pinMode(led_4[i], OUTPUT);
    analogWrite(led_4[i], i*255);
    pinMode(led_5[i], OUTPUT);
    analogWrite(led_5[i], i*255);
  }
  pinMode(led_1[2], OUTPUT);
  analogWrite(led_1[2], 0);
}

void sweep(Servo &s, int a, int b) {
  if (a < b) {
    for (int pos = a; pos <= b; pos += servo_inc) {
      s.write(pos);
      delay(servo_del);
    }
  } else {
    for (int pos = a; pos >= b; pos -= servo_inc) {
      s.write(pos);
      delay(servo_del);
    }
  }
}

void sweep_12(int a, int b, int c, int d) {
  if (a < b) {
    for (servo_1_pos = a; servo_1_pos <= b; servo_1_pos += servo_inc) {
      servo_1.write(servo_1_pos);
      delay(servo_del);
      servo_2.write(map(servo_1_pos, a, b, c, d));
      delay(servo_del);
    }
  } else {
    for (servo_1_pos = a; servo_1_pos >= b; servo_1_pos -= servo_inc) {
      servo_1.write(servo_1_pos);
      delay(servo_del);
      servo_2.write(map(servo_1_pos, a, b, c, d));
      delay(servo_del);
    }
  }
}
 
void setColor(int led[], int red, int green, int blue)
{
  analogWrite(led[0], red);
  analogWrite(led[1], green);
  analogWrite(led[2], blue);  
}
 
void setColorDigital(int led[], int red, int green)
{
  digitalWrite(led[0], red);
  digitalWrite(led[1], green); 
  lcd.setRGB(red*255, green*255, 0);
}

void setLedTime() {
  if (h >= 17) {
    // night time
    setColor(led_1, 0, 0, 255);
  } else {
    // day time
    setColor(led_1, 255, 255, 0);
  }
}

void loop() {
  setLedTime();
  // get current temperature
  a=analogRead(temp_pin);
  resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
  in_temp=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet&nbsp;;
  delay(1000);
  // update temperature display
  lcd.setCursor(4, 1);
  lcd.print(String(in_temp));

//  sweep(servo_1, 0, 180);
//  sweep(servo_2, 0, 180);
  sweep_12(45, 90, 0, 180);
  setColorDigital(led_2, 1, 0);
  setColorDigital(led_3, 1, 0);
  setColorDigital(led_4, 1,  0);
  setColorDigital(led_5, 1, 0);

  // get temperature reading from python
  if (Serial.available() > 0) {
    float out_temp = Serial.parseFloat();
    Serial.println(in_temp);
    lcd.setCursor(4, 0);
    lcd.print(String(out_temp));

    // print time things
    h = Serial.parseInt();
    m = Serial.parseInt();
    if (m <= 9) {
      lcd.setCursor(15,0);
      lcd.print(m);
      lcd.setCursor(14,0);
      lcd.print("0");
    } else {
      lcd.setCursor(14,0);
      lcd.print(m);
    }
    lcd.setCursor(13, 0);
    lcd.print(":");
    if (h > 9) {
      lcd.setCursor(11, 0);
    } else {
      lcd.setCursor(12, 0);
    }
    lcd.print(h);

    // devour rest of buffer
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
  setColorDigital(led_2, 0, 1);
  setColorDigital(led_3, 0, 1);
  setColorDigital(led_4, 0, 1);
  setColorDigital(led_5, 0, 1);

  sweep_12(90, 45, 180, 0);
//  sweep(servo_1, 180, 0);
//  sweep(servo_2, 180, 0);
}

