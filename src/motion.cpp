#include "motion.h"
#include "audio.h"
#include "sensors.h"
#include "tuner.h"
#include "robot_state.h"

void checkButton()
{
  if (digitalRead(BUTTON_PIN) == HIGH || webButtonPressed)
  {
    webButtonPressed = false;
    stop();
    beep(3);
    delay(500);
    while (digitalRead(BUTTON_PIN) == LOW && !webButtonPressed)
    {
      delay(100);
      handlePidSettings();
      measureDistanceAllDirections();
    }
    {
      webButtonPressed = false;
      beep(2);
      delay(500);
      SPI.begin();
      mfrc522.PCD_Init();
      mfrc522.PCD_DumpVersionToSerial();
      mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    }
  }
}

void drive(int motorSpeedLeft, int motorSpeedRight)
{
  if (motorSpeedLeft >= 0)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    motorSpeedLeft = -motorSpeedLeft;
  }

  if (motorSpeedRight >= 0)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  else
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    motorSpeedRight = -motorSpeedRight;
  }

  analogWrite(ENA, motorSpeedLeft);
  analogWrite(ENB, motorSpeedRight);
  checkButton();
}

void forward()
{
  drive(80, 80);
}

void backward()
{
  drive(-80, -80);
}

void stop()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void turnLeft(double desiredangle)
{
  mpu.update();
  float startAngle = mpu.getAngleZ();
  float currentAngle = mpu.getAngleZ();
  float howFareAreWeFromDestinacion = 0;

  drive(-turnMaxSpeed, turnMaxSpeed);

  while (currentAngle <= startAngle + desiredangle)
  {
    mpu.update();
    measureDistanceAllDirections();
    currentAngle = mpu.getAngleZ();
    howFareAreWeFromDestinacion = ((startAngle + desiredangle) - currentAngle) / desiredangle;
    howFareAreWeFromDestinacion = howFareAreWeFromDestinacion * 2;
    drive(-constrain((turnMinSpeed + (turnProportionalSpeed * howFareAreWeFromDestinacion)), (int)turnMinSpeed, (int)turnMaxSpeed), constrain((turnMinSpeed + (turnProportionalSpeed * howFareAreWeFromDestinacion)), turnMinSpeed, turnMaxSpeed));
  }
  drive(80, -80);
  delay(60);
  stop();

  mpu.update();
  lastCorrectAngle = mpu.getAngleZ();
}

void turnRight(double desiredangle)
{
  mpu.update();
  float startAngle = mpu.getAngleZ();
  float currentAngle = mpu.getAngleZ();
  float howFareAreWeFromDestinacion = 0;

  drive(turnMaxSpeed, -turnMaxSpeed);
  while (currentAngle >= startAngle - desiredangle)
  {
    mpu.update();
    measureDistanceAllDirections();
    currentAngle = mpu.getAngleZ();
    howFareAreWeFromDestinacion = (currentAngle - (startAngle - desiredangle)) / desiredangle;
    howFareAreWeFromDestinacion = howFareAreWeFromDestinacion * 2;
    drive(constrain((turnMinSpeed + (turnProportionalSpeed * howFareAreWeFromDestinacion)), (int)turnMinSpeed, (int)turnMaxSpeed), -constrain((turnMinSpeed + (turnProportionalSpeed * howFareAreWeFromDestinacion)), turnMinSpeed, turnMaxSpeed));
  }

  drive(-80, 80);
  delay(60);

  stop();
  mpu.update();
  lastCorrectAngle = mpu.getAngleZ();
}

bool thereIsAWall(int direction)
{
  if (distances[direction] >= 1 && distances[direction] <= (distanceFromSingleWall + distanceFromSingleWallTreshold))
    return true;
  else
    return false;
}

void PidDrive(double distanceFromMiddle, int maxSpeed, bool isThereAWall)
{
  input = distanceFromMiddle;
  pid.Compute();
  int motorSpeedLeft = constrain(maxSpeed - output, motorMinSpeed, motorMaxSpeed);
  int motorSpeedRight = constrain(maxSpeed + output, motorMinSpeed, motorMaxSpeed);

  drive(motorSpeedLeft, motorSpeedRight);

  if (isThereAWall)
  {
    lastCorrectAngle = mpu.getAngleZ();
  }
}

void delayWithForwardWithAlignment(double delayTimeDouble, int maxSpeed)
{
  unsigned long startTime = millis();
  int delayTime = int(delayTimeDouble);
  while (millis() - startTime < delayTime)
  {
    measureDistanceAllDirections();
    forwardWithAlignment(maxSpeed);
  }
}

void alignToFrontWallBeforeTurnIfThereIsOne(int forwardMaxSpeedValue)
{
  if (distances[DIRECTION_FRONT] <= 20)
  {
    while (distances[DIRECTION_FRONT] >= distanceFromFrontWall)
    {
      measureDistanceAllDirections();
      forwardWithAlignment(forwardMaxSpeedValue);
    }
  }
}

void forwardWithAlignment(int maxSpeed)
{
  if (thereIsAWall(DIRECTION_LEFT) && thereIsAWall(DIRECTION_RIGHT))
  {
    double distanceFromMiddle = (distances[DIRECTION_RIGHT] - distances[DIRECTION_LEFT]);
    if (distances[DIRECTION_LEFT] < distances[DIRECTION_RIGHT])
    {
      distanceFromMiddle = (distanceFromSingleWall - distances[DIRECTION_LEFT]) * 2;
    }
    else
    {
      distanceFromMiddle = (distances[DIRECTION_RIGHT] - distanceFromSingleWall) * 2;
    }

    PidDrive(distanceFromMiddle, maxSpeed, true);
  }
  if (thereIsAWall(DIRECTION_LEFT) && !thereIsAWall(DIRECTION_RIGHT))
  {
    double distanceFromMiddle = (distanceFromSingleWall - distances[DIRECTION_LEFT]) * 2;
    PidDrive(distanceFromMiddle, maxSpeed, true);
  }
  if (!thereIsAWall(DIRECTION_LEFT) && thereIsAWall(DIRECTION_RIGHT))
  {
    double distanceFromMiddle = (distances[DIRECTION_RIGHT] - distanceFromSingleWall) * 2;
    PidDrive(distanceFromMiddle, maxSpeed, true);
  }
  if (!thereIsAWall(DIRECTION_LEFT) && !thereIsAWall(DIRECTION_RIGHT))
  {
    float angle = mpu.getAngleZ();
    double error = (angle - lastCorrectAngle) * 0.2;
    PidDrive(error, maxSpeed, false);
  }
}

void orientRobot(double desiredAngle)
{
  mpu.update();
  if (abs(mpu.getAngleZ() - desiredAngle) > 10)
  {
    if (mpu.getAngleZ() > desiredAngle)
    {
      turnRight((mpu.getAngleZ() - desiredAngle) / 2);
    }
    else
    {
      turnLeft(abs(mpu.getAngleZ() - desiredAngle) / 2);
    }
  }
}