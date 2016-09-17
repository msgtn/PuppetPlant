// Grove temperature things
#include <math.h>
int a;
float in_temp;
int B=3975;                  //B value of the thermistor
float resistance;
// LCD display
#include <SPI.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Out");
  lcd.setCursor(0,1);
  lcd.print("In");
  Serial.begin(9600);
}

void loop() {
  // get current temperature
  a=analogRead(A2);
  resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
  in_temp=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet&nbsp;;
  delay(1000);
  
  if (Serial.available() > 0) {
    float out_temp = Serial.parseFloat();
    Serial.println(in_temp - out_temp);
    lcd.setCursor(4, 0);
    lcd.print(String(out_temp));
    lcd.setCursor(4, 1);
    lcd.print(String(in_temp));
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}
