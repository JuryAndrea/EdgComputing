#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
const int transmitter = 12; 
int activated = 0;

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
  pinMode(transmitter, OUTPUT); 
  digitalWrite(transmitter, LOW); 
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
    if (measure.RangeMilliMeter < 200){
      activated = 1;
      Serial.println("LIGHT SCHRANKER INTRUSION SYSTEM ACTIVATED");
    } else {
      activated = 0;
    }
  } else {
    Serial.println(" out of range ");
  }
    
   if (activated == 1) {
    digitalWrite(transmitter, HIGH);
  } else {
    digitalWrite(transmitter, LOW);
  }
  Serial.println("Activated is");
  Serial.println(activated);
    
  // delay(2000);
}
