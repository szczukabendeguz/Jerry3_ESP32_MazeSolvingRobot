#ifndef MOTION_H
#define MOTION_H

void checkButton();
void drive(int motorSpeedLeft, int motorSpeedRight);
void forward();
void backward();
void stop();
void turnLeft(double desiredangle);
void turnRight(double desiredangle);
bool thereIsAWall(int direction);
void PidDrive(double distanceFromMiddle, int maxSpeed, bool isThereAWall);
void delayWithForwardWithAlignment(double delayTime, int maxSpeed);
void alignToFrontWallBeforeTurnIfThereIsOne(int forwardMaxSpeed);
void forwardWithAlignment(int maxSpeed);
void orientRobot(double desiredAngle);

#endif