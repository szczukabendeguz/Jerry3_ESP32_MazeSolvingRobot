#include "audio.h"
#include "robot_state.h"
#include "motion.h"

void beep(int number)
{
  for (size_t i = 0; i < number; i++)
  {
    tone(BUZZER_PIN, 1000 / (i + 1));
    delay(100 / number);
    noTone(BUZZER_PIN);
    delay(100 / number);
  }
}

void beepWithForwardWithAlighnment(int number)
{
  for (size_t i = 0; i < number; i++)
  {
    tone(BUZZER_PIN, 1000 / (i + 1));
    delayWithForwardWithAlignment((100 / number), forwardMaxSpeed);
    noTone(BUZZER_PIN);
    delayWithForwardWithAlignment((100 / number), forwardMaxSpeed);
  }
}

void finishTone()
{
  tone(BUZZER_PIN, 523);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 659);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 784);
  delay(300);
  noTone(BUZZER_PIN);
  delay(50);

  tone(BUZZER_PIN, 784);
  delay(200);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 784);
  delay(200);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 880);
  delay(400);
  noTone(BUZZER_PIN);
  delay(50);

  tone(BUZZER_PIN, 784);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 698);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 659);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 587);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 523);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 587);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 659);
  delay(150);
  noTone(BUZZER_PIN);
  delay(30);

  tone(BUZZER_PIN, 784);
  delay(100);
  noTone(BUZZER_PIN);
  delay(20);
  tone(BUZZER_PIN, 880);
  delay(100);
  noTone(BUZZER_PIN);
  delay(20);
  tone(BUZZER_PIN, 784);
  delay(100);
  noTone(BUZZER_PIN);
  delay(20);
  tone(BUZZER_PIN, 880);
  delay(100);
  noTone(BUZZER_PIN);
  delay(20);

  tone(BUZZER_PIN, 523);
  delay(50);
  tone(BUZZER_PIN, 659);
  delay(50);
  tone(BUZZER_PIN, 784);
  delay(50);
  tone(BUZZER_PIN, 1047);
  delay(600);
  noTone(BUZZER_PIN);

  delay(200);

  tone(BUZZER_PIN, 1047);
  delay(200);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 1047);
  delay(200);
  noTone(BUZZER_PIN);
  delay(30);
  tone(BUZZER_PIN, 1047);
  delay(400);
  noTone(BUZZER_PIN);
  delay(50);

  tone(BUZZER_PIN, 523);
  delay(50);
  tone(BUZZER_PIN, 659);
  delay(50);
  tone(BUZZER_PIN, 784);
  delay(50);
  tone(BUZZER_PIN, 1047);
  delay(800);
  noTone(BUZZER_PIN);
}

void startupTone()
{
  for (size_t i = 5; i > 0; i--)
  {
    tone(BUZZER_PIN, 1000 / (i + 1));
    delay(100 / i);
    noTone(BUZZER_PIN);
    delay(100 / i);
  }
}