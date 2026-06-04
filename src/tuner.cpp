#include "tuner.h"
#include <ESP32VariableTuner.h>
#include "audio.h"
#include "robot_state.h"

static VarMonitor monitor;
static bool tunerVariablesRegistered = false;

static void registerTunerVariables()
{
  if (tunerVariablesRegistered)
  {
    return;
  }

  monitor.enableAutoLoad(true);

  monitor.registerDouble(&Pid_P, "P (Proportional)", "PID", 0.0, 200.0, 0.1);
  monitor.registerDouble(&Pid_I, "I (Integral)", "PID", 0.0, 200.0, 0.1);
  monitor.registerDouble(&Pid_D, "D (Derivative)", "PID", 0.0, 200.0, 0.1);

  monitor.registerDouble(&distanceFromSingleWall, "Faltól tartott távolság", "Motion", 0.0, 50.0, 0.1);
  monitor.registerDouble(&distanceFromFrontWall, "Előre tartott távolság", "Motion", 0.0, 50.0, 0.1);
  monitor.registerDouble(&delayBeforeTurn, "Időzítés", "Motion", 0.0, 5000.0, 10.0);
  monitor.registerInt(&forwardMaxSpeed, "Maximális sebesség", "Motion", 0, 255);
  monitor.registerInt(&turnMinSpeed, "Minimális fordulási sebesség", "Motion", 0, 255);
  monitor.registerInt(&turnMaxSpeed, "Maximális fordulási sebesség", "Motion", 0, 255);
  monitor.registerInt(&motorMaxSpeed, "Maximális motor sebesség", "Motion", 0, 300);
  monitor.registerInt(&motorMinSpeed, "Minimális motor sebesség", "Motion", -300, 300);
  monitor.registerBool(&webButtonPressed, "Web gomb", "Control");

  monitor.registerDouble(&distances[0], "Előre mért távolság", "Telemetry", 0.0, 100.0, 0.1, true);
  monitor.registerDouble(&distances[1], "Bal mért távolság", "Telemetry", 0.0, 100.0, 0.1, true);
  monitor.registerDouble(&distances[2], "Jobb mért távolság", "Telemetry", 0.0, 100.0, 0.1, true);

  tunerVariablesRegistered = true;
}

void setupPidWebInterface(const char *ssid, const char *password)
{
  registerTunerVariables();
  monitor.begin(ssid, password, true);

  Serial.println("ESP32VariableTuner elindult");
  Serial.print("Web UI IP: ");
  Serial.println(monitor.getIPAddress());
  beep(1);
}

void handlePidWebClient()
{
  monitor.handle();
}

void handlePidSettings()
{
  handlePidWebClient();
  pid.SetTunings(Pid_P, Pid_I, Pid_D);
}