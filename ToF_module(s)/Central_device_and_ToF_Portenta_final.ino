#include "Adafruit_VL53L0X.h"
#include <ArduinoBLE.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Pin set up
const int buzzer = A6; 
const int transReader = D5; 
const int fingerReader = D3; 

// People counter varibale initialization, to check change of state system
int peopleCounter = 0;
int peopleCounterComp = 0;

// Ensure camera activation command only once executed
int transMission = 0;
int btransMission = 0;

// Variable for detecting whether the Nano ToF was triggered
float nanoTrigger = 0;

// Keep track of time stamps for each device
long myTimeNano = -1;
long myTimePortenta = -1;
unsigned long myTime;

// Bluetooth device definition
BLEDevice peripheral;
BLECharacteristic camCharacteristic;

// Everything below here is Buzzer setup
#define  a3f    208     // 208 Hz
#define  b3f    233     // 233 Hz
#define  b3     247     // 247 Hz
#define  c4     261     // 261 Hz MIDDLE C
#define  c4s    277     // 277 Hz
#define  e4f    311     // 311 Hz    
#define  f4     349     // 349 Hz 
#define  a4f    415     // 415 Hz  
#define  b4f    466     // 466 Hz 
#define  b4     493     // 493 Hz 
#define  c5     523     // 523 Hz 
#define  c5s    554     // 554 Hz
#define  e5f    622     // 622 Hz  
#define  f5     698     // 698 Hz 
#define  f5s    740     // 740 Hz
#define  a5f    831     // 831 Hz 

#define rest    -1

int piezo = 9; // Connect your piezo buzzer to this pin or change it to match your circuit!
int led = LED_BUILTIN; 

volatile int beatlength = 100; // determines tempo
float beatseparationconstant = 0.3;

int threshold;

int a; // part index
int b; // song index
int c; // lyric index

// Parts 1 and 2 (Intro)

int song1_intro_melody[] =
{c5s, e5f, e5f, f5, a5f, f5s, f5, e5f, c5s, e5f, rest, a4f, a4f};

int song1_intro_rhythmn[] =
{6, 10, 6, 6, 1, 1, 1, 1, 6, 10, 4, 2, 10};

// Parts 3 or 5 (Verse 1)

int song1_verse1_melody[] =
{ rest, c4s, c4s, c4s, c4s, e4f, rest, c4, b3f, a3f,
  rest, b3f, b3f, c4, c4s, a3f, a4f, a4f, e4f,
  rest, b3f, b3f, c4, c4s, b3f, c4s, e4f, rest, c4, b3f, b3f, a3f,
  rest, b3f, b3f, c4, c4s, a3f, a3f, e4f, e4f, e4f, f4, e4f,
  c4s, e4f, f4, c4s, e4f, e4f, e4f, f4, e4f, a3f,
  rest, b3f, c4, c4s, a3f, rest, e4f, f4, e4f
};

int song1_verse1_rhythmn[] =
{ 2, 1, 1, 1, 1, 2, 1, 1, 1, 5,
  1, 1, 1, 1, 3, 1, 2, 1, 5,
  1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 3,
  1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 4,
  5, 1, 1, 1, 1, 1, 1, 1, 2, 2,
  2, 1, 1, 1, 3, 1, 1, 1, 3
};

// Parts 4 or 6 (Chorus)

int song1_chorus_melody[] =
{ b4f, b4f, a4f, a4f,
  f5, f5, e5f, b4f, b4f, a4f, a4f, e5f, e5f, c5s, c5, b4f,
  c5s, c5s, c5s, c5s,
  c5s, e5f, c5, b4f, a4f, a4f, a4f, e5f, c5s,
  b4f, b4f, a4f, a4f,
  f5, f5, e5f, b4f, b4f, a4f, a4f, a5f, c5, c5s, c5, b4f,
  c5s, c5s, c5s, c5s,
  c5s, e5f, c5, b4f, a4f, rest, a4f, e5f, c5s, rest
};

int song1_chorus_rhythmn[] =
{ 1, 1, 1, 1,
  3, 3, 6, 1, 1, 1, 1, 3, 3, 3, 1, 2,
  1, 1, 1, 1,
  3, 3, 3, 1, 2, 2, 2, 4, 8,
  1, 1, 1, 1,
  3, 3, 6, 1, 1, 1, 1, 3, 3, 3, 1, 2,
  1, 1, 1, 1,
  3, 3, 3, 1, 2, 2, 2, 4, 8, 4
};



