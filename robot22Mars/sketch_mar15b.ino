#include <PLab_ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <SoftwareSerial.h>
#include <PLabBTSerial.h>



#define LED 13
#define LENGTH 10

/* setup distance sensor */
/* ultrasound distance pins */
float turnMomentum = 1;
const int echoPinF = 5;
const int echoPinB = 3;
const int triggerPinF = 6;
const int triggerPinB = 6;
const int BT_RX = 4;  // Connect to RXD on Bluetooth unit
const int BT_TX = 2;  // Connect to TXD on Bluetooth unit
float distanceB;
int index=0;
bool beastMode;
float distanceF;
float tempArray[LENGTH];
int counter = 0;
int dist;
bool left;

PLabBTSerial btSerial (BT_TX, BT_RX);


// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  200 //

// these might need to be tuned for different motor types
#define REVERSE_SPEED     400 // 0 is stopped, 400 is full speed
#define TURN_SPEED        400
#define FORWARD_SPEED     400
#define REVERSE_DURATION  250 //ms
#define TURN_DURATION     350 // ms

PLab_ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];

ZumoReflectanceSensorArray sensors;
float readDistance(int echoPin, int triggerPin);
int calcAverage(float liste[]);

void setup()
{
  left=false;
  Serial.begin(9600);
  beastMode=true;
  /* setup distance sensor */
  pinMode(triggerPinF, OUTPUT);
  pinMode(triggerPinB, OUTPUT);
  pinMode(echoPinF, INPUT);
  pinMode(echoPinB, INPUT);
    btSerial.begin (9600);
  sensors.init();
  beastMode=readBT();
}

bool readBT()
{
  bool btnPressed=false;
  while(!btnPressed)
  {
     int availableCount = btSerial.available();
  if (availableCount > 0) {
    char text[availableCount];
    
    btSerial.read(text, availableCount);
    
    readCommand(text);
    Serial.println(text);
  }
  // Hvis vi har lyst til aa kunne skrive kommandoer i konsollvinduet, tar vi med dette
  // If we want to be able to write commands in the console window, we include this
  if (Serial.available () > 0) {
    btSerial.write (Serial.read ());
  }
  btnPressed=button.isPressed();
  delay(50);
  }
  return beastMode;
}
void checkLine(bool iChanged, bool changeMomentum)
{
  sensors.read(sensor_values);
  // Snu hvis kanten er truffet
  if (sensor_values[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (sensor_values[4] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if(!iChanged && changeMomentum)
  {
    // otherwise, go straight
    turnMomentum = 1;
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}
void beastModeF()
{
  motors.setSpeeds(0,0);
}
void notBeastMode()
{
  bool iChanged=false;
  sensors.read(sensor_values);
  distanceF = readDistance(echoPinF, triggerPinF, false);
  distanceB = readDistance(echoPinB, triggerPinB, false);

  if(distanceF>60 || distanceF<1)
    distanceF=1000;
  if(distanceB>60 || distanceB<1)
    distanceB=1000;
  Serial.print("Distance B - distance F: ");
  Serial.println(distanceB-distanceF);
  if(turnMomentum<0.4)
  {
    turnMomentum=0.3;
  }
  if(distanceB-distanceF>5.0)
  {
    if(left)
    {
      left=false;
      turnMomentum=1;
    }
    turnMomentum -= 0.22;
    motors.setSpeeds(FORWARD_SPEED , FORWARD_SPEED *turnMomentum);
    iChanged=true;
  }
  else if(distanceF-distanceB>5.0)
  {
    if(!left)
    {
      left=true;
      turnMomentum=1;
    }
    turnMomentum -= 0.22;
    motors.setSpeeds(FORWARD_SPEED *turnMomentum, FORWARD_SPEED );
    iChanged=true;
  }
  checkLine(iChanged,true);
}
void loop()
{
  
  if(beastMode)
  {
    beastModeF();
  }
  else
  {
    notBeastMode();
  }
}

/* readDistance: read distance measured by ultrasound */
float readDistance(int echoPin, int triggerPin, bool iChanged)
{
  digitalWrite(triggerPin, LOW);
  delay(2);

  digitalWrite(triggerPin, HIGH);
  for(int i =0; i < 3; i++)
  {
    checkLine(iChanged,false);
    delay(3);
  }

  digitalWrite(triggerPin, LOW);
  /* Measure duration from echo */
  long int duration = pulseIn(echoPin, HIGH, 30000);
  /* calculate distance */
  float distance = duration * 0.00017;
  return distance * 100;
}

void enableBeastMode()
{
  beastMode = !beastMode;
}

void readCommand (char *text) {
  Serial.print ("Mottok: ");
  Serial.println (text);
  if (0 == strcmp ("ON", text)) {
    Serial.println("Lys paa");
    enableBeastMode();
  } else if (0 == strcmp("OFF", text)) {
    Serial.println("Lys av");
  } else {
    // Dette skal ikke skje, saa vi kan si fra at det har skjedd
    // This should not happen, so we can tell that it did
    Serial.print ("Ukjent kommando!");
  }
}
/* calcAverage: find the integer average of elements in list */
int calcAverage(float list[])
{
  float average = 0;
  int i;
  for (i = 0; i < LENGTH; i++)
    average += list[i];
  return (int) (average / LENGTH);
}
