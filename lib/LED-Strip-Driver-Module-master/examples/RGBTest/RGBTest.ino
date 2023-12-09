#include <LEDStripDriver.h>

// use two available GPIO pins from your board
// DIN=GPIO16, CIN=GPIO14 in this example
LEDStripDriver led = LEDStripDriver(16, 14);


void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  led.setColor(255, 0, 0); // RGB
  delay(1000);
  led.setColor("#00FF00"); // Hex String
  delay(1000);
  led.setColor(0x0000FF); // Hex
  delay(1000);
  led.setColor(); // turn off
  delay(1000);
}