void setup() {
  Serial.begin(115200);
  
  Serial.println("Setting up");
  // wait until serial port opens for native USB devices
  while (! Serial) {
    Serial.println("inwhile");
    delay(1);
  }
  
  // Set up of the ToF on the Portenta
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
  pinMode(buzzer, OUTPUT); // Set buzzer 
  pinMode(buzzer, LOW); // Reset buzzer 

  // Communication
  pinMode(transReader, INPUT);
  pinMode(fingerReader, INPUT);

  // Initialization of tones for buzzer
  a = 4;
  b = 0;
  c = 0;


  // EVERYTHING UNDERNEATH HERE IS BLUETOOTH CONNECTION SET UP:
  // INITALE BLUETOOTH CONNECTION SET UP WITH CAMERA

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  Serial.println("Bluetooth® Low Energy Central - Peripheral Explorer");

  // start scanning for peripherals
  BLE.scan();

  Serial.println("Before BLE peripheral");
  peripheral = BLE.available();
  Serial.println("AFRER BLE peripheral");

  while (peripheral.localName() != "CAMERA") {
    Serial.println("In While");
    peripheral = BLE.available();
    if (peripheral) {
      // discovered a peripheral, print out address, local name, and advertised service
      Serial.print("Found ");
      Serial.print(peripheral.address());
      Serial.print(" '");
      Serial.print(peripheral.localName());
      Serial.print("' ");
      Serial.print(peripheral.advertisedServiceUuid());
      Serial.println();

      // see if peripheral is a LED
      if (peripheral.localName() == "CAMERA") {
        // stop scanning
        BLE.stopScan();

        explorerPeripheral(peripheral);

      }
    }
  }
}


void loop() {

  // Measure the distance of the ToF
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    if (measure.RangeMilliMeter < 200){
      myTime = millis();
      myTimePortenta = setTimerPortenta(myTime);
    }
  } else {
    Serial.println(" out of range ");
  }

  // Read from Nano and set Timer
  nanoTrigger = digitalRead(transReader);  
  if (nanoTrigger == 1.0) {
    myTime = millis();
    myTimeNano = setTimerNano(myTime);
  } 

  // Apply people counting logic
  peopleCounterLogic();

  // Check for finger print event
  int fingerActivated = digitalRead(fingerReader);

  if (fingerActivated == 1) {
    peopleCounter = 0;
  }

  // check if system should be triggered or not
  if (peopleCounter > 0 && fingerActivated == 0) {
    play();
  } else {
    pinMode(buzzer, LOW);
    transMission = 0;
    btransMission = 0;
  }

  // seperate check to communicate to the camera only once
  if (peopleCounter > 0 && fingerActivated == 0) {
    if(transMission == 0) {
      camCharacteristic.writeValue((byte)(0x01));
      transMission = 1;
    }
  } else {
    if (btransMission == 0) {
      camCharacteristic.writeValue((byte)(0x00));
      btransMission = 1;
    }
  }

  // Check for change of system state
  if (peopleCounter != peopleCounterComp) {
    Serial.println("ACTIVITY DETECTED");
    Serial.println("Count of people is: ");
    Serial.println(peopleCounter);
    peopleCounterComp = peopleCounter;
  }
}

float setTimerPortenta(float millis) {
  if (myTimePortenta == -1) {
    return millis;
  } else if (millis - myTimePortenta > 3000) {
    return millis;
  }else if (myTimePortenta < millis) {
    return myTimePortenta;    
  } 
}


float setTimerNano(float millis) {
  if (myTimeNano == -1) {
    return millis;
  } else if (millis - myTimeNano > 3000) {
    return millis;
  }else if (myTimeNano < millis) {
    return myTimeNano;    
  } 
}


void peopleCounterLogic() {
  // If nano first triggered and then portenta triggered:
  if (myTimeNano == -1 || myTimePortenta == -1 ) {
    return;
  } else if(myTimeNano < myTimePortenta) {
    peopleCounter += 1;
    myTimeNano = -1;
    myTimePortenta = -1;
    
  } else if (myTimeNano > myTimePortenta) {
      if (peopleCounter <= 0) {
        myTimeNano = -1;
        myTimePortenta = -1;
      } else {
        peopleCounter -= 1;
        myTimeNano = -1;
        myTimePortenta = -1;
      }
  }
} 


