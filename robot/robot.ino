/*
Drive forward and turn left or right when border is detected
  - Only reflectionsensor 0 and 5 are used.
*/
#include <PLab_ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

#define LED 13
#define LENGTH 20

/* setup distance sensor */
/* ultrasound distance pins */
const int echoPinF = 5;
const int echoPinB = 4;
const int triggerPinF = 6;
const int triggerPinB = 6;
float distanceB;
float distanceF;
float tempArray[LENGTH];
int counter = 0;
int dist;

const int turnMomentum = 8;


// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  1800 //

// these might need to be tuned for different motor types
#define REVERSE_SPEED     200 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     100
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     300 // ms

PLab_ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];

ZumoReflectanceSensorArray sensors;
float readDistance(int echoPin, int triggerPin);
int calcAverage(float liste[]);

void setup()
{
  Serial.begin(9600);

  /* setup distance sensor */
  pinMode(triggerPinF, OUTPUT);
  pinMode(triggerPinB, OUTPUT);
  pinMode(echoPinF, INPUT);
  pinMode(echoPinB, INPUT);

  sensors.init();
  button.waitForButton();
}

void loop()
{
  sensors.read(sensor_values);
  /*
  if ((distance = readDistance(echoPinF, triggerPin)) != 0) {
    Serial.print("front: ");
    Serial.println(distance);
  }
  if ((distance = readDistance(echoPinB, triggerPin)) != 0) {
    Serial.print("back: ");
    Serial.println(distance);
  }*/

  // Les distanse fra de to sensorene
  distanceF = readDistance(echoPinF, triggerPinF);
  distanceB = readDistance(echoPinB, triggerPinB);

  // legg til differanse til tempArray
  //tempArray[counter++] = distanceF - distanceB;
  //counter %= LENGTH;
  //dist = calcAverage(tempArray);

  if (distanceF > 30 || distanceF < -30 || distanceB > 30 || distanceB < -30 || (distanceF < 7 && distanceB < 7))
    dist = 0;
  else
    dist = distanceF - distanceB;


  Serial.println(dist);
  motors.setSpeeds(200,200);

  if (dist < -4) {
    motors.turnRight(200, 30);
  }
  else if (dist > 4) {
    motors.turnLeft(200, 30);
  }

  /*
  // Snu hvis kanten er truffet
  if (sensor_values[0] > QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (sensor_values[5] > QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // otherwise, go straight
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  */
}

/* readDistance: read distance measured by ultrasound */
float readDistance(int echoPin, int triggerPin)
{
  digitalWrite(triggerPin, LOW);
  delay(2);

  digitalWrite(triggerPin, HIGH);
  delay(10);

  digitalWrite(triggerPin, LOW);
  /* Measure duration from echo */
  long int duration = pulseIn(echoPin, HIGH, 30000);
  /* calculate distance */
  float distance = duration * 0.00017;
  return distance * 100;
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
