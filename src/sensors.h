#ifndef SENSORS_H
#define SENSORS_H

#include "robot_state.h"

double kalmanFilter(double measurement, KalmanFilter *filter);
double measureDistance(int analogPin, KalmanFilter *kalmanFilterInstance = nullptr);
double measureFrontDistance(int analogPin, KalmanFilter *kalmanFilterInstance = nullptr);
void measureDistanceAllDirections();

#endif