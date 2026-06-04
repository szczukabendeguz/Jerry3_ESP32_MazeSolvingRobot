#ifndef ROBOT_STATE_H
#define ROBOT_STATE_H

#include <Arduino.h>
#include <WiFi.h>
#include <MPU6050_light.h>
#include <SPI.h>
#include <MFRC522.h>
#include <PID_v1.h>

constexpr int DIRECTION_FRONT = 0;
constexpr int DIRECTION_LEFT = 1;
constexpr int DIRECTION_RIGHT = 2;
constexpr int DIRECTION_STOP = 3;
constexpr int DIRECTION_START = 4;
constexpr int DIRECTION_DEAD_END = 5;

constexpr int BUZZER_PIN = 12;
constexpr int BUTTON_PIN = 2;

constexpr int IR_PIN_LEFT = 34;
constexpr int IR_PIN_RIGHT = 36;
constexpr int IR_PIN_FRONT = 39;

constexpr int ENB = 14;
constexpr int IN4 = 27;
constexpr int IN3 = 16;
constexpr int IN2 = 17;
constexpr int IN1 = 25;
constexpr int ENA = 26;

constexpr int RST_PIN = 13;
constexpr int SS_PIN = 5;

struct KalmanFilter
{
  double estimate;
  double errorCovariance;
  double processNoise;
  double measurementNoise;
};

extern MPU6050 mpu;
extern unsigned long timer;
extern float lastCorrectAngle;

extern KalmanFilter frontKalman;
extern KalmanFilter leftKalman;
extern KalmanFilter rightKalman;

extern double distanceFromSingleWall;
extern double distanceFromSingleWallTreshold;
extern double distanceFromFrontWall;

extern bool webButtonPressed;
extern double distances[3];
extern int commands[256];
extern int currentCommand;

extern int motorMaxSpeed;
extern int motorMinSpeed;
extern int forwardMaxSpeed;
extern int forwardMinSpeed;
extern int forwardProportionalSpeed;

extern int turnMaxSpeed;
extern int turnMinSpeed;
extern int turnProportionalSpeed;
extern double delayBeforeTurn;

extern int pidmode;
extern double setpoint;
extern double input;
extern double output;
extern double Pid_P;
extern double Pid_I;
extern double Pid_D;
extern PID pid;

extern const char *ssid;
extern const char *password;

extern MFRC522 mfrc522;
extern MFRC522::MIFARE_Key key;

#endif