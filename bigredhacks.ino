// Grove temperature things
#include <math.h>
int a;
float in_temp;
int B=3975;                  //B value of the thermistor
float resistance;
#define temp_pin A0

// LCD display
#include <SPI.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Servos
#include <Servo.h>
#define servo_1_pin 2
#define servo_2_pin 3
Servo servo_1;
Servo servo_2;
void sweep(Servo &s, int a, int b);
const int servo_inc = 1;
const int servo_del = 15;

// LEDs
#define r1_pin 13
#define g1_pin 12
#define b1_pin 11
//////
#define r2_pin 44
#define g2_pin 45
#define b2_pin 46
//////
int led_1[] = {r1_pin, g1_pin, b1_pin};
int led_2[] = {r2_pin, g2_pin, b2_pin};
// time (for LEDs)
volatile int h = 0;
volatile int m = 0;

void setup() {
  // LCD init
  lcd.begin(16, 2);
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

  // LED init
  for (int i = 0; i < 3; i++) {
    pinMode(led_1[i], OUTPUT);
    analogWrite(led_1[i], 0);
    pinMode(led_2[i], OUTPUT);
    analogWrite(led_2[i], 0);
  }
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

void sweep_12(int a, int b) {
  if (a < b) {
    for (int pos = a; pos <= b; pos += servo_inc) {
//    Serial.println(pos);
      servo_1.write(pos);
      delay(servo_del);
      servo_2.write(pos);
      delay(servo_del);
    }
  } else {
    for (int pos = a; pos >= b; pos -= servo_inc) {
//    Serial.println(pos);
      servo_1.write(pos);
      delay(servo_del);
      servo_2.write(pos);
      delay(servo_del);
    }
  }
}
 
void setColor(int led[], int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(led[0], red);
  analogWrite(led[1], green);
  analogWrite(led[2], blue);  
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

  sweep(servo_1, 0, 180);
  sweep(servo_2, 0, 180);

  // get temperature reading from python
  if (Serial.available() > 0) {
    float out_temp = Serial.parseFloat();
    Serial.println(in_temp - out_temp);
    lcd.setCursor(4, 0);
    lcd.print(String(out_temp));

    // print time things
    h = Serial.parseInt();
    m = Serial.parseInt();
    lcd.setCursor(14, 0);
    lcd.print(m);
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

  sweep(servo_1, 180, 0);
  sweep(servo_2, 180, 0);
}

