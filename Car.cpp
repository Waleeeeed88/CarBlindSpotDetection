#include <SD.h>
#include <TMRpcm.h>

#define TRIG_PIN1 10
#define ECHO_PIN1 12
#define TRIG_PIN2 7
#define ECHO_PIN2 6
#define LED_PIN 13
#define SOUND_SPEED 0.034
#define SD_ChipSelectPin 53
#define motor1Pin1 22
#define motor1Pin2 24
#define motor2Pin1 34
#define motor2Pin2 36
#define enableAPin 26
#define enableBPin 38

TMRpcm sound;
unsigned long lastSoundTime = 0;
const unsigned long soundCooldown = 5000;  // Cooldown time in milliseconds (5 seconds)
boolean shouldRunMotor1 = true;
boolean shouldRunMotor2 = true;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card initialized.");
  
  sound.speakerPin = 11;
  sound.quality(1);
  sound.setVolume(6);
}

void loop() {
  // Sensor 1 controls Motor 1
  digitalWrite(TRIG_PIN1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN1, LOW);
  long duration1 = pulseIn(ECHO_PIN1, HIGH);
  float distance1 = duration1 * SOUND_SPEED / 2;

  // Sensor 2 controls Motor 2
  digitalWrite(TRIG_PIN2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN2, LOW);
  long duration2 = pulseIn(ECHO_PIN2, HIGH);
  float distance2 = duration2 * SOUND_SPEED / 2;

  // Check for proximity and control sound
  boolean playSound = false;
  if (distance1 <= 5 || distance2 <= 5) {
    digitalWrite(LED_PIN, HIGH);
    playSound = true;
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // Control sound based on cooldown
  unsigned long currentTime = millis();
  if (playSound && currentTime - lastSoundTime > soundCooldown) {
    sound.play("airhorn.wav");
    lastSoundTime = currentTime;
  }

  // Control Motor 1
  shouldRunMotor1 = distance1 > 5;
  if (shouldRunMotor1) {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    analogWrite(enableAPin, 255);
  } else {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    analogWrite(enableAPin, 0);
  }

  // Control Motor 2
  shouldRunMotor2 = distance2 > 5;
  if (shouldRunMotor2) {
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    analogWrite(enableBPin, 255);
  } else {
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
    analogWrite(enableBPin, 0);
  }

  delay(300);
}
