#include "hal.h"
#include <Arduino.h>
#include <SPI.h>

class ESP32HAL : public IHAL {
public:
    void pinMode(int pin, HAL_PinMode mode) override {
        switch(mode) {
            case HAL_INPUT: ::pinMode(pin, INPUT); break;
            case HAL_OUTPUT: ::pinMode(pin, OUTPUT); break;
            case HAL_INPUT_PULLUP: ::pinMode(pin, INPUT_PULLUP); break;
        }
    }

    void digitalWrite(int pin, HAL_DigitalVal val) override {
        ::digitalWrite(pin, val == HAL_HIGH ? HIGH : LOW);
    }

    int digitalRead(int pin) override {
        return ::digitalRead(pin) == HIGH ? HAL_HIGH : HAL_LOW;
    }

    int analogRead(int pin) override {
        return ::analogRead(pin);
    }

    void analogWrite(int pin, int val) override {
        // Simple mapping, ESP32 might use ledc, but standard analogWrite is sometimes available in newer cores
        // User's original code used analogWrite, so we pass it through
        ::analogWrite(pin, val);
    }

    unsigned long millis() override {
        return ::millis();
    }

    void delay(unsigned long ms) override {
        ::delay(ms);
    }

    void print(const std::string& str) override {
        Serial.print(str.c_str());
    }

    void println(const std::string& str) override {
        Serial.println(str.c_str());
    }

    void imuInit() override {
        // Will be called in main.cpp where MPU is already initialized, or we can move it here
    }

    void imuUpdate() override {
        // Handled in main.cpp for ESP32
    }

    float imuGetAngleZ() override {
        // Return 0 here, actual MPU is called in main.cpp for ESP32
        return 0;
    }

    void rfidInit() override {
    }

    bool rfidCardPresent() override {
        return false;
    }

    bool rfidReadUID(uint8_t* uid_buffer, uint8_t& uid_size) override {
        return false;
    }
};

#ifndef WEBOTS_SIMULATION
IHAL* hal = new ESP32HAL();
#endif
