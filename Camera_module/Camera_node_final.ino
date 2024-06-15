//Francesco Huber

#include "camera.h"
#include "himax.h"
#include <Stepper.h>

//CAMERA
HM01B0 himax;
Camera cam(himax);
#define IMAGE_MODE CAMERA_GRAYSCALE
FrameBuffer fb(320,240,2);

#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int ledPin = LED_BUILTIN; // pin to use for the LED

//STEPPER
//More info at: https://lastminuteengineers.com/28byj48-stepper-motor-arduino-tutorial/
const int stepsPerRevolution = 2038;  // change this to fit the number of steps per revolution
// for your motor
#define STEPS 2038
Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5);

//pin
const int activatePin = D0;
float value = 0.0; 

unsigned long lastUpdate = 0;

bool flag = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set camera pin to output mode
  pinMode(ledPin, OUTPUT);



  //CAMERA
  Serial.begin(921600);
  cam.begin(CAMERA_R320x240, IMAGE_MODE, 30);

  //STEPPER
  // set the speed at 60 rpm:
  myStepper.setSpeed(10);
  // initialize the serial port:
  //Serial.begin(9600);

  //pin
  pinMode(activatePin, INPUT);
  pinMode(activatePin, HIGH);

  // begin BLE initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("CAMERA");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE CAMERA Peripheral");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    Serial.println(central.localName());


    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the camera:
      //if (switchCharacteristic.written()) {
        if (switchCharacteristic.value() == (byte)0x01) {   
          digitalWrite(ledPin, HIGH); 
          activateLoop();      
        } else {                             
          digitalWrite(ledPin, LOW);     
        }
      //}
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

//Camera activation
void runCamera() {

    cam.setStandby(true);

      if(!Serial) {    
      Serial.begin(921600);
      while(!Serial);
    }
    
    // Time out after 2 seconds and send new data
    //bool timeoutDetected = millis() - lastUpdate > 2000;
    bool timeoutDetected = false; //TEST

    // Wait until the receiver acknowledges
    // that they are ready to receive new data
    if(!timeoutDetected && Serial.read() != 1) return;

    lastUpdate = millis();
    
    // Grab frame and write to serial
    if (cam.grabFrame(fb, 3000) == 0) {
      Serial.write(fb.getBuffer(), cam.frameSize());
    }
    cam.setStandby(false);

    if(switchCharacteristic.value() == (byte)0x00) { return; }
}

//Debug
void runStepper(bool forward) {
    //STEPPER
  // step one revolution  in one direction:
  //Serial.println("clockwise");
  if(forward){
    myStepper.step(stepsPerRevolution);
    delay(500);
  } else {
    // step one revolution in the other direction:
    //Serial.println("counterclockwise");
    myStepper.step(-stepsPerRevolution);
    delay(500);
  }
}

//Debug
void runStepperClockwise() {
    myStepper.step(stepsPerRevolution/8); //169,833 for 30 deg
    delay(500);
}

//Debug
void runStepperCounterClockwise() {
    myStepper.step(-stepsPerRevolution/8);
    delay(500);
}

//Run stepper for x degrees
void runStepperDeg(int deg) {
    myStepper.step(stepsPerRevolution/(360/deg)); //169,833 for 30 deg
    delay(500);
}

// Main logic for camera operation
// The code was lest is such a state to , 1 be more clear, 2 easily adapt custom rotations / configuraions
// I apologize for the terrible look of this :)
void activateLoop() {
  runCamera();
  runStepperDeg(30);

  runCamera();
  runStepperDeg(30);

  runCamera();
  runStepperDeg(30);

  runCamera();
  runStepperDeg(30);

  runCamera();
  runStepperDeg(30);

  runCamera();
  runStepperDeg(-30);

  runCamera();
  runStepperDeg(-30);

  runCamera();
  runStepperDeg(-30);

  runCamera();
  runStepperDeg(-30);

  runCamera();
  runStepperDeg(-30);

}



