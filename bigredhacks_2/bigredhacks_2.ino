// Grove temperature things
#include <math.h>
int a;
float in_temp;
int B=3975;                  //B value of the thermistor
float resistance;
#define temp_pin A0
// define temperature comfortability limits
#define temp_c_min 22
#define temp_c_max 26

// Grove button things
#define button_pin 18
#define pot_pin A1
volatile int temp_ref;
#define temp_ref_min 10
#define temp_ref_max 35
#define temp_gain 0.2

// LCD display
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
#define servo_1_down 45
#define servo_1_up 135
#define servo_2_down 135
#define servo_2_up 45
#define servo_breathe 20 
void sweep(Servo &s, int a, int b);
const int servo_inc = 1;
const int servo_del = 10;

// LEDs
// sun/moon
#define r1_pin 13
#define g1_pin 12
#define b1_pin 11
// spotlights
#define r2_pin 22
#define g2_pin 23
#define r3_pin 26
#define g3_pin 27
#define r4_pin 30
#define g4_pin 31
#define r5_pin 34
#define g5_pin 35
// create arrays
int led_1[] = {r1_pin, g1_pin, b1_pin};
int led_2[] = {r2_pin, g2_pin};
int led_3[] = {r3_pin, g3_pin};
int led_4[] = {r4_pin, g4_pin};
int led_5[] = {r5_pin, g5_pin};

// current time
volatile int h = 0;
volatile int m = 0;

// happiness level (0 - 100)
volatile int happiness = 100;
#define h_inc 5
// minimum scores for happiness levels
#define happy_1 30
#define happy_2 60
// updating happiness state
volatile int h_state = 2;

// setup function
void setup() {
  // LCD init
  lcd.begin(16, 2);
  lcd.setRGB(0, 255, 0);
  lcd.setCursor(0, 0);
  lcd.print("Puppet Plant");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // Serial init
  Serial.begin(9600);
  while (Serial.available() == 0) {
  }
  Serial.read();
  Serial.println("Start");

  // LCD init again
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
  // get an initial actual temperature reading
  in_temp = getTemp();
  lcd.setCursor(4, 1);
  lcd.print(String(in_temp));

  // Servo init
  servo_1.attach(servo_1_pin);
  servo_2.attach(servo_2_pin);
  servo_1_pos = servo_1_up;
  servo_2_pos = servo_2_up;
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

  // Pseudo-thermostat init
  pinMode(pot_pin, INPUT);
  
  delay(1000);
}

// let servo s move from a to b
void sweep(Servo &s, int a, int b) {
  // get the direction from a to b
  int i_sign = (b-a)/abs(b-a);
  for (int pos = a; pos <= b; pos += i_sign*servo_inc) {
    s.write(pos);
    delay(servo_del);
  }
}

// move both servos
// move servo_1 from a to b
// move servo_2 from c to d
void sweep_12(int a, int b, int c, int d) {
  int i_sign = (b-a)/abs(b-a);
  for (servo_1_pos = a; servo_1_pos <= b; servo_1_pos += i_sign*servo_inc) {
    servo_1.write(servo_1_pos);
    delay(servo_del);
    // must map range of 1 to range of 2
    servo_2.write(map(servo_1_pos, a, b, c, d));
    delay(servo_del);
  }
}

// set the color of the sun/moon LED
void setColor(int led[], int red, int green, int blue)
{
  analogWrite(led[0], red);
  analogWrite(led[1], green);
  analogWrite(led[2], blue);  
}

// set the colors of the spotlights and the screen
void setColorDigital(int led[], int red, int green)
{
  digitalWrite(led[0], red);
  digitalWrite(led[1], green); 
}

// change the sun/moon LED depending on the time
void setLedTime() {
  // night (blue, before 5am or after 5pm)
  if (h >= 17 || h < 5) {
    setColor(led_1, 0, 0, 255);
  // day (yellow)
  } else {
    setColor(led_1, 255, 255, 0);
  }
}

// get a temperature reading from the temperature sensor
float getTemp() {
  // get current temperature
  a=analogRead(temp_pin);
  resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
  return 1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet&nbsp;;
}

// change the inside temperature display
float tempChange() {
  temp_ref = map(analogRead(pot_pin), 0, 1024, temp_ref_min, temp_ref_max); 
  in_temp = in_temp + temp_gain*(temp_ref - in_temp);
  lcd.setCursor(12, 1);
  lcd.print(String(temp_ref));
  lcd.setCursor(4, 1);
  lcd.print(String(in_temp));
}

// calculate whether the plant should be sad or happy
bool getHappy() {
  // check if inside comfortability range
  if (in_temp >= temp_c_min && in_temp <= temp_c_max) {
    // cap happiness at 100
    happiness = min(happiness+h_inc, 100);
  // otherwise outside of comfortability range
  } else {
    // limit happiness to above 0
    happiness = max(happiness-h_inc, 0);
  }

  // compare happiness to happy thresholds, update h_state
  if (happiness > happy_2) {
    h_state = 2;
  } else if (happiness > happy_1) {
    h_state = 1;
  } else {
    h_state = 0;
  }
}

// change the spotlights and LCD depending on happiness 
// also move servos
void setHappy() {
  // check if happy
  int g = 0;
  if (h_state > 0) {
    g = 1;
  }
  int r = !g;

  // set spotlights
  setColorDigital(led_2, r, g);
  setColorDigital(led_3, r, g);
  setColorDigital(led_4, r, g);
  setColorDigital(led_5, r, g);
  // set LCD
  lcd.setRGB(r*255, g*255, 0);

  // update branch states
  switch (h_state) {
    // both happy
    case 2:
      servo_1_pos = servo_1_up;
      servo_2_pos = servo_2_up;
      break;
    // only one happy
    case 1:
      servo_1_pos = servo_1_down;
      servo_2_pos = servo_2_up;
      break;
    // neither happy
    default:
      servo_1_pos = servo_1_down;
      servo_2_pos = servo_2_down;
  }
  servo_1.write(servo_1_pos);
  servo_2.write(servo_2_pos);
}

// let the branches "breathe"
void breathe() {
  int servo_1_max = servo_1_pos + servo_breathe;
  int servo_1_min = servo_1_pos - servo_breathe;
  int servo_2_max = servo_2_pos - servo_breathe;
  int servo_2_min = servo_2_pos + servo_breathe;
  sweep_12(servo_1_max, servo_1_min, servo_2_max, servo_2_min);
}

// loop
void loop() {
  // update sun/moon LED
  setLedTime();

  // update the temperature display (will do this periodically)
  tempChange();
  delay(100);

  // update happiness
  getHappy();
  setHappy();

  // get info from python
  if (Serial.available() > 0) {
    // get, print the current outside temperature
    float out_temp = Serial.parseFloat();
    Serial.println(in_temp);
    lcd.setCursor(4, 0);
    lcd.print(String(out_temp));
    tempChange();

    // print time things
    h = Serial.parseInt();
    m = Serial.parseInt();
    // minute is 2 digits
    if (m > 9) {
      lcd.setCursor(14,0);
      lcd.print(m);
    // minute is 1 digit
    } else {
      lcd.setCursor(15,0);
      lcd.print(m);
      lcd.setCursor(14,0);
      lcd.print("0");
    }
    lcd.setCursor(13, 0);
    lcd.print(":");
    // hour is 2 digits
    if (h > 9) {
      lcd.setCursor(11, 0);
    // minute is 1 digit
    } else {
      lcd.setCursor(12, 0);
    }
    lcd.print(h);

    // devour rest of buffer
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
  
  tempChange();
}

