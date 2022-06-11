#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());

  tone(11, 300);
  
  for (int i = 0; i < EEPROM.length(); ++i) {
    EEPROM.write(i, 0);
  }

  noTone(11);
}

void loop() {

}
