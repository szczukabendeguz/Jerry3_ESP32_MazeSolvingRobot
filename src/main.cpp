#include <Arduino.h>
#include <MPU6050_light.h>
#include <SPI.h>
#include <MFRC522.h>
#include <string.h>
#include <stdio.h>
#include <WiFi.h>
#include <PID_v1.h>
#include <math.h>
#include <EEPROM.h>

#include "audio.h"
#include "motion.h"
#include "rfid.h"
#include "robot_state.h"
#include "sensors.h"
#include "tuner.h"

MPU6050 mpu(Wire);
unsigned long timer = 0;
float lastCorrectAngle = 0;

KalmanFilter frontKalman = {0, 1.0, 0.01, 0.1};
KalmanFilter leftKalman = {0, 1.0, 0.01, 0.1};
KalmanFilter rightKalman = {0, 1.0, 0.01, 0.1};

bool webButtonPressed = false;

double distanceFromSingleWall = 10;
double distanceFromSingleWallTreshold = distanceFromSingleWall / 2;
double distanceFromFrontWall = 10;

double distances[3];
int commands[256];
int currentCommand;

int motorMaxSpeed = 255;
int motorMinSpeed = -50;
int forwardMaxSpeed = 180;
int forwardMinSpeed = 120;
int forwardProportionalSpeed = forwardMaxSpeed - forwardMinSpeed;

int turnMaxSpeed = 180;
int turnMinSpeed = 120;
int turnProportionalSpeed = turnMaxSpeed - turnMinSpeed;
double delayBeforeTurn = 700;

int pidmode = 2;
double setpoint = 0;
double input, output;
double Pid_P = 1, Pid_I = 0, Pid_D = 0;
PID pid(&input, &output, &setpoint, Pid_P, Pid_I, Pid_D, DIRECT);

const char *ssid = "Jerry0_3";
const char *password = "Pass1234$";

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup()
{
  delay(100);
  pinMode(BUZZER_PIN, OUTPUT);

  // Soros kommunikáció inicializálása
  Serial.begin(115200);
  startupTone();
  // PID webinterface inicializálása és indítása
  Serial.println("\n\nPID Webinterface indítása...");
  setupPidWebInterface(ssid, password);
  Serial.println("PID Webinterface elindult");
  Serial.println("PID Webinterface elérhető a következő címen: ");
  Serial.println(WiFi.localIP());

  // gyro beállítása
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0)
  {
  } // stop everything if could not connect to MPU6050

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(100);
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");

  // Motorvezérlő pin-ek beállítása
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(BUTTON_PIN, OUTPUT); // Gomb bemenetének beállítása

  // RFID kártyaolvasó inicializálása

  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial(); // 0x07 = 48dB
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(-255, 255);
  pid.SetSampleTime(25);

  mpu.update();
  lastCorrectAngle = mpu.getAngleZ();

  commands[0] = 0;
  currentCommand = 0;

  delay(100);
  beep(3);
  // várunk a gomb megnyomására
  while (digitalRead(BUTTON_PIN) == LOW)
  {
    measureDistanceAllDirections();
    handlePidSettings();
  }
  {
    beep(3);
    delay(500);
  }
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

void loop()
{
  // PID webinterface kezelése
  measureDistanceAllDirections();
  handlePidSettings();
  while (distances[DIRECTION_FRONT] > distanceFromFrontWall)
  {
    measureDistanceAllDirections();
    handlePidSettings();
    forwardWithAlignment(forwardMaxSpeed);
    mpu.update();
    switch (rfidToDirection(commands))
    {
    case DIRECTION_LEFT:
      delayWithForwardWithAlignment(delayBeforeTurn, forwardMaxSpeed);
      turnLeft(85);
      break;
    case DIRECTION_RIGHT:
      delayWithForwardWithAlignment(delayBeforeTurn, forwardMaxSpeed);
      turnRight(85);
      break;
    case DIRECTION_STOP:
      stop();

      finishTone();
      drive(200, -200);
      delay(2000);
      stop();
      while (digitalRead(BUTTON_PIN) == LOW)
      {
        delay(100);
      }
      SPI.begin();
      mfrc522.PCD_Init();
      break;
    case DIRECTION_START:
      beepWithForwardWithAlighnment(4);
      break;
    default:
      break;
    }
  }
  stop();
  if (distances[DIRECTION_LEFT] >= distances[DIRECTION_RIGHT])
  {
    turnLeft(85);
    for (size_t i = 0; i < 40; i++)
    {
      measureDistanceAllDirections();
    }
    drive(200, 200);
  }
  else
  {
    turnRight(85);
    for (size_t i = 0; i < 40; i++)
    {
      measureDistanceAllDirections();
    }
    drive(200, 200);
  }
}

