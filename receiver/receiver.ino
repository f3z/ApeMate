#include "Device.h"

#include "LightController.h"

LightController controller;

void setup() {
  Serial.begin(115200);
  controller.Setup();
  // Initialise to green
  controller.SetColour(0);
}
 
void loop() {
    if(Serial.available()){
      uint8_t colour = Serial.read();
//      Serial.print("I received: ");
      Serial.println(colour, DEC);
      controller.SetColour(colour);
//      Serial.write(colour);
    }
  
    controller.DoIteration();
      /* Approx colour cycle rate of 0.5Hz. One colour cycle
     * takes 1024 loops, so each loop should take approx
     * 1.953125ms.
     */
    delay(2);
}