void play() {
  int notelength;
  if (a == 1 || a == 2) {
    // intro
    notelength = beatlength * song1_intro_rhythmn[b];
    if (song1_intro_melody[b] > 0) {
      digitalWrite(led, HIGH);
      tone(buzzer, song1_intro_melody[b], notelength);
    }
    b++;
    if (b >= sizeof(song1_intro_melody) / sizeof(int)) {
      a++;
      b = 0;
      c = 0;
    }
  }
  else if (a == 3 || a == 5) {
    // verse
    notelength = beatlength * 2 * song1_verse1_rhythmn[b];
    if (song1_verse1_melody[b] > 0) {
      digitalWrite(led, HIGH);
      tone(buzzer, song1_verse1_melody[b], notelength);
      c++;
    }
    b++;
    if (b >= sizeof(song1_verse1_melody) / sizeof(int)) {
      a++;
      b = 0;
      c = 0;
    }
  }
  else if (a == 4 || a == 6) {
    // chorus
    notelength = beatlength * song1_chorus_rhythmn[b];
    if (song1_chorus_melody[b] > 0) {
      digitalWrite(led, HIGH);
      tone(buzzer, song1_chorus_melody[b], notelength);
      c++;
    }
    b++;
    if (b >= sizeof(song1_chorus_melody) / sizeof(int)) {
      a++;
      b = 0;
      c = 0;
    }
  }
  delay(notelength);
  noTone(buzzer);
  digitalWrite(led, LOW);
  delay(notelength * beatseparationconstant);
  if (a == 7) { // loop back around to beginning of song
    a = 1;
  }
}



void explorerPeripheral(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    // peripheral.disconnect();
    return;
  }
  camCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
  // read and print device name of peripheral
  Serial.println();
  Serial.print("Device name: ");
  Serial.println(peripheral.deviceName());
  Serial.print("Appearance: 0x");
  Serial.println(peripheral.appearance(), HEX);
  Serial.println();

  // loop the services of the peripheral and explore each
  for (int i = 0; i < peripheral.serviceCount(); i++) {
    BLEService service = peripheral.service(i);

    exploreService(service);
  }

  Serial.println();

  // we are done exploring, disconnect
  // Serial.println("Disconnecting ...");
  // peripheral.disconnect();
  // Serial.println("Disconnected");
}

void exploreService(BLEService service) {
  // print the UUID of the service
  Serial.print("Service ");
  Serial.println(service.uuid());

  // loop the characteristics of the service and explore each
  for (int i = 0; i < service.characteristicCount(); i++) {
    BLECharacteristic characteristic = service.characteristic(i);

    exploreCharacteristic(characteristic);
  }
}

void exploreCharacteristic(BLECharacteristic characteristic) {
  // print the UUID and properties of the characteristic
  Serial.print("\tCharacteristic ");
  Serial.print(characteristic.uuid());
  Serial.print(", properties 0x");
  Serial.print(characteristic.properties(), HEX);

  // check if the characteristic is readable
  if (characteristic.canRead()) {
    // read the characteristic value
    characteristic.read();

    if (characteristic.valueLength() > 0) {
      // print out the value of the characteristic
      Serial.print(", value 0x");
      printData(characteristic.value(), characteristic.valueLength());
    }
  }
  Serial.println();

  // loop the descriptors of the characteristic and explore each
  for (int i = 0; i < characteristic.descriptorCount(); i++) {
    BLEDescriptor descriptor = characteristic.descriptor(i);

    exploreDescriptor(descriptor);
  }
}

void exploreDescriptor(BLEDescriptor descriptor) {
  // print the UUID of the descriptor
  Serial.print("\t\tDescriptor ");
  Serial.print(descriptor.uuid());

  // read the descriptor value
  descriptor.read();

  // print out the value of the descriptor
  Serial.print(", value 0x");
  printData(descriptor.value(), descriptor.valueLength());

  Serial.println();
}

void printData(const unsigned char data[], int length) {
  for (int i = 0; i < length; i++) {
    unsigned char b = data[i];

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b, HEX);
  }
}


