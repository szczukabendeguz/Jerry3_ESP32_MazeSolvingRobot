#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <string>

// Enum for pin modes
enum HAL_PinMode {
    HAL_INPUT,
    HAL_OUTPUT,
    HAL_INPUT_PULLUP
};

// Enum for digital values
enum HAL_DigitalVal {
    HAL_LOW = 0,
    HAL_HIGH = 1
};

// Interface for Hardware Abstraction Layer
class IHAL {
public:
    virtual ~IHAL() {}

    // Core Arduino-like functions
    virtual void pinMode(int pin, HAL_PinMode mode) = 0;
    virtual void digitalWrite(int pin, HAL_DigitalVal val) = 0;
    virtual int digitalRead(int pin) = 0;
    virtual int analogRead(int pin) = 0;
    virtual void analogWrite(int pin, int val) = 0;
    
    // Time functions
    virtual unsigned long millis() = 0;
    virtual void delay(unsigned long ms) = 0;

    // Serial/Print functions
    virtual void print(const std::string& str) = 0;
    virtual void println(const std::string& str) = 0;

    // IMU functions
    virtual void imuInit() = 0;
    virtual void imuUpdate() = 0;
    virtual float imuGetAngleZ() = 0;

    // RFID functions
    virtual void rfidInit() = 0;
    virtual bool rfidCardPresent() = 0;
    virtual bool rfidReadUID(uint8_t* uid_buffer, uint8_t& uid_size) = 0;
};

// Global HAL instance access
extern IHAL* hal;

#endif // HAL_H
