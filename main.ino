#include <Servo.h>

Servo servo;


const int trigPin = 13;
const int echoPin = 12;
const int servoPin = 11;
const int enAPin = 6;
const int in1Pin = 7;
const int in2Pin = 5;
const int in3Pin = 4;
const int in4Pin = 2;
const int enBPin = 3;
const int ldrPin = A0;
const int lightPin = 10;
const int claPin = 9;  // Pin per il clacson

enum Motor { LEFT, RIGHT };

static unsigned char angleIndex = 0;

void go(Motor m, int speed) {
  digitalWrite(m == LEFT ? in1Pin : in3Pin, speed > 0 ? HIGH : LOW);
  digitalWrite(m == LEFT ? in2Pin : in4Pin, speed <= 0 ? HIGH : LOW);
  analogWrite(m == LEFT ? enAPin : enBPin, speed < 0 ? -speed : speed);
}



void testMotors() {

  static int speed[8] = {128, 255, 128, 0, -128, -255, -128, 0};
  go(RIGHT, 0);
  for (unsigned char i = 0; i < 8; i++) {
    go(LEFT, speed[i]);
    delay(200);
  }

  for (unsigned char i = 0; i < 8; i++) {
    go(RIGHT, speed[i]);
    delay(200);
  }

}



unsigned int readDistance() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long period = pulseIn(echoPin, HIGH);
  return period * 343 / 2000;
}



#define NUM_ANGLES 13
unsigned char sensorAngle[NUM_ANGLES] = {60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180};
unsigned int distance[NUM_ANGLES];



void readNextDistance() {
  static unsigned char angleIndex = 0;
  static signed char step = 1;
  distance[angleIndex] = readDistance();

  
  angleIndex += step;
  if (angleIndex == NUM_ANGLES - 1) step = -1;
  else if (angleIndex == 0) step = 1;
  servo.write(sensorAngle[angleIndex]);
}



void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(claPin, OUTPUT);    // Inizializza il pin del clacson come output
  digitalWrite(trigPin, LOW);
  pinMode(enAPin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enBPin, OUTPUT);
  servo.attach(servoPin);
  pinMode(lightPin, OUTPUT);
  pinMode(claPin, OUTPUT);    // Inizializza il pin del clacson come output
  servo.write(90);
  go(LEFT, 0);
  go(RIGHT, 0);
  testMotors();
  servo.write(sensorAngle[0]);
  delay(200);

  for (unsigned char i = 0; i < NUM_ANGLES; i++) {
    readNextDistance();
    delay(200);
  }
}

void loop() {
  int ldrValue = analogRead(ldrPin);

  if (ldrValue < 3) {
    digitalWrite(lightPin, HIGH);
  } else {
    digitalWrite(lightPin, LOW);
  }

  // Leggi la distanza e inviala tramite Bluetooth
  readNextDistance();

  unsigned char tooClose = 0;
  for (unsigned char i = 0; i < NUM_ANGLES; i++) {
    if (distance[i] < 300) tooClose = 1;
  }
  
  if (tooClose) {
    digitalWrite(claPin, HIGH);
    delay(10);
    digitalWrite(claPin, LOW);
    go(LEFT, -180);
    go(RIGHT, -80);
  } else {
    go(LEFT, 255);
    go(RIGHT, 255);
  }

  delay(50); // Potresti regolare questo delay a seconda delle tue necessità
}
