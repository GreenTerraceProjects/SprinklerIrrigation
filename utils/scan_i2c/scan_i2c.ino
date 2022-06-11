#include <Wire.h>

void setup() {
  Serial.begin(9600);
  delay(1000);  
  
  Serial.println("I2C scanning ...");
  Wire.begin();

  byte error, address;
  int numDevices;
  numDevices = 0;
  for (address = 0; address < 128; ++address) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C Address: 0x");
      Serial.println(address, HEX);

      ++numDevices;
    }
  }

  Serial.print("Total devices: ");
  Serial.println(numDevices);

  // Found 0x27 as address of the I2C board.
}

void loop() {

}
