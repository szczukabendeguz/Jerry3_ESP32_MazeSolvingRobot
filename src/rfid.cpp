#include "rfid.h"
#include "audio.h"
#include "robot_state.h"

int rfidToDirection(int *dirs)
{
  int retVal[4] = {0, 0, 0, 0};
  if (dirs == nullptr)
  {
    dirs = retVal;
  }

  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return -1;
  }

  if (!mfrc522.PICC_ReadCardSerial())
  {
    return -1;
  }

  beepWithForwardWithAlighnment(1);
  if ((mfrc522.uid.uidByte[1] == 0xBC))
  {
    switch (mfrc522.uid.uidByte[2] & 0xF0)
    {
    case 0xC0:
      dirs[0] = DIRECTION_START;
      break;
    case 0x50:
      dirs[0] = DIRECTION_STOP;
      break;
    case 0xF0:
      dirs[0] = DIRECTION_RIGHT;
      break;
    case 0x00:
      dirs[0] = DIRECTION_LEFT;
      break;
    case 0x90:
    case 0xA0:
      dirs[0] = DIRECTION_FRONT;
      dirs[1] = DIRECTION_RIGHT;
      dirs[2] = DIRECTION_RIGHT;
      dirs[3] = DIRECTION_LEFT;
    default:
      dirs[0] = -1;
      break;
    }
  }
  else if (mfrc522.uid.uidByte[1] == 0xBD)
  {
    switch (mfrc522.uid.uidByte[2] & 0xF0)
    {
    case 0xD0:
    case 0xE0:
    case 0xF0:
      dirs[0] = DIRECTION_DEAD_END;
      break;
    case 0x00:
      dirs[0] = DIRECTION_LEFT;
      dirs[1] = DIRECTION_RIGHT;
      dirs[2] = DIRECTION_RIGHT;
      break;
    case 0x60:
      dirs[0] = DIRECTION_RIGHT;
      dirs[1] = DIRECTION_LEFT;
      dirs[2] = DIRECTION_LEFT;
      break;
    default:
      dirs[0] = -1;
      break;
    }
  }

  mfrc522.PICC_HaltA();
  return dirs[0];
}