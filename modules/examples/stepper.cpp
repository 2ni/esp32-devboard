/*
 * Simple stepper code
 * using https://github.com/tyhenry/CheapStepper
 * pio lib install 834
 *
 * in1 - io1 (gpio16)
 * in2 - io2 (gpio04)
 * in3 - miso (gpio12)
 * in4 - mosi (gpio13)
 *
 */

#include <Arduino.h>
#include "base_functions.h"
#include <CheapStepper.h>

boolean moveClockWise = true;
CheapStepper stepper(16, 4, 12, 13);

void setup() {

  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

}

void loop() {
  // full rotation 4096 steps
  for (int s=0; s<1024; s++) {
    stepper.step(moveClockWise);
    int nStep = stepper.getStep();
    if (nStep%64==0) {
      DF("step: %i\n", nStep);
    }
  }
  // function off is not available in version provided by platformio.
  // check git
  // avoid consuming current
  stepper.off();
  delay(2000);
  moveClockWise = !moveClockWise;
}
