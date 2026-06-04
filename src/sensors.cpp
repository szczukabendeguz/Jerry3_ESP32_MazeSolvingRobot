#include "sensors.h"
#include <math.h>

double kalmanFilter(double measurement, KalmanFilter *filter)
{
  double predictedEstimate = filter->estimate;
  double predictedErrorCovariance = filter->errorCovariance + filter->processNoise;

  double kalmanGain = predictedErrorCovariance / (predictedErrorCovariance + filter->measurementNoise);
  filter->estimate = predictedEstimate + kalmanGain * (measurement - predictedEstimate);
  filter->errorCovariance = (1 - kalmanGain) * predictedErrorCovariance;

  return filter->estimate;
}

double measureDistance(int analogPin, KalmanFilter *kalmanFilterInstance)
{
  int rawValue = analogRead(analogPin);

  const int lookupSize = 11;
  const int analogValues[lookupSize] = {2500, 1500, 880, 600, 450, 405, 370, 330, 300, 150, 100};
  const double distanceLookup[lookupSize] = {1.0, 2.5, 5.0, 7.5, 10.0, 12.5, 15.0, 17.5, 20.0, 35.0, 45.0};

  double rawDistance = 0;
  if (rawValue < 100)
  {
    rawDistance = 50.0;
  }
  else if (rawValue > 2500)
  {
    rawDistance = 1.0;
  }
  else
  {
    for (int i = 0; i < lookupSize - 1; i++)
    {
      if (rawValue <= analogValues[i] && rawValue > analogValues[i + 1])
      {
        double proportion = (double)(rawValue - analogValues[i + 1]) / (analogValues[i] - analogValues[i + 1]);
        rawDistance = distanceLookup[i] + (1.0 - proportion) * (distanceLookup[i + 1] - distanceLookup[i]);
        break;
      }
    }

    if (rawDistance == 0)
    {
      rawDistance = 15.0;
    }
  }

  if (kalmanFilterInstance != nullptr)
  {
    return kalmanFilter(rawDistance, kalmanFilterInstance);
  }

  return rawDistance;
}

double measureFrontDistance(int analogPin, KalmanFilter *kalmanFilterInstance)
{
  int rawValue = analogRead(analogPin);

  const int lookupSize = 8;
  const int analogValues[lookupSize] = {2000, 1200, 750, 500, 375, 300, 200, 150};
  const double distanceLookup[lookupSize] = {5, 10, 15, 20, 25, 30, 35, 40};

  double rawDistance = 0;
  if (rawValue < 150)
  {
    rawDistance = 70.0;
  }
  else if (rawValue > 2000)
  {
    rawDistance = 5.0;
  }
  else
  {
    for (int i = 0; i < lookupSize - 1; i++)
    {
      if (rawValue <= analogValues[i] && rawValue > analogValues[i + 1])
      {
        double proportion = (double)(rawValue - analogValues[i + 1]) / (analogValues[i] - analogValues[i + 1]);
        rawDistance = distanceLookup[i + 1] + proportion * (distanceLookup[i] - distanceLookup[i + 1]);
        break;
      }
    }

    if (rawDistance == 0)
    {
      rawDistance = 60000.0 * pow(rawValue, -1.05) + 3.0;
    }
  }

  if (kalmanFilterInstance != nullptr)
  {
    return kalmanFilter(rawDistance, kalmanFilterInstance);
  }

  return rawDistance;
}

void measureDistanceAllDirections()
{
  distances[DIRECTION_FRONT] = measureFrontDistance(IR_PIN_FRONT, &frontKalman);
  distances[DIRECTION_RIGHT] = measureDistance(IR_PIN_RIGHT, &rightKalman);
  distances[DIRECTION_LEFT] = measureDistance(IR_PIN_LEFT, &leftKalman);
}